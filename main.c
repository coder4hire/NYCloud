/*
* CarPlayerTiny.cpp
*
* Created: 25.07.2020 1:18:09
*/

// 9.6 MHz (default) built in resonator
//#define F_CPU 9600000UL
#define F_CPU 1200000UL

#include <string.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define G_BIT _BV(PB2)
#define B_BIT _BV(PB1)
#define R_BIT _BV(PB0)
#define BTN_BIT _BV(PB4)
#define SENSOR_BIT _BV(PB3)

#define IN_BTN (PINB & BTN_BIT)
#define IN_SENSOR (PINB & SENSOR_BIT)

#define LONG_BUTTON_PRESS_INTERVAL 10

#define TRUE 1
#define FALSE 0

#define SLEEP_TIMEOUT_TICKS 6000

#define EFFECT_RND_LENGTH 300
#define TRANSITION_SPEED 2
#define TRANSITION_STEPS 24
#define BLINK_TRANSITION_SPEED 12


union Color
{
	uint32_t Value;
	struct
	{
		uint8_t B;
		uint8_t G;
		uint8_t R;
	};
} color = {0};

union Color targetColor={0};

unsigned short seed=4175;
unsigned short effectTicks=EFFECT_RND_LENGTH;
unsigned short sleepTicks=0;

uint8_t currentEffectIdx=0;
uint8_t shiftFader=1;

void effectRandom();
void effectWhite();
void effectBlinkRandom();

void (*effects[])()=
{
	effectRandom,
	effectWhite,
	effectBlinkRandom
};

uint8_t linrand()
{
	seed = (seed<<1) + seed;
	return (seed>>((seed>>3)&0xF));
}

#define delayTicks(ticks) { _delay_ms(50*ticks);effectTicks+=ticks;}


//ISR(TIM0_OVF_vect){
ISR(TIM0_COMPA_vect)
{
	cli();
	static union Color bufColor;
	static uint8_t counter = 0;
	if (++counter==128)
	{
		bufColor.R= color.R>>shiftFader;
		bufColor.G= color.G>>shiftFader;
		bufColor.B= color.B>>shiftFader;
			
		PORTB |= (bufColor.R!=0) |
		((bufColor.G!=0)<<2) |
		((bufColor.B!=0)<<1);
		counter=0;
	}
		
	if (counter == bufColor.R) PORTB &= ~R_BIT;
	if (counter == bufColor.G) PORTB &= ~G_BIT;
	if (counter == bufColor.B) PORTB &= ~B_BIT;
	TCNT0 = 0;
	sei();
}

int main()
{
	uint8_t btnPressed = FALSE;
		
	DDRB = R_BIT | G_BIT | B_BIT; // Setting output for OUT_BIT, others for input
	PORTB |= BTN_BIT; // Set pull-up high for button

	TCCR0B = _BV(CS00);
	TIMSK0 = _BV(OCIE0A);
	OCR0A = 14;

	sei();

	while (1)
	{
		if(!IN_BTN)
		{
			if(!btnPressed)
			{
				currentEffectIdx=(currentEffectIdx+1) % (sizeof(effects)/sizeof(effects[0]));
				sleepTicks=0;
				shiftFader=1;
			}
			btnPressed=TRUE;
		}
		else
		{
			btnPressed=FALSE;
		}

		if(IN_SENSOR)
		{
			if(shiftFader>1)
			{
				shiftFader--;
			}
			else
			{
				shiftFader=1;
			}
			sleepTicks=0;
		}
		else if(!IN_SENSOR)
		{
			if(sleepTicks<SLEEP_TIMEOUT_TICKS)
			{
				sleepTicks++;
				if(sleepTicks > SLEEP_TIMEOUT_TICKS/2 && sleepTicks < SLEEP_TIMEOUT_TICKS/2+8)
				{
					shiftFader = (sleepTicks>>1) - SLEEP_TIMEOUT_TICKS/4+1;
				}
				else if (sleepTicks > SLEEP_TIMEOUT_TICKS-8)
				{
					shiftFader = (sleepTicks>>1) + 8 - SLEEP_TIMEOUT_TICKS/2;
				}

			}
		}

		if(shiftFader<8)
		{
			effects[currentEffectIdx]();
			effectTicks++;			
		}
		_delay_us(400);
	}

}

void effectRandom()
{
	static union Color target;
	static int8_t stepR,stepG,stepB;
	if(effectTicks==EFFECT_RND_LENGTH)
	{
		target.R = linrand();
		target.G = linrand();
		target.B = linrand();
		if(target.R+target.G+target.B<64*3)
		{
			target.Value<<=2;
		}

		stepR=((int16_t)target.R-(int16_t)color.R)/TRANSITION_STEPS;
		stepG=((int16_t)target.G-(int16_t)color.G)/TRANSITION_STEPS;
		stepB=((int16_t)target.B-(int16_t)color.B)/TRANSITION_STEPS;

		effectTicks=0;
	}
	else
	{
		if(effectTicks<=TRANSITION_SPEED*TRANSITION_STEPS)
		{
			if(!(effectTicks%TRANSITION_SPEED))
			{
				color.R+=stepR;
				color.G+=stepG;
				color.B+=stepB;
			}
		}
		else
		{
			color=target;
		}
	}
}

void effectWhite()
{
	color.Value=0xFFFFFF;
}
	
void effectBlinkRandom()
{
	if(effectTicks>BLINK_TRANSITION_SPEED)
	{
		color.R = linrand();
		color.G = linrand();
		color.B = linrand();
		effectTicks=0;
	}
}