# Shed Light Control

This project is to put [The Shed](https://www.cs.kent.ac.uk/makerspace/)'s
lighting controls on the internet, and to monitor the air quality in The Shed
throughout the day.

You will want to make this point to your server. To do this, change `MQTT_HOST`
and `MQTT_PORT` as defined in `mqtt_client.h`.

## Features

- MQTT support
 - Sensor data reported once per 5s over MQTT_PORT
 - Light state may be changed by publishing mesasges to the `sjc80/set_lights`
   topic
- Sensors available:
 - DHT22 Humidity and Temperature sensor
 - GP2Y10 Particle Counter (_experimental_)

There is no security built in. You can run this on a LAN only, which is
preferential to exposing the light switch over the internet; however to keep
the client simple (all `js` speaking to MQTT) adding authentication was not a
sensible design option.

## Building

With the gcc arm tools available on your path, you should be able to do:

```bash
make -j4

# modify for wherever you mount your mbed
cp ./frdm_shed_lights.bin /media/mbed
```

to build and deploy to an FRDM K64F development board.

On Windows, I suggest installing Cygwin with GNU Make to execute the
Makefile.

## MQTT
### Sensors

| Sensor         | Endpoint                                          |
|:--------------:|:--------------------------------------------------|
| DHT22          | MQTT Published `sjc80/dht22`                      |
| Particle count | MQTT Published `sjc80/particle_count`             |

### State

| Sensor         | Endpoint                                          |
|:--------------:|:--------------------------------------------------|
| State          | MQTT Published `sjc80/light_state`                |

### Control

| Sensor         | Endpoint                                          |
|:--------------:|:--------------------------------------------------|
| Set lights     | MQTT Subscribes `sjc80/set_lights`                |


## License

Parts of this work are &copy; The University of Kent and Simon Cooksey.
Various libraries are under separate licenses and have individual copy right
holders.
