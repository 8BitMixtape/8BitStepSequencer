/*************************************************************************

        Attiny 13 SoundPitix-VCO
        Basic sound generator for building modular mixed analog/digital 
        synthesizers. 

	11.09.2007 chris, version 1.5 

	13.8.2014  chris, version 1.5.1 copyright message added, 

                                        code cleaned
                                        
        http://www.hobby-roboter.de/forum/viewtopic.php?f=5&t=73

	chris: christoph(at)roboterclub-freiburg.de

*************************************************************************/

/*************************************************************************

	Hardware

	prozessor:	ATtin85
	clock:		8Mhz internal RC oscillator

	PIN1	RESET			connect 10K pullup
	PIN2	PORTB3/ADC3		waveform selector
	PIN3	PORTB4/ADC2		frequency control
	PIN4	GND
	PIN5	PORTB0/OC10		8Bit PWM sound output
	PIN6	PORTB1			GND=LFO selected; VCC=VCO ( default )
	PIN7	PORTB2/ADC1		volume control
	PIN8	VCC

*************************************************************************/

/*************************************************************************
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*
*************************************************************************/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>

#define INIT_ADC  ADCSRA = (1<<ADEN) | (1<<ADPS2) | (1<<ADPS1)

uint16_t adc_read(char input)
{
    // Hole ADC value
    uint16_t wert,temp=0;
    uint8_t n;

    // VCC ref and ADCx
    ADMUX = (0<<REFS0) | (0x07&input);

    // Clear ADIF and start single conversion
    ADCSRA |= (1 << ADIF) | (1<<ADSC);
    
    // wait for conversion done, ADIF flag active
    while(!(ADCSRA & (1 << ADIF)));

	n= ADCH; // dummy adc read
	for(n=0;n<4;n++)
	{
           // Clear ADIF and start single conversion
           ADCSRA |= (1 << ADIF) | (1<<ADSC);
           
           // wait for conversion done, ADIF flag active
           while(!(ADCSRA & (1 << ADIF)));

	   // read out ADCL register
           wert = ADCL;

           // read out ADCH register
           wert += (ADCH << 8);

           temp+=wert;
	}
    return (temp>>2);
}

// sampling frequency

#define FS (37500/2) // @FCPU=9.6Mhz

uint8_t envelope=255;
uint16_t freq_const=440*65536/FS; 

#define SINUS 0
#define TRIANGLE 1
#define SAWTOOTH 2
#define RECTANGLE 3
#define NOISE 4

uint8_t wavetype=SAWTOOTH;
uint8_t RectanglePwmValue=40;

int8_t sinwerte[17]={0, 12, 25, 37, 49, 60, 71, 81, 90, 98, 106, 112, 117, 122, 125, 126, 127 };

SIGNAL (TIM0_OVF_vect)
{
	static uint8_t x;
	static uint16_t phase1,oldphase;
	static int8_t k,flag=0;
	static int16_t temp,temp2;
	static uint16_t noise=0xAA;

	if(flag)
	{
		//Phase VCO
		oldphase=phase1;
		phase1=phase1+freq_const;    	
		if(wavetype==SINUS)
		{
			x=phase1>>10;
			if(x>48) 
			{ 
				x=64-x;
				k=-sinwerte[x];
			}
			else if(x>32) 
			{ 
				x=x-32;
				k=-sinwerte[x];
			}
			else if(x>16) 
			{ 
				x=32-x;
				k=sinwerte[x];
			}
			else 
			{ 
				k=sinwerte[x];
			}	
			temp=k;
		}

		if(wavetype==TRIANGLE) 
		{
			x=phase1>>8;
			if(x&0x80) temp=(x^0xFF);
			else temp=x;
			temp=(temp<<1)-128;
		}

		if(wavetype==RECTANGLE) 
		{
			x=phase1>>8;
			if(x>RectanglePwmValue) 
			{
				temp=127;
			}
			else temp=-127;
		}

		if(wavetype==SAWTOOTH)
		{
			temp=(phase1>>8)-128;
		}

		if(wavetype==NOISE)
		{				
			temp = noise;
			noise=noise << 1;
			temp ^= noise;

			if ( ( temp & 0x4000 ) == 0x4000 )noise |= 1;
			if((oldphase^phase1)&0x4000) 
			{
				temp2=(noise>>6)&0xFF;
			}
			temp=temp2;
		}
	}	
 	else
	{
		temp=(temp*envelope);
		OCR0A=(temp>>8)+128;
	}
	flag^=0x01;
}

#define VOLUME_CONTROL 1
#define FREQUNCY_CONTROL 2
#define WAVEFORM_CONTROL 3

int main(void)
{
	uint8_t wert,wave;
	uint16_t adcwert;

	INIT_ADC;

	//************** init Timer ************************************
	///FAST PWM mode and set OC0A ( PB0 ) port on compare match	

	TCCR0A=(1<<COM0A0) | (1<<COM0A1) | (1<< WGM00 ) | (1<< WGM01 ); 
	TCCR0B=(1<<CS00) ; // no prescaling, 
    	OCR0A=255; // Counter end

	// interrupt mask register: enable timer1 overflow 
	TIMSK |= (1 << TOIE0);
	sei();

	// Ports
	DDRB=(1<<PINB0); // Pin as output ( Pin OC0A )	
	PORTB=(1<<PORTB1) | (1<<PORTB2)| (1<<PORTB3); // Pullup

	while(1)

	{
		adcwert= adc_read(FREQUNCY_CONTROL);
		freq_const= adcwert*5*65536/FS/2;
		if(PINB&(1<<PINB1)) TCCR0B=(1<<CS00); // VCO
		else 
		{
			//freq_const=adcwert*65536/FS/4; // LFO ( 1/10 Frequency )
			TCCR0B=(1<<CS01) ; // 1/8 prescaling, 
			freq_const= adcwert;
		}
		envelope= adc_read(VOLUME_CONTROL)>>2;	

		//envelope=100;
		wert= adc_read(WAVEFORM_CONTROL)>>2;

		//wert=0;
		if(wert<=60) {wave=NOISE;}	
		if(wert>60) {wave=RECTANGLE;RectanglePwmValue=0x05;}
		if(wert>80) {wave=RECTANGLE;RectanglePwmValue=0x10;}			
		if(wert>100) {wave=RECTANGLE;RectanglePwmValue=0x20;}			
		if(wert>120) {wave=RECTANGLE;RectanglePwmValue=0x40;}	
		if(wert>140) {wave=RECTANGLE;RectanglePwmValue=0x80;}	
		if(wert>160) wave=SAWTOOTH;	
		if(wert>180) wave=TRIANGLE;
		if(wert>200) wave=SINUS;

		wavetype=wave;
	}
}


