/*************************/
/*  Header File          */
/*  Project MBT          */
/*                       */
/* 	Settings			 */
/*************************/

//Important Predefines
#define MIN_PRE_CHARGE	2						//Minimum voltage the battery must be precharged on
#define MAX_VOLTAGE		4.2						//Maximum rating for th specific battery type... depends
#define END_OF_CHARGE	3.5						//Voltage to which the battery will be charged when the measurements are done
#define MIN_CAPACITY	1600					//Minimum capacity the battery must have to be suitable
#define MAX_RESISTANCE	50						//Maximum inner resistance

//Tolerance
#define MAX_CAP_DIFF	10						//Its recommended not to change this value
#define TEMP_CONSTANT	3.6						//Callibration value for the NTC
#define MAX_CAP_DIFF	10						//Its recommended not to change this value
#define TEMP_CONSTANT	3.6						//NTC Constant
#define ADC_OFFSET		0.25					//ADC shottky offset

//Strings sent by the master -> Requests
const uint8_t request_info= 0xCC; 				//0b11001100		//Master requests information
const uint8_t request_secs= 0xF0; 				//0b11110000		//Master requests the second byte of information, after the finish is acknowledged

//Answer string templates
const uint8_t error_high_temp 	= 0b01100000;	//high temperature (>=60°C)		send temperature /2
const uint8_t error_low_volt 	= 0b00100000;	//low voltage (<2,5V)			send voltage x 10

const uint8_t error_not_suit	= 0b01000000;	//not suitable
const uint8_t error_res_high 	= 0b01001000;	//res too high (res>=50mOhm)
const uint8_t error_cap_dif 	= 0b01010000;	//self discharge... charge_cap|discharge_cap+-10%
const uint8_t error_timeout 	= 0b01011000;	//timeout (time>=10h)

const uint8_t idle 				= 0b10100000;	//idle
const uint8_t processing 		= 0b10000000;	//measurement in progress		current progress in percent
const uint8_t done 				= 0b11000000;	//done with the measurement		resistance in mOhms

/*
Progress:
	00% 0) Idle
	00% 1) balance the cell to a start level: 3V
	10% 2) charge until 4V with 1A
	35% 3) charge until 4,2V with 500mA
	55% 4) resistance measurement with 1 - 2A
	55% 5) discharge until 3V with 1A
	80% 6) charge until 3.7V
	100% - Done
*/