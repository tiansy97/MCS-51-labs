#include<reg51.h>
#define uchar unsigned char
#define uint unsigned int

#define dg1 0x7ff8
#define dg2 0x7ff9
#define dg3 0x7ffa
#define dg4 0x7ffb

sbit SW1=P1^0;    //Increase 5ms by pressing switch 1
sbit SW2=P1^1;       //Decrease 5ms by pressing switch 2
sbit OUTPUT=P1^5;     //Output Port
uchar TIMER0_L, TIMER0_H;
uint period;
uchar  xdata  *pdg;		//Pointer used to display numbers on LED
uchar table[10] = {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90};

void delay(uchar);
void Initialize(void);
void Display(void);
bit SW1Down(void);
bit SW2Down(void);

char code dx516[3] _at_ 0x003b; 


void main()			
{
    Initialize();			  
    while(1)
    {
        if (SW1Down())
        {
            if (period == 50)
            {
                period=5;
                TIMER0_H=0xf7;
            }
            else
            {
                period+=5;
                TIMER0_H-=0x09;
            }
        }
        if (SW2Down())
        {
            if (period == 5)
            {
                period=50;
                TIMER0_H=0xa6;
            }
            else
            {
                period-=5;
                TIMER0_H+=0x09;
            }
        }
        Display();
    }
}


void Initialize(void)
{
    P1 = 0xff;
    TIMER0_L = 0x00;		//Set Timer0 low and high
    TIMER0_H = 0xd3;
    period = 25;


    TMOD=0x01;				//Timer0£ºGate 0£¬Method 1
    IT0=1;				   
    EX0=1;
    ET0=1;
    EA=1;

    TL0 = TIMER0_L;
    TH0 = TIMER0_H;

    TR0=1;					//T0 On
}


void Display(void)		  //Display period on the LED
{
    pdg = dg1;
    *pdg = table[0];
    delay(10);
    pdg = dg2;
    *pdg = table[0];
    delay(10);
    pdg = dg3;
    *pdg = table[(int)(period%10)];
    delay(10);
    pdg = dg4;
    *pdg = table[(int)(period/10)];
    delay(10);
}


bit SW1Down(void)
{
    if (SW1 == 0)
    {
        delay(10);                  //Check twice, debounce
        if (SW1 == 0) 
		{
		    while (1)				//Return 1 when the switch is released
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


void Timer0() interrupt 1	   		//Timer0 interrupt
{
    TR0=0;
    OUTPUT=!OUTPUT;		//Invert the output to produce square waves
    TL0=TIMER0_L;
    TH0=TIMER0_H;
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
	















