# Shed Light Control

This project is to put [The Shed](https://www.cs.kent.ac.uk/makerspace/)'s
lighting controls on the internet, and to monitor the air quality in The Shed
throughout the day.

You will want to make this point to your server. To do this, change `MQTT_HOST`
and `MQTT_PORT` as defined in `mqtt_client.h`.

## Building

You should be able to do:

```bash
make -j4
cp ./frdm_shed_lights.bin G:/ # replace with your mbed's drive letter
```

to build and deploy to an FRDM K64F development board.

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
