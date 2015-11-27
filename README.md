# Shed Light Control

This project is to put [The Shed](https://www.cs.kent.ac.uk/makerspace/)'s
lighting controls on the internet, and to monitor the air quality in The Shed
throughout the day.

## Sensors

| Sensor         | Endpoint                                          |
|:--------------:|:--------------------------------------------------|
| DHT22          | MQTT Published `sjc80/dht22`                      |
| Particle count | MQTT Published `sjc80/particle_count`             |

## State

| Sensor         | Endpoint                                          |
|:--------------:|:--------------------------------------------------|
| State          | MQTT Published `sjc80/light_state`                |

## Control

| Sensor         | Endpoint                                          |
|:--------------:|:--------------------------------------------------|
| Set lights     | MQTT Subscribes `sjc80/set_lights`                |


## License

Parts of this work are &copy; The University of Kent and Simon Cooksey.
Various libraries are under separate licenses and have individual copy right
holders.
