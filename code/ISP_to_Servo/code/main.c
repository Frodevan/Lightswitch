/*
 * GccApplication2.c
 *
 * Created: 23.08.2017 16.42.56
 * Author : medlem
 */ 

#define F_CPU 8000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

void setup_servo();
void lyspaa();
void lysav();
void setup_com();
ISR(USI_OVF_vect);
void delay(int);

volatile uint_fast8_t cominn = 0xFF;

int main(void)
{
	setup_com();
	setup_servo();
	int charcount = 0;
	
	
	
    while (1) 
    {
		uint_fast8_t data = 0xFF;
		cli();
		if(cominn != 0xFF)
		{
			data = cominn;
			cominn = 0xFF;
		}
		sei();
		
		if (data != 0xFF)
			switch(charcount)
			{
				case 0:
					if (data == 'O') charcount++;
					else charcount = 0;
					break;
				case 1:
					if (data == 'V') charcount++;
					else charcount = 0;
					break;
				case 2:
					if (data == ' ') charcount++;
					else charcount = 0;
					break;
				case 3:
					if (data == 'O') charcount++;
					else charcount = 0;
					break;
				case 4:
					if (data == 'N') {lyspaa(); charcount = 0;}
					else if (data == 'F') charcount++;
					else charcount = 0;
					break;
				case 5:
					if (data == 'F') lysav();
				default:
					charcount = 0;
			}
    }
}

/*******************************************************/
/* Servostuff                                          */
/*******************************************************/

void setup_servo()
{
	DDRA |= (1<<DDA7);
	TCCR0A |= (1<<COM0B1) | (1<<COM0B0) | (1<<WGM01) | (1<<WGM00);	// Set output på OC0B og fast-PWM
	TCCR0B |= (1<<CS00) | (1<<CS02) | (1<<WGM02);					// også sett prescaler
	TCNT0 = 0x00;
	OCR0A = 0x80;
	OCR0B = 0x74;
}

void lyspaa()
{
	 OCR0B = 0x77;
	 delay(500);
 	 OCR0B = 0x74;
}

void lysav()
{
	 OCR0B = 0x71;	
	 delay(500);
	 OCR0B = 0x74;
}

/***************************************************************/
/* Komunikasjon. SPI                                           */
/***************************************************************/
//
// Er ikke pinner nok til treveis og I2C suger våt eselballe.
//

void setup_com()
{
	cli();
	DDRB &= ~(1<<PINB0) & ~(1<<PINB2);
	USIDR = 0;																	// Reset data
	USISR &= ~(1<<USICNT0) & ~(1<<USICNT1) & ~(1<<USICNT2) & ~(1<<USICNT3);		// Reset input clock
	USISR |= (1<<USIOIF);														// & interrupt
	USICR = (1<<USICS1) | (1<<USIOIE);											// Set input only and enable interrupt
	sei();
}

ISR(USI_OVF_vect)
{
	cominn = USIDR;
	setup_com();
	delay(10);
	setup_com();
}

/************************************************************************/
/* Noncritical Timetjafs                                                */
/************************************************************************/
void delay(int mill)
{
	for(int i=0; i<mill; i++)
	_delay_ms(1);
}
