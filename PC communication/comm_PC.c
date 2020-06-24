/*
Author:     Siyuan Tian (tiansy97), Ruiqi Hu
Time:       2018/10
Functions:  The code enables bidirectional serial communication between the microcontroller
            and the PC. The MCU transmits a string to the computer about every 3 seconds, and 
            it receives 8-bit numbers from the PC. It then decodes them and displays on LEDs.   
Concepts:   Serial communication, timer and timer interrupt, BCD (Binary-Coded Decimals), 
            segment displays, etc  
*/

// #include<reg51.h>
// #include<intrins.h>
// #include<absacc.h>

#define uchar unsigned char
#define uint  unsigned int 

#define dg1 0x7ff8
#define dg2 0x7ff9
#define dg3 0x7ffa
#define dg4 0x7ffb

uint Ax;
uint Bx;
uint Cx;
uint Dx;

uint Tflag;
uint st;
uchar *Tbyte;
uchar Rbyte;
uchar xdata *pdg;

uchar xdata Str[] = "I Love China\n";
uchar xdata Led[10] =	{0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90};

void Initialize(void);
void Receive(void);
void Transimit(void);
void Delay(uchar);
void Display(void);
void BCD4(uint sum);
	
char code dx516[3] _at_ 0x003b;


void main()
{
	Initialize();
	while(1)
    {
        if (Tflag == 1)	   //Tflag is changed by Timer 0 interrupt
        {
            Transimit();
         }
	    if (RI == 1)
		{
			Receive();
		}
    }

}


void Timer0() interrupt 1
{
    TH0 = 0x10;	   //x0=0x1000.
    TL0 = 0x00;
	TR0 = 0;
	st-- ;
	if (st == 0)    //The number counted by Timer 0 
	{
	    Tflag = 1;    //If the number is reduced to 0, the transmit flag is set to be 1. 
		st = 15;  	  //Reset st.	
		Transimit();	              		                 
	}
	TR0 = 1;
}


void Initialize(void)
{
    Tflag = 0;
	st = 15;
 
    TMOD = 0x21;   //0010 0001:T1 method 2, T0 method 1.
	PCON = 0x00;   //SMOD=0.
	SCON = 0x50;   //0101 0000:communication method 1.
	
	TH1 = 0xfd;    //Set Baud rate. 9600=(1/32)*(fosc/12)*(1/(256-x1)),x1=253.
    TL1 = 0xfd;
    TH0 = 0x10;	   //x0=0x1000.
    TL0 = 0x00;
 
    ET0 = 1;
    ET1 = 0;
    EA = 1;
    EX0 = 1;


    TR1 = 1;
	TR0 = 1;
}

void Receive(void)
{
	RI = 0;	
    Rbyte = SBUF; 	 //Receive data
	Display();
}


void Transimit(void)
{
	Tflag = 0;
    Tbyte = &Str;		   //Tbyte points to str.
    
	while((*Tbyte)!= '\0') //While there is something remaining
    {
        SBUF = *Tbyte;
        while(!TI);    //Wait until TI=1 (finish transmiting).
        TI = 0;
        Tbyte++;
    }  	
}


void Delay(uchar n)
{
    uchar i;
    while (n--)
    {
        for (i = 255; i > 0; i--);
    }
}


void Display(void)		//Display the 4 digit number on segment LEDs
{
	BCD4(Rbyte);
	pdg = dg4;
    *pdg = Led[Ax];
    Delay(10);
    pdg = dg3;
    *pdg = Led[Bx];
    Delay(10);     
    pdg = dg2; 
    *pdg = Led[Cx];
    Delay(10);
    pdg = dg1;
    *pdg = Led[Dx];
    Delay(10);
}


void BCD4(uint sum)	    //Code the number (8-bit) with four binary numbers
{
      Ax = sum / 1000;
      Bx = (sum % 1000) / 100;
      Cx = (sum % 100) / 10;
      Dx = sum % 10;
}