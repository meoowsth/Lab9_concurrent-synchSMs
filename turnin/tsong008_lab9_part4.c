/*	Author: lab
 *  Partner(s) Name: Tinghui Song
 *	Lab Section: 24
 *	Assignment: Lab #9  Exercise #4
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 * 
 *	Demo URL: https://drive.google.com/file/d/1vx-tljuYiK9F_vjYB7nfpbw-tUkmYV5k/view?usp=sharing
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif
#include "Timer.h"

unsigned char tempA;
unsigned char temp_BL = 0x00;
unsigned char temp_TL = 0x00;
unsigned char temp_Speaker = 0x00;
unsigned char freq = 2;

enum ThreeLEDs_States{ TL_Start, TL_1, TL_2, TL_3} ThreeLEDs_State;

void Tick_ThreeLEDs(){
	switch(ThreeLEDs_State){//Transitoins
		case TL_Start:
			ThreeLEDs_State = TL_1;
			break;
		case TL_1:
			ThreeLEDs_State = TL_2;
			break;
		case TL_2:
			ThreeLEDs_State = TL_3;
			break;
		case TL_3:
			ThreeLEDs_State = TL_1;
			break;
		default:
			break;
	}
	switch(ThreeLEDs_State){//Actions
		case TL_Start:
			break;
		case TL_1:
			temp_TL = 0x01;
			break;
		case TL_2:
			temp_TL = 0x02;
			break;
		case TL_3:
			temp_TL = 0x04;
			break;
		default:
			break;
	}
}

enum BlinkingLED_States{BL_Start, BL_On, BL_Off} BlinkingLED_State;
void Tick_BlinkingLED(){
	switch (BlinkingLED_State){//Transitions
		case BL_Start:
			BlinkingLED_State = BL_On;
			break;
		case BL_On:
			BlinkingLED_State = BL_Off;
			break;
		case BL_Off:
			BlinkingLED_State = BL_On;
			break;
		default:
			break;
	}
	switch (BlinkingLED_State){//Actions
		case BL_Start:
			break;
		case BL_On:
			temp_BL = 0x08;
			break;
		case BL_Off:
			temp_BL = 0x00;
			break;
		default:
			break;
	}
		 
}

enum Speaker_States{ Speaker_Start, Speaker_Wait, Speaker_On} Speaker_State;

void Tick_Speaker(){
	switch(Speaker_State){//Transitions
		case Speaker_Start:
			Speaker_State = Speaker_Wait;
			break; 
		case Speaker_Wait:
			Speaker_State = (tempA == 0x04)? Speaker_On:Speaker_Wait;
			break; 
		case Speaker_On:
			Speaker_State = Speaker_Wait;
			break; 
		default:
			break; 
	}
	switch(Speaker_State){ //Actions
		case Speaker_Start:
			break; 
		case Speaker_Wait:
			temp_Speaker = 0x00;
			break; 
		case Speaker_On:
			temp_Speaker = 0x10;
			break; 
		default:
			break; 
	}
}

enum Frequency_States{ Frequency_Start, Frequency_Wait, Frequency_Inc, Frequencey_Dec}Frequency_State;

void Tick_Frequency(){
	switch(Frequency_State){ //transistions state
		case Frequency_Start:
			Frequency_State = Frequency_Wait;
			break;
		case Frequency_Wait:
			if((tempA & 0x01) == 0x01){
				Frequency_State = Frequency_Inc;
			}else if((tempA & 0x02) == 0x02){
				Frequency_State = Frequencey_Dec;
			}
			break;
		case Frequency_Inc:
			Frequency_State = Frequency_Wait;
			break;
		case Frequencey_Dec:
			Frequency_State = Frequency_Wait;
			break;
		default:
			break;	
	}

	switch(Frequency_State){ //action states
		case Frequency_Start:
			break;

		case Frequency_Wait:
			break;

		case Frequency_Inc:
			freq++;
			break;
		case Frequencey_Dec:
			if(freq >= 1){
				freq--;
			}
			break;
		default:
			break;	
	}
}

enum Output_States{Output_Start, Output_Run} Output_State;

void OutputLED(){
	static unsigned char tempB = 0x00;
	switch (Output_State){//Transitions
		case Output_Start:
			Output_State = Output_Run;
			break;
		case Output_Run:
			Output_State = Output_Run;
			break;
	}
	switch (Output_State){//Ations
		case Output_Start:
			break;
		case Output_Run:
			tempB = temp_BL | temp_TL | temp_Speaker;
			break;
	}
	PORTB = tempB;
}


int main(void) {
    /* Insert DDR and PORT initializations */
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;

    /* Insert your solution below */
	TimerSet(1);
 	TimerOn();

	ThreeLEDs_State = TL_Start;
	BlinkingLED_State = BL_Start;
	Speaker_State = Speaker_Start;
	Output_State = Output_Start;
	Speaker_State = Speaker_Start;
	Frequency_State = Frequency_Start;

	unsigned long BL_elapsedTime = 0;
	unsigned long TL_elapsedTime = 0;
	unsigned long Speaker_elapsedTime = 0;
	unsigned long Frequency_elapsedTime = 0;
	const unsigned long timerPeriod = 1;

    while (1) {
	tempA = ~PINA;
	if (BL_elapsedTime >= 1000) {
		Tick_BlinkingLED();
		BL_elapsedTime = 0;
	}
	if (TL_elapsedTime >= 300) {
		Tick_ThreeLEDs();
		TL_elapsedTime = 0;
	}
	if (Speaker_elapsedTime >= freq){
		Tick_Speaker();
		Speaker_elapsedTime = 0;
	}
	if (Frequency_elapsedTime >= 20 ){
		Tick_Frequency();
		Frequency_elapsedTime = 0;
	}
	OutputLED();
	while (!TimerFlag) {}
	TimerFlag = 0;
	BL_elapsedTime += timerPeriod;
	TL_elapsedTime += timerPeriod;
	Speaker_elapsedTime += timerPeriod;
	Frequency_elapsedTime += timerPeriod;
    }
    return 1;
}
