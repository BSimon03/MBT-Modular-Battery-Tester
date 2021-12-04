/*************************/
/*  Header File          */
/*  Project MBT          */
/*                       */
/*  Important Strings    */
/*************************/

//Important Predefines
#define MIN_PRE_CHARGE	2
#define MAX_VOLTAGE		4.2
#define END_OF_CHARGE	3.5
#define MIN_CAPACITY	1600
#define MAX_RESISTANCE	50

//Tolerance
#define MAX_CAP_DIFF	10

//Strings sent by the master -> Requests
const uint8_t empty_string= 0x00;
const uint8_t request_info= 0x2A; //0b00101010
const uint8_t request_secs= 0xE4; //0b11100100

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

//Answer string templates
const char error_high_temp 		= 0b00000000;
const char error_low_volt 		= 0b00100000;

const char error_not_suit		= 0b10000000;
const char error_cap_low 		= 0b10000000;
const char error_res_high 		= 0b10010000;
const char error_cap_dif 		= 0b10100000;
const char error_timeout 		= 0b10110000;

const char idle 				= 0b01100000;
const char processing 			= 0b10000000;
const char done 				= 0b11000000;

/*
Bits:
0:: - no test
 00:: - high temperature	(>=60°C)
	   send temperature /2
	   
 01:: - low voltage			(<=2,5V)
	   send voltage x 10
	   
 10:: - not suitable		(cap<=800; res>=50mOhm; charge_cap|discharge_cap+-10%)
	send reason
   00 - cap too low
   01 - res too high
   10 - 10% difference
   11 - timeout
   
 11:: - idle
 
1:: - got to do
 10:: - measurement in progress
	  current progress in percent
	  
 11:: - done with the measurement
	  resistance in mOhms
*/