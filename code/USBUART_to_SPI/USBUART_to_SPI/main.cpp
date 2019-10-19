/*
 * USBUART_to_SPI.cpp
 *
 * Created: 9/11/2017 12:23:04 AM
 * Author : medlem
 */ 

#define F_CPU 16000000UL  // 16 MHz
#define BAUD 9600
#define false 0
#define true ~false
#define BUFFERLENGDE 500

#include <util/setbaud.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

void ovon(void);
void ovoff(void);
void uart_init(void);
void send(uint8_t c);
uint8_t motta(void);
void com_init(void);
void sendspi(uint8_t);

volatile uint8_t serialbuffer[BUFFERLENGDE];
volatile uint16_t hode = 0;
volatile uint16_t hale = 0;

int main(void)
{
	uart_init();
    com_init();
	
    while (1) 
    {
		uint8_t data = motta();
		if (data != 0xFF)
		{
			if(data == 'L')
				{ovoff();}
			if(data == 'D')
				{ovon();}
		}
    }
}


void ovon(void)
{
	sendspi(0x00);
	sendspi('O');
	sendspi('V');
	sendspi(' ');
	sendspi('O');
	sendspi('N');
}

void ovoff(void)
{
	sendspi(0x00);
	sendspi('O');
	sendspi('V');
	sendspi(' ');
	sendspi('O');
	sendspi('F');
	sendspi('F');
}

/////////////////////////////////////////////////////////
//
// Init
//
//		Resetter tonegeneratorene en og en,
//		og setter ID p dem underveis.
//
//
#define KLOKKEPINNE PORTB5	// 12: clock
#define KLOKKEDIRBIT DDB5

#define DATAPINNE PORTB3	// 13: data
#define DATADIRBIT DDB3


void com_init(void)
{
	DDRB |= (1<<KLOKKEDIRBIT) | (1<<DATADIRBIT);
	PORTB |= (1<<KLOKKEPINNE) | (1<<DATAPINNE);

	sendspi(0x00);
	sendspi(0x00);
	sendspi(0x00);
}

void sendspi(uint8_t kommando)
{
	for (int i = 0; i < 8; i++)
	{
		if(kommando&(0x80>>i))
			PORTB |= 1<<DATAPINNE;
		else
			PORTB &= ~(1<<DATAPINNE);
		_delay_us(50);
		PORTB |= 1<<KLOKKEPINNE;
		_delay_us(50);
		PORTB &= ~(1<<KLOKKEPINNE);
		_delay_us(50);
	}
	PORTB |= 1<<DATAPINNE;
	_delay_ms(20);
}



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////
//////  Serieportstuff
//////
//////    Blablabla... https://appelsiini.net/2011/simple-usart-with-avr-libc/
//////
//////
void uart_init(void) {
	
	UBRR0H = UBRRH_VALUE;
	UBRR0L = UBRRL_VALUE;

	#if USE_2X
	UCSR0A |= 1<<U2X0;
	#else
	UCSR0A &= ~(1<<U2X0);
	#endif

	UCSR0C = (1<<UCSZ01) | (1<<UCSZ00); /* 8-bit data */
	UCSR0B = (1<<RXEN0) | (1<<TXEN0);   /* Enable RX and TX */


	UCSR0B |= (1 << RXCIE0); // Enable the USART Recieve Complete interrupt (USART_RXC)
	sei(); // Enable the Global Interrupt Enable flag so that interrupts can be processed
}

ISR(USART_RX_vect)
{
	cli();
	serialbuffer[hode++] = UDR0;
	hode = hode%BUFFERLENGDE;
	sei();
}

uint8_t motta(void) {
	uint8_t returverdi = 0xFF;
	if(hode != hale)
	{
		cli();
		returverdi = serialbuffer[hale++];
		hale = hale%BUFFERLENGDE;
		sei();
	}
	return returverdi;
}
