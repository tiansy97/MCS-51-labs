/*
Author:     Siyuan Tian (tiansy97), Ruiqi Hu
Time:       2018/12
Functions:  The code reads the water temperature target from a BCD rotary switch. It measures 
            the current water temperature in the kettle by a thermistor circuit and ADC. Then 
            it compares these two and controls the relay to turn on/off heating to make the 
            water temperature reach its target value.
Concepts:   Temperature sensing, A/D conversion, relay control circuit, segment displays, etc  
*/

#include<reg51.h>
#include <absacc.h>
#include<intrins.h>
#define uchar unsigned char
#define uint unsigned int

#define dg4 0x7ff8
#define dg3 0x7ff9
#define dg2 0x7ffa
#define dg1 0x7ffb
#define dg5 0xbfff
#define T_ADDR 0xDFFA

#define dg6 0x7FFC

uchar xdata *pdg;


uchar code table[10] = {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90};

uchar set,seti,setm;

sbit W1=P1^7; 
sbit EOC=P3^3; 
uint Tset,Trel,num1, num2,buffer, Tlast, Tll, flag;
int Poor;
float temp;


void Delay(uchar);  
void Display(void);
void mapper(uint, uint);
void getnum(void);       
void BCD(void);


void Display(void)
{

    pdg = dg1;
    *pdg = table[num1];
    Delay(10);
    pdg = dg2;
    *pdg = table[num2];
    Delay(10);     
    pdg = dg3; 
    *pdg = table[seti];        
    Delay(10);
    pdg = dg4;
    *pdg = table[setm];
    Delay(10);
}


void Delay(uchar n)
{
    uchar i;
    while (n--)
    {
        for (i = 255; i > 0; i--);
    }
}


void mapper(uint st, uint ed) // Map the temperature into the correct value
{
    temp = (float)st + (float)(ed-st)*((float)buffer / 256.0);
}


void getnum()
{
    num1 = (uint)(temp) / 10;
    num2 = (uint)(temp) % 10;
    
}


void BCD(void)  // Read the target temperature
{ 
   W1 = 0;
   pdg = dg5;
   set = *pdg;
   set = ~set;	      
   setm = set&0x0f;   
   seti = set>>4;
}   	


void main(void)       
{   
    num1 = num2 = 0;
    buffer = 0;
    Tlast = 0;
    Tll = 0;
    temp = 0.0;
    flag = 0;

    while(1)
    {
        BCD();     
        pdg = T_ADDR;
        *pdg = 1;       // Any write operation 
        while (EOC);
        buffer = *pdg;
        mapper(0, 125);     
        getnum();
        Display();

        Tset=10*seti+setm;
        Trel=10*num1+num2;
        Poor=Tset-Trel;     // The  difference between the target temp and current temp
        
        if (Poor <= 0)      // If overheat, stop heating
        {
            pdg = dg6;
            *pdg=0x00;
        }
        else if (Poor>=10)      // Keep heating
            {
                pdg = dg6;
                *pdg=0x02;  
            }
            else if(Poor>=5)    // Heat for 1/2 of the time
            {
                pdg = dg6;
                *pdg =0x02; 
                Delay(400);
                pdg = dg6;
                *pdg= 0x00;  
                Delay(400);
            }
            else 
			{
			    if (Poor>=3)     // Heat for 1/4 of the time
                {
                    pdg = dg6;
                    *pdg =0x02; 
                    Delay(200);
                    pdg = dg6;
                    *pdg=0x00;  
                    Delay(600);
                }
                else
                {
                    if (Poor>=1)    // Heat for 1/6 of the time
                    {
                        pdg = dg6;
                        *pdg =0x02; 
                        Delay(200);
                        pdg = dg6;
                        *pdg=0x00;  
                        Delay(1000);
                    }
                    else
                    {
                        pdg = dg6;
                        *pdg=0x00;
                    }
                }
			}
    }

        
}