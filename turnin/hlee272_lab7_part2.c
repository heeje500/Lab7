/*	Author:Heeje Lee
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #7  Exercise #2
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#include "io.h"
#endif

volatile unsigned char TimerFlag = 0;

unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;

void TimerOn() {
	TCCR1B = 0x0B;
	OCR1A = 125;
	TIMSK1= 0x02;
	TCNT1 = 0;
	_avr_timer_cntcurr = _avr_timer_M;
	SREG |= 0x80;
}

void TimerOff() {
	TCCR1B = 0x00;
}

void TimerISR() {
	TimerFlag = 1;
}

ISR(TIMER1_COMPA_vect){
	_avr_timer_cntcurr--;
	if (_avr_timer_cntcurr ==0) {
		TimerISR();
		_avr_timer_cntcurr = _avr_timer_M;
	}
}

void TimerSet(unsigned long M) {
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}

enum States{init, zero, one, two, wait, wait2, winner, restart} state;

unsigned char count;
void Tick() {
	
	static unsigned char score = 5;
	switch(state) { //transitions
		case init:
			PORTB = 0x00;
			state = zero;
			break;

		case zero:
			if ((~PINA & 0x01) == 0x01) {
				if (score > 0) {
					score--;
				}
				else {
					score = 0;
				}
				
				state = wait;
			}

			else {
				state = one;
			}
			count = 0x01;
			break;

		case two:
			if ((~PINA & 0x01) == 0x01) {
				if(score > 0) {
					score--;
				}
				else {
					score = 0;
				}
				state = wait;
			}
			else {
				state = one;
			}
			count = 0;
			break;

		case one:
			
			if ((~PINA & 0x01) == 0x01) {
				if (score < 9) {
					score++;
				}
				else {
					score = 9;
				}
				state = wait;
			}
			else {
				if (count == 0) {
					state = zero;
				}
				else if (count == 1) {
					state = two;
				}
			}			
			break;

		case wait:
			if ((~PINA & 0x01) == 0x01) {
				state = wait;
			}
			else {
				state = wait2;
			}
			break;

		case wait2:
			if (score >= 9) {
				state = winner;
			}

			else if ((~PINA & 0x01) == 0x01) {
				state = restart;
			}

			else {
				state = wait2;
			}
			break;

		case winner:
			if ((~PINA & 0x01) == 0x01) {
				state = restart;
			}
			else {
				state = winner;
			}
			break;

		case restart:
			if ((~PINA & 0x01) == 0x01) {
				state = restart;
			}

			else {
				state = init;
			}
			break;


		default:
			state = init;
			break;
	}

	switch(state) { //actions

		case init: 
			LCD_DisplayString(1, "Score:");
			break;

		case zero:
			PORTB = 0x01;
			LCD_Cursor(7);
			LCD_WriteData(score + '0');
			break;

		case one:
			PORTB = 0x02;
			LCD_Cursor(7);
			LCD_WriteData(score + '0');
			break;

		case two: 
			PORTB = 0x04;
			LCD_Cursor(7);
			LCD_WriteData(score + '0');
			break;

		case wait:
			LCD_Cursor(7);
			LCD_WriteData(score +'0');
			break;
		
		case wait2:
			break;

		case winner:
			LCD_DisplayString(1, "WINNER");
			score = 0;
			break;

		case restart:
			break;

		default:
			break;
		}
}

int main(void) {
    /* Insert DDR and PORT initializations */
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	DDRC = 0xFF; PORTC = 0x00;
	DDRD = 0xFF; PORTD = 0x00;

	LCD_init();

	TimerSet(100);
	TimerOn();
	state = init;
	LCD_DisplayString(1, "Score:");
    /* Insert your solution below */
    while (1) {
	    Tick();
	    while(!TimerFlag){}
	    TimerFlag = 0;
    }
    return 1;
}
