/*
Copyright (C) 2021  Henrik Ullman
License: GPL Version 3
*/

#include <prom.h>
#include <promhttp.h>
#include <microhttpd.h>

#include "prometheus.h"


prom_gauge_t *dht_gauge;


void	init_prometheus(void){
  prom_collector_registry_default_init();
}

void init_metric_prometheus(void){
  dht_gauge = prom_collector_registry_must_register_metric (prom_gauge_new ("dht_gauge", "exports dht11 sensor data", 1, (const char *[]) {"label"}));
}

int log_prometheus(const char* label, double value){
  return prom_gauge_set (dht_gauge, value, (const char *[]) { label });
}

void close_prometheus(void){
  prom_collector_registry_destroy (PROM_COLLECTOR_REGISTRY_DEFAULT);
}

