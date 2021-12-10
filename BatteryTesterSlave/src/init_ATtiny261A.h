/*************************/
/*  Header File          */
/*  Project MBT          */
/*                       */
/*  Slave Initiation     */
/*************************/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdint.h>

//Bit defines
//SPI
#define SDI			PINB0		//Serial Data Input Pin
#define SDO			PINB1		//Serial Data Output Pin
#define USCK		PINB2		//Clock Pin -> Input

#define CS			PINB6		//Chip Select/Slave Select Pin... to be pulled down by the master when data shifting is complete

//Power
#define CHARGE		PINA0		//PWM Output Pin for the charging circuit
#define DISCHARGE	PINA5		//PWM Output Pin for the discharging circuit

//ADC
#define TEMP		PINA6		//ADC5 MUX5:0 000101	//Connected to the NTC
#define BATT		PINA4		//ADC6 MUX3:0 000011	//Connected directly to the battery

//Status
#define STAT_RED	PINA1		//Red status LED
#define STAT_GREEN	PINA2		//Green status LED

#define ADC_INTERRUPT ADCSRA & 0b00010000 



//Clock Prescaler
#define CLK_PRESCALER_VALUE 1  //Must be 1, 2, 4, 8, 16, 32, 64, 128 or 256

/* MCU CLOCK PRESCALER */

//Prescaler value converted to bit settings.

#if CLK_PRESCALER_VALUE == 1
#define CLK_PS_SETTING (1<<CLKPCE)

#elif CLK_PRESCALER_VALUE == 2
#define CLK_PS_SETTING (1<<CLKPCE)|(1<<CLKPS0)

#elif CLK_PRESCALER_VALUE == 4
#define CLK_PS_SETTING (1<<CLKPCE)|(1<<CLKPS1)

#elif CLK_PRESCALER_VALUE == 8
#define CLK_PS_SETTING (1<<CLKPCE)|(1<<CLKPS1)|(1<<CLKPS0)

#elif CLK_PRESCALER_VALUE == 16
#define CLK_PS_SETTING (1<<CLKPCE)|(1<<CLKPS2)

#elif CLK_PRESCALER_VALUE == 32
#define CLK_PS_SETTING (1<<CLKPCE)|(1<<CLKPS2)|(1<<CLKPS0)

#elif CLK_PRESCALER_VALUE == 64
#define CLK_PS_SETTING (1<<CLKPCE)|(1<<CLKPS2)|(1<<CLKPS1)

#elif CLK_PRESCALER_VALUE == 128
#define CLK_PS_SETTING (1<<CLKPCE)|(1<<CLKPS2)|(1<<CLKPS1)|(1<<CLKPS0)

#elif CLK_PRESCALER_VALUE == 256
#define CLK_PS_SETTING (1<<CLKPCE)|(1<<CLKPS3)

#else
#error Invalid prescaler setting.

#endif

/*
void mcu_set_clock()		//set clock to 10MHz? supply voltage?
{
	//internal clock must not be set, its selected by default
	CLKPR|=(1<<CLKPCE); //Clock prescaler enabled
	CLKPR&=~(1<<CLKPS3)|(1<<CLKPS2)|(1<<CLKPS1)|(1<<CLKPS0); //no prescaler (all 0), but still avoids unintentional clock changes
}
*/

void mcu_set_clock()					//Function to easily switch between Clock prescalers using the previous define
{
	CLKPR = CLK_PRESCALER_VALUE;
}

void PCINT_setup()						//Pin Change Interrupt, both edges
{
	PCMSK1 |= (1<<PCINT14);				//Interrupt Pin
	GIMSK |= PCIE1;                     //General Interrupt Mask Register / PCIE bit activates external interrupts
}

void PWM_setup()	//Charge/Discharge
{
	//Timer 1: 10kHz Software-PWM
	//2 Channels can be controlled by changing OCR1x and used via the OVF Interrupt

	TCCR1A&=~(1<<COM1A1)|(1<<COM1A0)|(1<<COM1B1)|(1<<COM1B0);
	TCCR1A|=(1<<PWM1A)|(PWM1B);
	
	TCCR1B|=(1<<CS13)|(1<<CS11)|(1<<CS10);
	TCCR1B&=~(1<<CS12);
	
	TCCR1C&=~(1<<COM1D1)|(1<<COM1D0);
	
	TCCR1D&=~(1<<WGM11);
	TCCR1D|=(1<<WGM10);
	
	TIMSK|=(1<<OCIE1A)|(1<<OCIE1B);
	
	OCR1C=20;
	
	OCR1A=0;
	OCR1B=0;
}

void SPI_setup()
{
	//port settings for slave
	DDRB|=(1<<SDO);     //Setting direction of PB1
	DDRB&=~(1<<SDI)|(1<<CS)|(1<<USCK);
	PORTB|=(1<<SDI)|(1<<CS);  // Pull-up
	
	//Choosing SPI aka three wire mode p.133
	USICR&=~(1<<USIWM1);
	USICR|=(1<<USIWM0);
	
	//Select clock source
	USICR|=(1<<USICS1);					//External clock source
	USICR&=~(1<<USICS0)|(1<<USICLK);	//positive edge
}

void ADC_setup()
{
	//ADC5 : Temperature Sensor NTC 
	ADCSRA|=(1<<ADEN);					//ADC enabled, 
	ADCSRA|=(1<<ADPS2)|(1<<ADIE);		//Clock Prescaler of 16, ADC Interrupt enabled
	
	ADMUX|=(1<<REFS1);									//Internal Reference Voltage 2.56V
	ADCSRB|=(1<<REFS2);									//Internal Reference Voltage 2.56V

	//Result is right adjusted
	
	//ADATE is not enabled, which means we drive the ADC in Single Conversion Mode.
	//By setting ADSC (ADC Start Conversion) to a logic 1, the conversion is getting started.
	//Once the conversion is done, ADSC is cleared and the ADIF flag will be set.
	//When its completed the channel can safely be changed. The next conversion takes 25 clock cycles.
}

void init_attiny261a()					//Combining all setup functions
{
	DDRA|=(1<<STAT_GREEN)|(1<<STAT_RED);
	mcu_set_clock();
	PCINT_setup();
	PWM_setup();
	ADC_setup();
	SPI_setup();
}