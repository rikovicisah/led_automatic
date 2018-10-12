/*
 * LDR_automatic_LED.c
 *
 * Created: 2.10.2018 23:05:03
 * Author : Irhad
 */ 

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
 
#define F_CPU 1000000UL
#define BIT_IS_SET(byte, bit) (byte & (1 << bit))
#define BIT_IS_CLEAR(byte, bit) (!(byte & (1 << bit)))

typedef enum{false, true} bool;
	
static volatile unsigned int num = 0;

bool previousState = false;
bool start = false;

void setup();
void loop();

void ADC_init();
void ADC_start();
void ADC_end();

void EI_init(); // External Interrupt

int main(void)
{
    setup();
	loop();
}

void setup(){
	DDRC &= ~(0x1); // LDR Input
	DDRB = 0xFF; //LEDs value Output
	
	DDRD = 0x3; //LED light LED start Output Button Input
	PORTD |= (1 << PIND2); //Button pull
}

void loop(){
	EI_init();
	sei();
	while(1){
		
		if(num%2==1){
			PORTD |= (1 << PIND1);
			ADC_start();
		}else{
			PORTD &= ~(1 << PIND1);
			ADC_end();
		}
		
	}
		
}

//------------------------------------------------------------------------
void ADC_start(){
	ADC_init();
	ADCSRA |= (1 << ADSC);
}

void ADC_end(){
	ADCSRA = 0x0;
	ADCH = 0x0;
	PORTB = 0x0;
	PORTD &= ~(1 << PIND0);
}

void ADC_init(){
	ADMUX = 0x60;
	ADCSRA = 0x8B;
	ADCSRB = 0x0;
	ADCH = 0x0;
}

ISR(ADC_vect) {
	PORTB = ADCH;	// assign contents of ADC high register to Port D pins
	//int b = (int)ADCH;
	if((int)ADCH > 220)	{ //100
		PORTD |= 0x1;
	}else{
		PORTD &= ~(0x1);
	}
	_delay_ms(100);
	ADCSRA |= (1 << ADSC);		// start next ADC
}
//------------------------------------------------------------------------
//Button interrupt
//------------------------------------------------------------------------
void EI_init(){
	//EICRA = (1 << ISC01); //falling edge INT0
	EIMSK = (1 << INT0);
}

ISR(INT0_vect){
	num++;
}
//------------------------------------------------------------------------