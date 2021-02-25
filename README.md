# dht11-prometheus-exporter

## Description

Prometheus exporter for DHT-11 temperature and humidity sensor.

## Dependencies

```bash
sudo apt install libmicrohttpd-dev
```

```bash
libprom-dev
libpromhttp-dev
```
available [here](https://github.com/digitalocean/prometheus-client-c)

```bash
libwiringpi-dev
```
available [here](https://github.com/WiringPi/WiringPi)

## Compiling

```bash
$ make
# make install
```

## Usage

```bash
dht11-prometheus -g 2 -p 9111 -r 60
```

```text
-g [GPIO port]
-p [Export to Prometheus (default port 9111)]
-r [poll interval in seconds]
```

see systemd service file for using as a system service


Add the following in your Prometheus config file, `prometheus.yml`, under the section `scrape_configs:`

```text
  - job_name: dht11
    static_configs:
      - targets: ['localhost:9111']
```


## Licence

GPL Version 3

See `COPYING` for details
