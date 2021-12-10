/*************************/
/*  Header File          */
/*  Project MBT          */
/*                       */
/*  Master Initiation    */
/*************************/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdint.h>

//Bit defines
//SPI
#define MISO		PINB3		//Serial Data Input Pin
#define MOSI		PINB2		//Serial Data Output Pin
#define SCL		    PINB1		//Clock Pin -> Output

#define SS			PINB0		//Chip Select/Slave Select Pin... to be pulled down when data shifting is complete

//Status
#define STAT_RED	PINA1		//Red status LED
#define STAT_GREEN	PINA2		//Green status LED

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

void mcu_set_clock()					    //Function to easily switch between Clock prescalers using the previous define
{
	CLKPR = CLK_PRESCALER_VALUE;
}

void SPI_setup()
{
	//port settings for master
	DDRB|=(1<<MOSI)|(1<<SS)|(1<<SCL);       //Setting direction of PB1
	DDRB&=~(1<<MISO);
	PORTB|=(1<<MISO)|(1<<SS);
	
	
    SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0);    //Enable SPI, Master, set clock rate fck/16 
}

void init_atmega32u4()					    //Combining all setup functions
{
	mcu_set_clock();
	SPI_setup();
}