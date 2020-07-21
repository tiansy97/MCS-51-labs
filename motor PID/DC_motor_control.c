/*
Author:     Siyuan Tian (tiansy97), Ruiqi Hu
Time:       2018/11
Functions:  The code measures the motor speed from the encoder signal which causes the external
            interrupt. It decodes the speed and receives the speed command from PC. It then uses
            PID control to generate an appropriate PWM wave to control the current motor speed.
Concepts:   Motor speed measurement, PID control, Serial communication, timer and timer interrupt, 
            external interrupt, segment displays, etc  
*/


#include <stdio.h>
#include <reg51.h>
#include <absacc.h>

#define uchar unsigned char

sbit P15 = P1^5;
int comm = 0;
int curr = 0;
int err = 0;
int errlast = 0;
int mark = 0;
int m = 125;
int max = 300;
int numb = 0;
int PWM = 0;
uchar xdata table[10] = {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90};

struct PIDcontrol
{
      uchar P;
      uchar I;
      uchar D;
}
PIDcon = {50,15,0};


int Speed(int);
int PID(void);
void Receive(void);
void Display(void);
void Delay(uchar);  

char code dx516[3] _at_ 0x003b;


void main()
{
    //Initialize the chip
    SP=0x70;    
    SCON=0x50;          //Mode 1(SM1=1,REN=1)
    TMOD=0x21;          //T0:mode 1,T1:mode 2
    PCON=0x00;          //SMOD=0

    TH1=0xfd;           //Baud rate=9600
    TL1=0xfd;

    TH0=0xfe;          //Timer0: T=0.4ms
    TL0=0x8f;

    EA=1;
    ET0=1;
    ET1=0;
    EX1=1;
    IT1=1;
    ES=0;

    PT0=1;              //T0 has high priority
    TR1=1;
    TR0=1;
    RI=0;
    
    //Receive the first command from PC
    TI=1;
    printf("Please input the command speed!\n");
    TI=0;
    //scanf("%c",&comm);
    while (!RI);	
    Receive();
    TI=1;
    printf("The current command speed is %d\n",comm);
    TI=0;

    while (1)
    {          
        if (RI)   
        {
            Receive();
            TI=1;
            printf("The current command speed is %d\n",comm);
            TI=0;
        }
        if (mark)
        {
            m = PID();    // Update m trough PID control (once for every period of the PWM)
            mark = 0;
        }
    	
        // Generate PWM wave
        if(PWM < m) P15 = 1;
        else P15 = 0;
        Display();
    }
}


void Receive(void)
{
    RI = 0;	
    comm = SBUF; 	
}


void timer0(void) interrupt 1 
{
    EA=0;
    TH0=0xfe;
    TL0=0x8f;
    numb++;
    PWM++;
    if(PWM == max)
    {
        mark = 1;       // The flag of one PID control update
        PWM = 0;
    }
    PT0=1;
    EA=1;
}


void int1(void) interrupt 2  // Interrupt from an external pin
{
    EX1=0;
    curr = Speed(numb);
     
    EX1=1;
    PT0=1;
    IE1=0;
}


int Speed(int a)          //Current speed calculation   
{
    int spd;

    numb = 0;
    spd = 2500/a;
    if(spd<5)  spd = 5;
    if(spd>50)  spd = 50;

    return spd;
}


int PID(void)                //PID control
{
    int derr,x;
    static int errsum = 0;

    err = comm-curr;       //current error
    derr = err-errlast;     //deviation of error (current error-last error)
    errsum += err;          //sum of total error
    errlast = err;          //last error
    x = PIDcon.P*err+PIDcon.I*errsum+PIDcon.D*derr;  //PID equation
    if (x<0) x = 0;
    else if (x>max) x = max;
    
    return x;
}


void Display()    // Display the current speed and the command speed
{
    int Comma,Commb,Curra,Currb;

    Comma = comm/10;
    Commb = comm%10;
    Curra = curr/10;
    Currb = curr%10;
    XBYTE[0x7ff8]=table[Currb];
    Delay(10);
    XBYTE[0x7ff9]=table[Curra];
    Delay(10);
    XBYTE[0x7ffa]=table[Commb];
    Delay(10);
    XBYTE[0x7ffb]=table[Comma];
    Delay(10);
}


void Delay(uchar n)
{
    uchar i;
    while (n--)
    {
        for (i = 255; i> 0; i--);
    }
}
