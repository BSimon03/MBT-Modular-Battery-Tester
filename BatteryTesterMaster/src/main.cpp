/*
 * BatteryTesterMaster.cpp
 *
 * Created: 28/10/2021 12:39:57
 * Author : Simon
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>

//Bit defines
//SPI
#define SDI  PINB0
#define SDO  PINB1
#define USCK PINB2
#define CS	 PINB6

//Constants
const char not_selected=0x00;
const char request_info=0x2A;
const char send_second_string=0xE4;

const char slave_count=8;

//data
char send[8]={0};
char received[8];

int capacity[8]={0}, resistance[8]={0};		//discharge amphours and resistance
	
//Interrupt
volatile unsigned long t=0;
volatile unsigned long millis=0;	//32bit: Overflow after ~48 days

void mcu_set_clock()
{
	//internal clock must not be set, its selected by default
	CLKPR|=(1<<CLKPCE); //Clock prescaler enabled
	CLKPR&=~(1<<CLKPS3)|(1<<CLKPS2)|(1<<CLKPS1)|(1<<CLKPS0); //no prescaler (all 0), but still avoids unintentional clock changes
}

void SPI_setup()
{
	//port settings for master
	DDRB|=(1<<SDO)|(1<<CS)|(1<<USCK);		//CS as output, CLK as output
	PORTB|=(1<<CS);							//CS pulled to high
	
	DDRB&=~(1<<SDI);
	PORTB|=(1<<SDI);  // Pull-up
	
	//Choosing SPI aka three wire mode p.133
	USICR&=~(1<<USIWM1);
	USICR|=(1<<USIWM0);
	
	//set as master, generates clock p.135
	USICR|=(1<<USITC);			
	
	//Select clock source: Software clock strobe USICLK p.134
	USICR|=(1<<USICLK);			
	USICR&=~(1<<USICS0)|(1<<USICS1);
}

char check_slave(char s_address)
{
	/*empty byte*/
	int i=0;
	for(i=0; i>slave_count-s_address; i++)
	USIDR=not_selected;
	
	/*request*/
	USIDR=request_info;
	i++;
	
	/*empty byte*/
	for(i; i>slave_count; i++)
	USIDR=not_selected;
	
	/*Pull Down */
	PORTB&=~(1<<CS);
}

float receive_float(char s_address, char request)
{
	
}

void send_data()
{
	
}

void timer_setup()
{
	//Timer 0: 1ms
	TCCR0A|=(1<<CTC0);	//CTC0
	TCCR0A&=~(1<<ICEN0)|(1<<TCW0);
	TCCR0B|=(1<<CS02)|(1<<CS00);
	TCCR0B&=~(1<<CS01);
	
	TIMSK|=(1<<OCIE0A);
	OCR0A=20;
}

void delay_ms(unsigned int ms)
{
	unsigned long t = millis;
	while((t+ms)>=millis){}
}

int main(void)
{
	//DO out, DI in
	//sck in, cs in
	
	DDRA
	DDRB
	//
	
	set_mcu_clock();
	SPI_setup();
	timer_setup();
	
	sei();		//global interrupt enable

	while (1)
	{
		
	}
}

ISR(TIMER0_COMPA_vect)
{
	millis++;	//count +1 every ms
}