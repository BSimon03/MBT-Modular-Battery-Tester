/*************************/
/*  Header File          */
/*  Project MBT          */
/*                       */
/*  Important Strings    */
/*************************/

//Strings sent by the master -> Requests
const char empty_string=0x00;
const char request_info=0x2A; //0b00101010
const char request_secs=0xE4; //0b11100100

//Answer string prototypes
const char error_high_temp = 0b00000000;
const char error_low_volt = 0b00100000;
const char error_not_suit = 0b01000000;
const char error_timeout = 0b01100000;

const char ok_inprogress = 0b10000000;
const char ok_done = 0b11000000;



/*
Process:
	00% 0) Idle
	00% 1) balance the cell to a start level: 3V
	10% 2) charge until 4V with 1A
	35% 3) charge until 4,2V with 500mA
	55% 4) resistance measurement with 1 - 2A
	55% 5) discharge until 3V with 1A
	80% 6) charge until 3.7V
	100% - Done
*/

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
 10:: - still testing
	  current progress in percent
	  
 11:: - done with the measurement
	  resistance in mOhms
*/