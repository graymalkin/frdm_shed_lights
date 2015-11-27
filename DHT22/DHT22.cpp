/*
 * (C) The University of Kent and Simon Cooksey 2015.
 */

#include "mbed.h"
#include <inttypes.h>

#include "DHT22.h"

/*
 * The DHT22 uses a 1 wire interface, sending 1's and 0s by varying the length
 * of the HIGH time on the signal pin.
 */

/*
 * Wait for a rising or falling edge on the sense pin.
 *
 * Returns: the number of uS elapsed before the transition
 */
int DHT22::wait_for_edge(edge_type_t type)
{
	dht22_s.input();

	// Timing is done by increasing this number, as the Timer class appears to
	// be super slow.
	int time = 0;
	do {
		wait_us(2);
		time+=2;
	} while(dht22_s != (int)type);

	// wait for the edge to transition properly
	wait_us(2);
	return time;
}

/*
 * Send a start bit to the DHT22
 */
void DHT22::send_start()
{
	dht22_s.output();
	dht22_s = 0;
	wait_us(DHT22_START_BIT_TIME);
	dht22_s = 1;
	dht22_s.input();
}

/*
 * Wait for the DHT22 to send the start bit ACK, after this we can read data.
 */
void DHT22::await_start_response()
{
	dht22_s.input();
	wait_for_edge(EDGE_TYPE_FALLING); // 20-40 uS
	wait_for_edge(EDGE_TYPE_RISING);  // 80 uS
	wait_for_edge(EDGE_TYPE_FALLING); // 80 uS
}

/*
 * Reads 16 bits of data from the DHT22
 *
 * Returns: the signed value read. dht22_t.
 * NB. the DHT22 uses a sign bit to do -ve and positive, but this is
 * incompatible with signed numbers in C, so the conversion is done here.
 */
int16_t DHT22::read_word()
{
	dht22_s.input();
	int32_t duration;
	int16_t word = 0x00;
	for(char bit = 0; bit < 16; bit++)
	{
		/*       /-----------\
		 *      /   duration  \    50us
		 * ----/               \-------
		 */
		wait_for_edge(EDGE_TYPE_RISING);
		duration = wait_for_edge(EDGE_TYPE_FALLING);

		if(duration > DHT22_SIGNAL_HIGH_LOW_BOUNDARY)
		{
			word |= (1 << 15-bit);
		}
		else
		{
			word |= (0 << 15-bit);
		}
	}
	if(word & 0x8000)
		return 1 - (word ^ 0x8000);
	return word;
}

/*
 * Reads 8 bits of data from the DHT22
 *
 * Returns: the unsigned checksum value read. dht22_t.
 */
uint8_t DHT22::read_checksum()
{
	dht22_s.input();
	uint32_t duration;
	uint8_t word;
	for(char bit = 0; bit < sizeof(uint8_t)*8; bit++)
	{
		/*       /-----------\
		 *      /   duration  \    50us
		 * ----/               \-------
		 */
		wait_for_edge(EDGE_TYPE_RISING);
		duration = wait_for_edge(EDGE_TYPE_FALLING);

		if(duration > DHT22_SIGNAL_HIGH_LOW_BOUNDARY)
		{
			word |= (1 << 7-bit);
		}
		else
		{
			word |= (0 << 7-bit);
		}
	}
	return word;
}

/*
 * Reads a packet of DHT22 data.
 *
 * Param data: the packet to fill.
 */
void DHT22::read(DHT22_data_t * data)
{
	// Send start bits
	send_start();

	// Wait for device to respond
	await_start_response();

	// Read data bits (16+16)
	int16_t humidity = read_word();
	int16_t temp = read_word();

	// Read checksum  (8)
	uint8_t checksum = read_checksum();

	data->humidity = (humidity);
	data->temp = (temp);
	data->checksum = checksum;
}
