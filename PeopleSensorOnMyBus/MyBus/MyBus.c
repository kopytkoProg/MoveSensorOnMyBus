/*
* MyBus.c
*
* Created: 2014-12-21 11:01:16
*  Author: michal
*/

#include "MyBus.h"

#define RECEIVING_MSG  0
#define HANDLING_MSG  1

void clear_tx_buffer(void);
uint8_t can_send(void);
volatile char* cp_to_buffer(volatile char *s1, const char *s2);
void send_text(char *s, uint8_t length);
void usart_inicjuj(void);
void send_buffer(uint8_t byte_to_send);
void init_timer0(void);
void enable_timer0(void);
void disable_timer0(void);
void reset_timer0(void);


void set_transmit(void)
{
	PORTD |= (1<<PD2);
}

void set_receive(void)
{
	PORTD &= ~(_BV(PD2));
}

void init_my_buss(void)
{
	usart_tx_bufor_ind = 0;
	usart_rx_bufor_ind = 0;
	init_timer0();															// init timer
	usart_inicjuj();														// initialize USART (RS-232)
	DDRD |= (1<<DDD2);
	set_receive();
}


uint8_t crc (volatile char *s, uint8_t length)
{
	uint8_t crc = 0;
	for (uint8_t i = 0; i < length; i++) crc += *(s+i);
	return crc;
}
//--------------------------------------------------------------
void usart_inicjuj(void)
{

	// set computed by 'setbaud' values
	UBRR0H = UBRRH_VALUE;
	UBRR0L = UBRRL_VALUE;
	#if USE_2X
	UCSR0A |=  (1<<U2X0);
	#else
	UCSR0A &= ~(1<<U2X0);
	#endif
	

	UCSR0C = (1<<UCSZ01) | (1<<UCSZ00);
	// 8 bits
	// 1 bit stop
	// parity none
	UCSR0B = (1<<TXEN0) | (1<<RXEN0) | (1<<RXCIE0)| (1<<TXCIE0);
}
//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
void clear_tx_buffer(void)
{
	for (int i = 0; i < TX_BUFFER_SIZE; i++ ) usart_tx_bufor[i] = 0;
}
//--------------------------------------------------------------
uint8_t volatile data_to_send = 0;
uint8_t can_send(void)
{
	return ((UCSR0A & (1<<UDRE0)) && !data_to_send);
}
//--------------------------------------------------------------
volatile char* cp_to_buffer(volatile char *s1, const char *s2)
{
	volatile char *s = s1;
	while ((*s++ = *s2++) != 0);
	return (s1);
}
//--------------------------------------------------------------
void send_text(char *s, uint8_t length)
{
	if (can_send())
	{
		cp_to_buffer(usart_tx_bufor, s);
		send_buffer(length);
	}
}
void send_data (volatile char *d, uint8_t length)
{
	if (can_send()){
		volatile char *s = d;
		for (uint8_t i = 0; i < length; i++) usart_tx_bufor[i] = *(s+i);
		send_buffer(length);
	}
}
//--------------------------------------------------------------
//--------------------------------------------------------------
ISR(USART_TX_vect)
{
	if (data_to_send==0) set_receive();
}
ISR(USART_UDRE_vect)
{
	if(usart_tx_bufor_ind < TX_BUFFER_SIZE && data_to_send > 0)
	{
		UDR0 = usart_tx_bufor[usart_tx_bufor_ind++];
		data_to_send--;
	}
	else
	{
		usart_tx_bufor_ind = 0;
		UCSR0B &= ~(1<<UDRIE0);												//wy³¹cz przerwania pustego bufora nadawania
		data_to_send = 0;
	}
}
//--------------------------------------------------------------
void send_buffer(uint8_t byte_to_send)
{
	data_to_send = byte_to_send;
	usart_tx_bufor_ind = 0;
	set_transmit();
	UCSR0B |= (1<<UDRIE0);
}
//--------------------------------------------------------------
//--------------------------------------------------------------
uint8_t volatile recive_counter = 0;
uint8_t volatile state = RECEIVING_MSG;
/**
After call can be received new message
*/
void set_last_message_as_handled(void)
{
	usart_rx_bufor_ind = 0;
	recive_counter = 0;
	state = RECEIVING_MSG;
}

//--------------------------------------------------------------
/**

*/
uint8_t is_message_ready(void)
{
	return state == HANDLING_MSG;
}

//--------------------------------------------------------------
ISR(USART_RX_vect)
{
	if(state == RECEIVING_MSG)
	{
		if(recive_counter < RX_BUFFER_SIZE)
		{
			usart_rx_bufor[recive_counter++] = UDR0;
			
			if(recive_counter == 1)
			{
				reset_timer0();
				enable_timer0();
			}
			else if(recive_counter < 3)
			{
				reset_timer0();
			}
			else if(recive_counter < usart_rx_bufor[MSG_DATA_LENGTH] + 4)		// Data and crc
			{
				reset_timer0();
			}
			else //if (recive_counter == usart_rx_bufor[MSG_DATA_LENGTH] + 4)		//success
			{
				disable_timer0();
				usart_rx_bufor_ind = recive_counter;
				state = HANDLING_MSG;
				recive_counter = 0;
			}

		}
		else																		// To many received data, it is error
		{
			disable_timer0();														// Clear all data
			recive_counter = 0;
			usart_rx_bufor_ind = 0;													// Clear all data
		}
	}
	else																			// New data com while previews data are not handled
	{
		
	}
}
//--------------------------------------------------------------
void init_timer0(void)
{
	TCCR0A |= (1<<WGM01);													// CTC Mode
	OCR0A = 255;															// TOP Value
	TIMSK0 |= (1<<OCIE0A);													// compare match interrupt enable
}
//--------------------------------------------------------------
void enable_timer0(void)
{
	TCCR0B |= (1 << CS02) | (1 << CS00);									// Prescaler
}
//--------------------------------------------------------------
void disable_timer0(void)
{
	TCCR0B &= ~((1 << CS02) | (1 << CS01) | (1 << CS00));					// Prescaler
}
//--------------------------------------------------------------
void reset_timer0(void)
{
	TCNT0 = 0;																// Reset counter
}
//--------------------------------------------------------------
ISR (TIMER0_COMPA_vect)														// Failed transmission
{
	disable_timer0();
	recive_counter = 0;														// Clear All buffers
	usart_rx_bufor_ind = 0;													// Clear All buffers
	state = RECEIVING_MSG;
	//PORTB ^= (_BV(PB1));

}
//--------------------------------------------------------------