/*************************/
/*  main.cpp             */
/*  Project MBT          */
/*                       */
/*  Attiny261A Slave     */
/*  V1.0                 */
/*                       */
/*  Author: Simon Ball   */
/*************************/

#include "init_ATtiny261A.h"
#include "MBT_SPI.h"

#define ADC_SAMPLES 6

//Data received
volatile uint8_t storedDATA = 0;

//Data to be sent
volatile uint8_t information_string;		//Bits[7:6] for indication and information, Bits[5:0] resistance of the battery cell
volatile uint8_t value_string;			//Bits[7:0] capacitance of the battery cell

//Interrupt
volatile uint8_t timer_counter = 0;
volatile uint8_t adc_counter = 6;
volatile uint32_t millis = 0;		//32bit: Overflow after ~48 days

volatile uint32_t t = 0;			//millis compare value

//Status
volatile uint8_t status = 0;
	// 0  : Idle, waiting
	// 1  : balance the cell to a start level: 3V
	// 2  : charging until 4V with 1A
	// 3  : charging until 4,2V with 500mA
	// 4  : resistance measurement with 1 - 2A
	// 5  : discharging until 3V with 1A
	// 6  : charging until 3.7V
	// 7  : Charging
	// 8  : Discharging
	// 9  : Error, temperature too high
	// 10 : Error, low voltage
	// 11 : Error, not suitable (res too high, cap too low)
	// 12 : Error, timeout 10h
	// 13 : Done with the measurement
	
volatile uint8_t ADCstat = 0;
	// 0  : Set up for Battery Temperature Measurement
	// 1  : Set up for Battery Voltage Measurement

//Measurements
volatile uint16_t charge_capacity = 0;
volatile uint16_t discharge_capacity = 0;
volatile uint16_t discharge_resistance = 0;

volatile uint16_t adc_value_raw = 0;
volatile float adc_values[ADC_SAMPLES] = {0};
volatile uint16_t battery_temperature = 0;
volatile float battery_voltage = 0;

volatile float bubble = 0;			//Bubblesort algorithm

//Outcome of Measurements
volatile uint8_t capacity = 0;
volatile uint8_t resistance = 0;

int main(void)
{	
	
    init_attiny261a();
	
	sei();		//global interrupt enable

    while (1) 
    {
		//Communication
		information_string=0;
		switch(status)
		{
			case 0:		//Idle, waiting
				information_string |= idle;

			break;
			case 1:		//balance the cell to a start level: 3V
				information_string |= processing;

			break;
			case 2:		//charging until 4V with 1A
				information_string |= processing;

			break;
			case 3:		//charging until 4,2V with 500mA
				information_string |= processing;

			break;
			case 4:		//resistance measurement with 1 - 2A
				information_string |= processing;

			break;
			case 5:		//discharging until 3V with 1A
				information_string |= processing;

			break;
			case 6:		//charging until 3.7V
				information_string |= processing;

			break;
			case 7:		//charging
				information_string |= processing;

			break;
			case 8:		//Discharging
				information_string |= processing;

			break;
			case 9:		//Error, temperature too high
				information_string |= error_high_temp;

			break;
			case 10:	//Error, low voltage
				information_string |= error_low_volt;

			break;
			case 11:	//Error, not suitable (res too high, cap too low)
				information_string |= error_not_suit;

			break;
			case 12:	//Error, timeout 10h
				information_string |= error_timeout;

			break;
			case 13:	//Done with the measurement
				information_string |= done;
				value_string = 0xFF;

			break;
		}
	}
}


//Pin change interrupt set up for the chip-select pin

