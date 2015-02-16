/*
* PeopleSensorOnMyBus.c
*
* Created: 2015-01-26 14:49:18
*  Author: michal
*/


#include "PeopleSensorOnMyBus.h"
#define DIODES_PORT (_BV(DDB0)|_BV(DDB1)|_BV(DDB6)|_BV(DDB7))
//--------------------------------------------------------------

//--------------------------------------------------------------
int main(void)
{
	
	
	
	DDRB |= DIODES_PORT;													// set out PB (for diodes)
	DDRD &= ~_BV(DDD7);														// set IN PD7
	
	init_my_buss();
	sei();
	// Enables interrupts
	uint8_t last_status = 0;
	uint8_t actual_status = 0;
	uint8_t counter = 0;
	uint64_t counterLong = 	0;
	
	while(1)
	{
		
		// Get sesnor status and show it using LED
		if (PIND & _BV(PIND7))
		{
			actual_status = 1;
			PORTB |= (_BV(PB0));
		}
		else
		{
			actual_status = 0;
			PORTB &= ~(_BV(PB0));
		}
		
		// Increase counter if status hanged
		if (last_status != actual_status) {counter++; counterLong++;}
		
		last_status = actual_status;
		
		// My bus LOOP
		if (is_message_ready())	// great or equal 4 because packet always have address, command,  length and CRC
		{																							//(End is when usart_rx_bufor_ind is equals 4(addr, cmd, length, crc) + usart_rx_bufor_ind[1](length of data field))
			
			if(usart_rx_bufor[usart_rx_bufor_ind-1] == crc(usart_rx_bufor, usart_rx_bufor_ind - 1)	// check CRC
			&& usart_rx_bufor[MSG_ADDRESS] == MY_ADDRESS)											// check Address
			{
				//_delay_ms(10);
				//char msg[] = {1, 2, 2, 'O', 'K', 0};
				usart_rx_bufor[0] = MASTER_ADDRESS;
				
				switch (usart_rx_bufor[MSG_COMMAND])
				{
					
					//--------------------------------------------------------------
					
					case CMD_GET_COUNTER:
					
					usart_rx_bufor[MSG_DATA_LENGTH] = 1;
					usart_rx_bufor[MSG_DATA_START] = counter;
					usart_rx_bufor_ind = 4 + 1;
					
					break;
					
					//--------------------------------------------------------------
					case CMD_GET_64b_COUNTER:
					
					usart_rx_bufor[MSG_DATA_LENGTH] = 8;
					
					uint64_t *v = (uint64_t *)&usart_rx_bufor[MSG_DATA_START];
					(*v) = counterLong;
					
					usart_rx_bufor_ind = 4 + 8;
					
					break;
					
					//--------------------------------------------------------------
					
					case CMD_HELLO_WORLD:
					// nothing to do
					break;
					
					//--------------------------------------------------------------
					
					default:
					/* UNKNOWN */
					break;
				}
				usart_rx_bufor[usart_rx_bufor_ind - 1] = crc(usart_rx_bufor, usart_rx_bufor_ind - 1);
				send_data(usart_rx_bufor, usart_rx_bufor_ind);					// Send response
			}
			set_last_message_as_handled();//usart_rx_bufor_ind = 0;											// Now can
		}
	}
}
//--------------------------------------------------------------
//--------------------------------------------------------------


