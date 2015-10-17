/*
 * (C) The University of Kent and Simon Cooksey 2015.
 */

#ifndef __DHT22_h_
#define __DHT22_h_

// We'll pick a point to decide if a signal is 1 or 0 from. 
#define DHT22_SIGNAL_HIGH_LOW_BOUNDARY      50   // uS
#define DHT22_START_BIT_TIME                500  // uS
#define DHT22_START_BIT_RESPONSE            80   // uS

typedef enum {
	EDGE_TYPE_FALLING,
	EDGE_TYPE_RISING,
} edge_type_t;

typedef struct {
	int16_t temp;
	int16_t humidity;
	uint8_t checksum;
} DHT22_data_t;

class DHT22 {
public:
	DHT22(PinName pin)  : dht22_s(pin) { }
	void read(DHT22_data_t * data);
private:
	DigitalInOut dht22_s;
	Timer dht22_t;

	int wait_for_edge(edge_type_t type);
	void send_start();
	void await_start_response();
	int16_t read_word();
	uint8_t read_checksum();
};

#endif // __DHT22_h_