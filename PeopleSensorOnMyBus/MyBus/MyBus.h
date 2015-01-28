/*
* MyBus.h
*
* Created: 2014-12-21 11:04:53
*  Author: michal
*/


#ifndef MYBUS_H_
#define MYBUS_H_

#ifndef F_CPU
	#define F_CPU 8000000UL
#endif
/*
* IncFile1.h
*
* Created: 2014-12-20 12:38:36
*  Author: michal
*/

//#include <stddef.h>
#include <avr/io.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <util/delay.h>


//definiowanie parametrów transmisji za pomoc¹ makr zawartych w pliku
//nag³ówkowym setbaud.h. Je¿eli wybierzesz prêdkoœæ, która nie bêdzie
//mo¿liwa do realizacji otrzymasz ostrze¿enie:
//#warning "Baud rate achieved is higher than allowed"

#define BAUD 38400 //250000	//9600 //5760											//tutaj podaj ¿¹dan¹ prêdkoœæ transmisji
// rs-485 
//	|speed			|distance		|
//	|250000			|~300m			|
#include <util/setbaud.h>													//linkowanie tego pliku musi byæ po zdefiniowaniu BAUD

#define TX_BUFFER_SIZE 128
#define RX_BUFFER_SIZE 128

// =================== Fields of message ===================

#define MSG_ADDRESS			0
#define MSG_COMMAND			1
#define MSG_DATA_LENGTH		2												// Data length field		
#define MSG_DATA_START		3												// Place where the data start 

// =========================================================

volatile unsigned int usart_tx_bufor_ind;									//transmit buffer index  
char volatile usart_tx_bufor[TX_BUFFER_SIZE];								//transmit buffer 

volatile unsigned int usart_rx_bufor_ind;									//receive buffer index, contain size of received data. New value set when new massage received completed
char volatile usart_rx_bufor[RX_BUFFER_SIZE];								//receive buffer


void send_data (volatile char *s, uint8_t length);
void init_my_buss(void);
uint8_t crc (volatile char *s, uint8_t length);
void set_last_message_as_handled(void);
uint8_t is_message_ready(void);

#endif /* MYBUS_H_ */