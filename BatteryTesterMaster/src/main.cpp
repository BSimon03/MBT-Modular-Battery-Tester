/*************************/
/*  main.cpp             */
/*  Project MBT          */
/*                       */
/*  ATmega32u4 Master    */
/*  V1.0                 */
/*                       */
/*  Author: Simon Ball   */
/*************************/

#include "init_ATmega32u4.h"
#include "MBT_settings.h"

//Data received
uint8_t storedDATA = 0;

//Data to be sent
uint8_t information_string; //Bits[7:6] for indication and information, Bits[5:0] resistance of the battery cell
uint8_t value_string;		//Bits[7:0] capacitance of the battery cell

//Interrupt
uint8_t timer_counter = 0;
uint32_t millis = 0; //32bit: Overflow after ~48 days

uint32_t t = 0; //millis compare value

//Measurements
uint16_t charge_capacity = 0;
uint16_t discharge_capacity = 0;
uint8_t discharge_resistance = 0;

uint8_t battery_temperature = 0;
float battery_voltage = 0;

//Outcome of Measurements
uint8_t capacity = 0;
uint8_t resistance = 0;

int main(void)
{
	init_atmega32u4(); //Initiating the MCU, Registers configurated

	sei(); //global interrupt enable

	while (1)
	{
		
	}
}