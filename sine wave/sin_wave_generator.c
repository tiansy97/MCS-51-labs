/*
Author:     Siyuan Tian (tiansy97), Ruiqi Hu
Time:       2018/11
Functions:  The code generates a sine wave and changes its amplitude by pressing switches.
            To do that, it updates the output to the D/A converter every 1 ms (using timer 
            interrupt). It also displays the current amplitude of the wave on LEDs.
Concepts:   I/O port, D/A conversion, timer and timer interrupt, segment displays, etc  
*/


#include<reg52.h>
#include<intrins.h>
#include<absacc.h>

#define uchar unsigned char
#define uint unsigned int

#define LED4 XBYTE[0x7FF8]      // Directly set to be pointers rather than addresses
#define LED3 XBYTE[0x7FF9]
#define LED2 XBYTE[0x7FFA]
#define LED1 XBYTE[0x7FFB]
#define DAC XBYTE[0xEFFF]
#define VMAX 10.0

void delay(uchar);
void Initialize(void);
void Display(void);
bit SW1Down(void);
bit SW2Down(void);

sbit SW1=P1^0;
sbit SW2=P1^1;

uchar TIMER0_L, TIMER0_H;
uint value;
uint amp, st;
float ampref;

uchar code table[11] = {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xc1}; 
uchar code sin_value[100] = {128,136,143,151,159,167,174,182,189,196,
                             202,209,215,220,226,231,235,239,243,246,
							 249,251,253,254,255,255,255,254,253,251,
							 249,246,243,239,235,231,226,220,215,209,
							 202,196,189,182,174,167,159,151,143,136,
							 128,119,112,104, 96, 88, 81, 73, 66, 59,
							 53, 46 ,40 ,35 ,29 , 24 ,20 ,16, 12,  9,
							 6,  4,  2,  1,  0,   0,  0,  1,   2,  4,
							 6,  9,  12, 16, 20,  24, 29, 35, 40,  46,
							 53, 59, 66, 73, 81,  88, 96, 104,112, 119};
                             // Pre-compute and store 100 data points for one wave period


bit SW1Down(void)
{
    if (SW1 == 0)
    {
        delay(10);                  
        if (SW1 == 0) 
		{
		    while (1)
		    {
		        if (SW1 == 1)
			    return 1 ;
		    }
		}			
		else return 0;
    }
    else return 0;
}


bit SW2Down(void)
{
    if (SW2 == 0)
    {
        delay(10);                  
        if (SW2 == 0) 
		{
			
		    while (1)		
		    {
		        if (SW2 == 1)
			    return 1 ;
		    }
		}				
		else return 0;
    }
    else return 0;
}


void Timer0() interrupt 1
{
	TL0=TIMER0_L;        
    TH0=TIMER0_H;           
    
	st += 1;
    if (st >= 100) st = 0;

    value = (int)(sin_value[st] * ampref);		// Update the current output (maximum is 255)														              
    DAC = value;            // Send the value to D/A converter
             
}


void Display()
{
    LED1 = 0xff;
    delay(10);
    LED2 = table[amp/10];
    delay(10);
    LED3 = table[amp%10];
    delay(10);
    LED4 = table[10];
    delay(10);
}


void Initialize()
{
    st = 0; 
    amp = 5;
	ampref = (float)amp/VMAX;
    value = 0;
       
	TIMER0_L = 0x66;       // Update the output every 1ms (100 times per period, the period is 100ms)
    TIMER0_H = 0xFC;

    TMOD=0x01;                
    ET0=1;
    EA=1;  

    TL0 = TIMER0_L;       
    TH0 = TIMER0_H;
            
    TR0=1;               
}   


void delay(uchar n)
{
    uchar i;
    while (n--)
    {
        for (i = 255; i > 0; i--);
    }
}


char code dx516[3] _at_ 0x003b; 


void main()       
{
    Initialize();
	Display();
    while(1)
    {   
        if (SW1Down())      //Increase amplitude
        {        
            amp++;
			ampref = (float)amp/VMAX;    //The ratio of the current amplitude and the max amplitude
            if (amp > 10) 
			{
    			amp = 5;
    			ampref = (float)amp/VMAX;
			}
            Display();
        }
		if (SW2Down())        //Decrease amplitude
        {        
            amp--;
			ampref = (float)amp/VMAX;
            if (amp < 5) 
			{
    			amp = 10;
    			ampref = (float)amp/VMAX;
			}
            Display();
        }   
    }    
}
