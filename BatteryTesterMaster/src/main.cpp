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

#define SLAVECOUNT 10

uint8_t slave = 0;

//Data received
uint8_t storedDATA[SLAVECOUNT] = {0};

//Data to be sent
uint8_t information_string; //Bits[7:6] for indication and information, Bits[5:0] resistance of the battery cell
uint8_t value_string;		//Bits[7:0] capacitance of the battery cell

/*//Interrupt
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
uint8_t resistance = 0;*/

int main(void)
{
	init_atmega32u4(); //Initiating the MCU, Registers configurated

	sei(); //global interrupt enable

	while (1)
	{
		SPDR = request_info;
		PORTB &= ~(1 << SS);
		while (!(SPSR & (1 << SPIF)))
			;

		for (slave = 0; slave < SLAVECOUNT; slave++)
		{
			storedDATA[slave] = SPDR;
			switch (storedDATA[slave])
			{
			case error_high_temp: //high temperature (>=60°C)		temperature /2

				break;

			case error_low_volt: //low voltage (<=2,5V)			voltage x 10

				break;

			case error_not_suit: //not suitable

				break;

			case error_res_high: //res too high (res>=50mOhm)

				break;

			case error_cap_dif: //self discharge... charge_cap|discharge_cap+-10%

				break;

			case error_timeout: //timeout (time>=10h)

				break;

			case idle: //idle
				//On-Board LED high
				break;

			case processing: //measurement in progress		current progress in percent

				break;

			case done: //done with the measurement		resistance in mOhms

				break;

			default:
				//On-Board LED low
				break;
			}
		}
	}
}