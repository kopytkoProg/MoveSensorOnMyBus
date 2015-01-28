/*
 * PeopleSensorOnMyBus.h
 *
 * Created: 2015-01-26 14:51:23
 *  Author: michal
 */ 


#include "MyBus/MyBus.h"

#include <avr/io.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <util/delay.h>

//=================================================================================================================================
#define MY_ADDRESS 30
#define MASTER_ADDRESS 1

// COMMANDS :

// Counter
#define CMD_GET_COUNTER								1	
#define CMD_GET_64b_COUNTER							2

// Other
#define CMD_HELLO_WORLD								200		// Hello word



//#define MSG_CRC
//=================================================================================================================================