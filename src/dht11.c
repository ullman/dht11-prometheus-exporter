/*
Copyright (C) 2021  Henrik Ullman
License: GPL Version 3
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <wiringPi.h>
#include <unistd.h>
#include <math.h>

#include <prom.h>
#include <promhttp.h>
#include <microhttpd.h>

#include "prometheus.h"

#define TIMEOUT 85
#define INDICATION_TRESHOLD 28



void dht11_init(int gpio)
{
  pinMode(gpio, OUTPUT);
  digitalWrite(gpio, LOW);
  delay(18);
  digitalWrite(gpio,HIGH);
  delayMicroseconds(40);
  pinMode(gpio,INPUT);
}

int dht11_verify(int *values)
{
	if(values[4]==((values[0]+values[1]+values[2]+values[3])& 0xFF))
	{
		return 0;
	}
	else{
		printf("Invalid checksum, discarding measurement...");
			return 1;
	}
}

void dht11_send(int *values)
{
	int p_status;
	float temp;
	float humidity;

	temp = (float)values[2]+(float)values[3]/10;
	humidity = (float)values[0]+(float)values[1]/10;

	p_status = log_prometheus ("temperature", roundf(temp*10.0)/10.0);
	p_status = log_prometheus ("humidity", roundf(humidity*10.0)/10.0);
	if(p_status){
		   fprintf(stderr, "error exporting variable to prometheus\n");
							                   }

}


void dht11_read(int gpio)
{
  int indication_time;
  int i;
  int dht_state=HIGH;
  int bit=0;
  int values[5] = {0,0,0,0,0};

  //Send request to DHT
  dht11_init(gpio);
  for(i=0; i<TIMEOUT; i++)
    {
      indication_time = 0;

      while((digitalRead(gpio)==dht_state) && (indication_time != 100)){
        indication_time++;
        delayMicroseconds(1);
      }
      dht_state=digitalRead(gpio);

      if (indication_time == 100){
        break;
      }

      if((i>=4) && (i%2==0))
        {
          values[bit/8] <<=1;
          if(indication_time > INDICATION_TRESHOLD){
            values[bit/8] |=1;
          }
          bit++;
        }
    }
  if(!dht11_verify(values)){
	  dht11_send(values);
  }
}

int main (int argc, char *argv[])
{

  int gpio;
  int c;
  unsigned short port=9111;
  unsigned short poll_frequency=3000;

  while ((c = getopt (argc, argv, "g:r:p:")) != -1)
    {
      switch (c)
        {
        case 'g':
          gpio = atoi(optarg);
          printf ("Reading input from serial device: %i\n", gpio);
          break;
        case 'r':
          poll_frequency = atoi(optarg)*1000;
          printf ("Poll frequency: %i\n", poll_frequency/1000);
          break;
        case 'p':
          port = atoi(optarg);
          printf ("Running prometheus exporter on port: %i\n", port);
          break;
        case '?':
          if (optopt == 'g')
            {
              fprintf (stderr, "Please specify gpio port [-g gpio_port]\n");
            }
          else if (optopt == 'r')
            {
              fprintf (stderr, "Please specify poll frequency [-r seconds]\n");
            }
          else if (optopt == 'p')
            {
              fprintf (stderr, "Please specify port [-p port]\n");
            }
          else
            {
              fprintf (stderr, "Unknown option -%c\n", optopt);

            }
          return 1;
        default:
          abort ();
        }
    }
  //setup wiringpi
	    if (wiringPiSetup()== -1 ){
		exit(1);
	    }
  //setup prometheus
  init_prometheus();
  init_metric_prometheus();
  promhttp_set_active_collector_registry(NULL);
  struct MHD_Daemon *daemon = promhttp_start_daemon(MHD_USE_SELECT_INTERNALLY, port, NULL, NULL);
  if (daemon == NULL){
	  fprintf (stderr, "Error initializing prometeus exporter.\n");
	  exit (1);
  }


  while(1)
    {
      dht11_read (gpio);
     delay(poll_frequency);
    }
  printf ("exiting...\n");
  close_prometheus();

}
