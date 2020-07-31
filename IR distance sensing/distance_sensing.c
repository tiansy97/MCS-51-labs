/*
Author:     Siyuan Tian (tiansy97)
Time:       2019/5
Functions:  The code reads the data from ADC which is connected to an IR sensor to 
			measure the distance. The communication between ADC and MCU is through SPI. 
			The code also displays the data on seven segment LEDs (dynamic indication). 
*/

#include "reg52.h"			 
#include "XPT2046.h"	

typedef unsigned int u16;	  
typedef unsigned char u8;

sbit LSA=P2^2;
sbit LSB=P2^3;
sbit LSC=P2^4;

u8 disp[4];
u8 code smgduan[10]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f};


void delay(u16 i)
{
	while(i--);	
}


//Data processing
void datapros()
{
	u16 temp;
	static u8 i;

	if(i==50)
	{
		i=0;
		temp = Read_AD_Data(0xE4);		
	}
	i++;

	disp[0]=smgduan[temp/1000];		//thousands
	disp[1]=smgduan[temp%1000/100];		//hundreds
	disp[2]=smgduan[temp%1000%100/10];	//tens
	disp[3]=smgduan[temp%1000%100%10];	//units	
}


//Digital display
void DigDisplay()
{
	u8 i;

	for(i=0;i<4;i++)
	{
		switch(i)	 //Select which digit is being displayed
		{
			case(0):
				LSA=0;LSB=0;LSC=0; break;	//Pin 2.2 - 2.4 decides which LED is being selected
			case(1):
				LSA=1;LSB=0;LSC=0; break;
			case(2):
				LSA=0;LSB=1;LSC=0; break;
			case(3):
				LSA=1;LSB=1;LSC=0; break;	
		}
		P0=disp[3-i]; 	//Port 1 sends the data
		delay(100);		
		
		P0=0x00; //Clear the output before selecting the next LED, which is required for dynamic LED displays
	}		
}


void main()
{	
	while(1)
	{
		datapros();	 // Process data
		DigDisplay();	// Display data	
	}		
}


void SPI_Write(uchar dat)
{
	uchar i;
	CLK = 0;
	for(i=0; i<8; i++)
	{
		DIN = dat >> 7;  	
		dat <<= 1;
		CLK = 0;			
		CLK = 1;
	}
}


uint SPI_Read(void)
{
	uint i, dat=0;

	CLK = 0;
	for(i=0; i<12; i++)		
	{
		dat <<= 1;
		CLK = 1;
		CLK = 0;
		dat |= DOUT;
	}
	return dat;	
}


//Read data from ADC
uint Read_AD_Data(uchar cmd)
{
	uchar i;
	uint AD_Value;

	CLK = 0;
	CS  = 0;
	SPI_Write(cmd);
	for(i=6; i>0; i--); 	
	CLK = 1;	  
	_nop_();
	_nop_();
	CLK = 0;
	_nop_();
	_nop_();
	AD_Value=SPI_Read();	// read data (by SPI)
	CS = 1;

	return AD_Value;	
}