ISR(PCINT_vect)
{
	if((PINB & (1<<CS))== 0)
	{
		// If edge is falling, the command and index variables shall be initialized
		// and the 4-bit overflow counter of the USI communication shall be activated:
		USICR |= (1<<USIOIE);
		USISR = 1<<USIOIF;      // Clear Overflow bit
	}
	else
	{
		// If edge is rising, turn the 4-bit overflow interrupt off:
		USICR &= ~(1<<USIOIE);
	}
}


 // USI interrupt routine. Always executed when 4-bit overflows (after 16 clock edges = 8 clock cycles):

 ISR(USI_OVF_vect)
 {
		storedDATA = USIDR;      // Read in from USIDR register
		switch(storedDATA){

			 // Switch-Case to respond according to request from Master:

			 case empty_string:             // If storedDATA is an empty string only the flag is being cleared
			 USISR = 1<<USIOIF;				// Clear Overflow bit
			 break;

			 case request_info:				// If the master requested information, the information string immediately gets sent back.
			 USIDR = information_string;
			 USISR = 1<<USIOIF;
			 break;

			 case request_secs:				// If the master requested the second string, values are getting sent back.
			 USIDR = value_string;
			 USISR = 1<<USIOIF;
			 break;
		 }
 }
 
 
 
 ISR(TIMER1_COMPA_vect)		//Charging OFF on compare match
 {
	 PORTA|=(1<<CHARGE);
 }
 ISR(TIMER1_COMPB_vect)		//Discharging OFF on compare match
 {
	 PORTA&=~(1<<DISCHARGE);
 }
 ISR(TIMER1_OVF_vect)		//Charge or Discharge ON
 {
	 if(status==1)
	 PORTA&=~(1<<CHARGE);
	 if(status==2)
	 PORTA|=(1<<DISCHARGE);
	timer_counter++;
	if(timer_counter==10)
	{
		timer_counter=0;
		millis++;
	}
 }
 
 ISR(ADC_vect)					//ADC Conversion completed, interrupt flag cleared
 {			//Result is right adjusted
	 if (adc_counter>=ADC_SAMPLES)
	  {
		  //Bubblesort and averaging algorithm
		  for(adc_counter=0; adc_counter<=(ADC_SAMPLES-2); adc_counter++)
		  {
			  if(adc_values[adc_counter]<adc_values[adc_counter-1])
				{
					bubble=adc_values[adc_counter-1];
					adc_values[adc_counter-1]=adc_values[adc_counter];
					adc_values[adc_counter]=bubble;
				}
		  }
		  
		  for(adc_counter=0; adc_counter<=(ADC_SAMPLES-2); adc_counter++)
		  {
			  if(adc_values[adc_counter]<adc_values[adc_counter-1])
			  {
				  bubble=adc_values[adc_counter+1];
				  adc_values[adc_counter+1]=adc_values[adc_counter];
				  adc_values[adc_counter]=bubble;
			  }
		  }
		  
		  if(!ADCstat)					//Measured Temperature
		  {	  
			  battery_temperature=0;		//Resetting battery temperature
			  for(adc_counter=1; adc_counter<=(ADC_SAMPLES-2); adc_counter++)	//Adding all measured values to battery_voltage, except the outer ones.
				battery_temperature+=adc_values[adc_counter];
			  battery_temperature/=(ADC_SAMPLES-2);
			  adc_counter=0;
			  ADCstat=1;
			  ADMUX&=~(1<<MUX0)|(1<<MUX1)|(1<<MUX2);		//Clearing all important bits of the ADMUX register
			  ADMUX|=(1<<MUX0)|(1<<MUX1);				//Attaching Channel 6 to the ADC
			  
		  }
		  else
		  {								//Measured Battery Voltage
			  battery_voltage=0;		//Resetting battery voltage
			  for(adc_counter=1; adc_counter<=(ADC_SAMPLES-2); adc_counter++)	//Adding all measured values to battery_voltage, except the outer ones.
				battery_voltage+=adc_values[adc_counter];
			  battery_voltage/=(ADC_SAMPLES-2);
			  adc_counter=0;
			  ADCstat=0;
			  ADMUX&=~(1<<MUX0)|(1<<MUX1)|(1<<MUX2);		//Clearing all important bits of the ADMUX register
			  ADMUX|=(1<<MUX0)|(1<<MUX2);				//Attaching Channel 5 to the ADC
		  }
	 }
	adc_value_raw|=ADCL;
	adc_value_raw|=((ADCH&0b00000011)<<8);
	adc_values[adc_counter]=(float)adc_value_raw/400;		//divided by 1023 aka 10-bit, multiplied by 2,56 aka internal reference voltage
	adc_value_raw=0;
	adc_counter++;
 }