# Shed Light Control

This project is to put [The Shed](https://www.cs.kent.ac.uk/makerspace/)'s
lighting controls on the internet, and to monitor the air quality in The Shed
throughout the day.

## Sensors

| Sensor         | Endpoint                                          |
|:--------------:|:--------------------------------------------------|
| Temperature    | GET `/query/temperature`                          |
| Humidity       | GET `/query/humidity`                             |
| Particle count | GET `/query/particle`                             |

## State

| Sensor         | Endpoint                                          |
|:--------------:|:--------------------------------------------------|
| State          | GET `/query/lights`                               |

## Control

| Sensor         | Endpoint                                          |
|:--------------:|:--------------------------------------------------|
| Lights on      | GET `/on`                                         |
| Lights off     | GET `/off`                                        |
| Lights dim1    | GET `/dim1`                                       |
| Lights dim2    | GET `/dim2`                                       |


## License

Parts of this work are &copy; The University of Kent and Simon Cooksey.
Various libraries are under separate licenses and have individual copy right
holders.

