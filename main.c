/*
 * CarPlayerTiny.cpp
 *
 * Created: 25.07.2020 1:18:09
 */ 

// 9.6 MHz (default) built in resonator
#define F_CPU 960000UL

#include <string.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define G_BIT _BV(PB2)
#define B_BIT _BV(PB1)
#define R_BIT _BV(PB0)
#define BTN_BIT _BV(PB4)

#define IN_PB4 (PINB & BTN_BIT)

#define LONG_BUTTON_PRESS_INTERVAL 10

#define FADE_STEPS 32
#define FADE_WAIT_STEPS 18
#define FADE_GROUPS 3
#define BLACK_STEPS 12

#define TRUE 1
#define FALSE 0

#define EFFECT_TIMEOUT_TICKS 512


unsigned short seed=4175;
unsigned short effectTicks=0;

uint8_t currentEffectIdx=0;

void effectRandom();
void effectFadeIn();
void effectFadeOut();
void effectQueue();

void (*effects[])()=
{
	effectRandom,
	effectFadeIn,
	effectQueue,
	effectFadeOut
};

uint8_t linrand()
{
	seed = 3 * seed;
	return (seed>>((seed>>3)&0xF));
}

#define delayTicks(ticks) { _delay_ms(50*ticks);effectTicks+=ticks;}

int main()
{
	void (*currentEffect)() = effects[0];
	
	DDRB = R_BIT | G_BIT | B_BIT; // Setting output for OUT_BIT, others for input
	PORTB |= BTN_BIT; // Set pull-up high for button

	while(1)
	{
		for(int i=0;i<8;i++)
		{
			PORTB = (PORTB & ~7) | i;
			_delay_ms(500);
		}
	}

	
	//currentEffect();
	//while(1)
	//{
		//if(effectTicks>=EFFECT_TIMEOUT_TICKS || !IN_PB4)
		//{
			//_delay_ms(200);
			//effectTicks=0;
			//currentEffectIdx++;
			//if(currentEffectIdx>=(sizeof(effects)/sizeof(effects[0])))
			//{
				//currentEffectIdx=0;
			//}
			//currentEffect=effects[currentEffectIdx];
		//}
		//currentEffect();		
	//}
}

uint8_t getUnsignedRandomStep() {
	return (linrand()&0x7)|1;
}

//void effectRandom()
//{
	//for(uint8_t i=0;i<LEDS_COUNT*3;i++)
	//{
		//led[i]=linrand();
	//}
	//setleds(led);
	//delayTicks(6);
//}
//
//void doTransition(uint8_t fadeCounter)
//{
	//static uint8_t prevSparkPos=255;
	//static uint8_t shouldDoSpark=0;
//
	//if(fadeCounter>=BLACK_STEPS) {
		//if(fadeCounter<BLACK_STEPS+FADE_STEPS)
		//{
			//for(uint8_t i=0;i<FADE_GROUPS*3;i++)
			//{
				//led[i]+=fadeSteps[i];
				//led[i+9]=led[i];
				//led[i+18]=led[i];			
			//}
			//setleds(led);
		//}
		//else
		//{
			//if(shouldDoSpark)
			//{
				//uint8_t sparkPos = fadeCounter-(BLACK_STEPS+FADE_STEPS);
				//if(sparkPos>=LEDS_COUNT) {
					//sparkPos = (LEDS_COUNT*2-1)-sparkPos;
				//}
				//if(prevSparkPos<LEDS_COUNT) {
					//memcpy(led+prevSparkPos*3,led+(prevSparkPos + (prevSparkPos<FADE_GROUPS ? FADE_GROUPS: -FADE_GROUPS))*3,3);
				//}
				//prevSparkPos = sparkPos;
				//if(sparkPos<LEDS_COUNT)
				//{
					//memset(led+sparkPos*3,255,3);
				//}
				//setleds(led);
			//}
		//}
	//}
	//else
	//{
		//shouldDoSpark=linrand()>128;
	//}
//
	//delayTicks(1);
//}
//
//void effectFadeIn()
//{
	//uint8_t fadeCounter=effectTicks&0x3f;
	//if(!fadeCounter)
	//{
		//for(uint8_t i=0;i<FADE_GROUPS*3;i++)
		//{
			//fadeSteps[i]=getUnsignedRandomStep();
		//}
		//memset(led,0,LEDS_COUNT*3);
//
		//setleds(led);
	//}
	//
	//doTransition(fadeCounter);
//}
//
//void effectFadeOut()
//{
	//uint8_t fadeCounter=effectTicks&0x3f;
	//if(!fadeCounter)
	//{
		//for(uint8_t i=0;i<FADE_GROUPS*3;i++)
		//{
			//led[i] = (linrand()&0xE0)|0x20;
//
			//fadeSteps[i]=-(led[i]>>5);
			//led[i+9]=led[i];
			//led[i+18]=led[i];
		//}
		//setleds(led);		
	//}
	//
	//doTransition(fadeCounter);
//}
//
//void effectQueue()
//{
	//if(!effectTicks)
	//{
		//memset(led,0,LEDS_COUNT*3);
	//}
//
	//for(uint8_t i=LEDS_COUNT*3-1;i>=3;i--) 
	//{
		//led[i]=led[i-3];
	//}
//
	//for(uint8_t i=0;i<3;i++)
	//{
		//led[i]=((led[3+i]<<1)+getUnsignedRandomStep());
		//if(led[i]>=effectTicks)
		//{
			//led[i]-=effectTicks&0xFF;
		//}
	//}
	//
	//setleds(led);	
	//delayTicks(3);	
//}
