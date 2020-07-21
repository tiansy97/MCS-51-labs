/*
Author:     Siyuan Tian (tiansy97), Ruiqi Hu
Time:       2018/11
Functions:  The system measures the room temperature using a temperature sensor. The analog
            output is converted to diginal signals by the ADC0809 chip. The code reads the 
            signal and converts it to real temperatures, and it applies a digital filter to 
            stablize the readings. It then displays the temperature on LEDs.    
Concepts:   Sensor circuit design, A/D conversion, digital filtering, segment displays, etc  
*/


#include<reg51.h>
#include<intrins.h>
#include<absacc.h>

#define uchar unsigned char
#define uint  unsigned int 

#define dg4 0x7ff8
#define dg3 0x7ff9
#define dg2 0x7ffa
#define dg1 0x7ffb
#define ADC0809 0xdff8     //Address of ADC0809 

sbit EOC=P3^3;
uchar xdata *pdg;
uchar xdata *p;
uint result;
uint Aa,Bb,Cc,Dd;
float temp;
float last;
float now;
float para;

uchar xdata table[10] = {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90};
   

void Initialize(void);
void display(void);
void delay(uchar);  
void transfer(uint, uint);
void bcd(void);
void filter(void);


void main()
{
    Initialize();
    while(1)
    {
        p = ADC0809;
        *p = 255;           //Any number, activate ADC
        while (EOC);        //When the AD process finishes, EOC=0
        result = *p;         //Get the result from ADC
        transfer(0,50);
        filter();
        bcd();
        display();
    }
}


void Initialize(void)       //Initialize parameters
{
    Aa = Bb = Cc = Dd = 0;
    result = 0;
    temp = 0.0;
    last = 0.0;
    now = 0.0;
    para = 0.01;
}


void transfer(int min, int max)    
{
    temp = (float)min + (float)(max-min)*((float)(result) / 256.0);
    //Turn the measures into real temperature (float)
}


void filter(void)                 // First-order RC filtering
{
    now = (1.0 - para)*last + para * temp;
    last = now;
    temp = now;
}


void bcd(void)
{
    Aa = 0;                            //Hundreds
    Bb = (uint)(temp) / 10;            
    Cc = (uint)(temp) % 10;
    Dd = (uint)(temp*10.0) % 10;       //Tenths
}


void display(void)
{
    pdg = dg1;
    *pdg = table[Aa];
    delay(10);
    pdg = dg2;
    *pdg = table[Bb];
    delay(10);     
    pdg = dg3; 
    *pdg = table[Cc]-0x80;      //Number+dot
    delay(10);
    pdg = dg4;
    *pdg = table[Dd];
    delay(10);
}


void delay(uchar n)
{
    uchar i;
    while (n--)
    {
        for (i = 255; i > 0; i--);
    }
}










