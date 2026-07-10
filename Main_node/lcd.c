#include<lpc21xx.h>
//#include "types.h"
#include "delay.h"

#define RS 16

#define RW 18
#define EN 17

#define DATA 8
//char space[17]="                ";
void write_lcd(int );

void cmd_lcd(int data)
{
	//CMD RS-INSTR SO 0
	// RS:0/1  0:INSTR REG 1:DATA REG
	IOCLR0=1<< RS;
	write_lcd(data);
}

void Init_LCD()
{
    // Configure RS (Register Select) pin as output
	IODIR0|=1<<RS;
    // Configure RW (Read/Write) pin as output
	IODIR0|=1<<RW;
    // Configure EN (Enable) pin as output
	IODIR0|=1<<EN;
    // Configure 8 LCD data pins (D0-D7) as outputs
	IODIR0|=0XFF<<DATA;
	
	delay_ms(15);
	cmd_lcd(0x30);
	
	delay_ms(5);
	cmd_lcd(0x30);
	
	delay_ms(5);
	cmd_lcd(0x30);
	/*
	000D NF--      D:1/0 8BIT BUS 4BIT BUS
	0011 1000      N:1/0 2 LINES 1LINE 
								 F:1/0 5x10 5x8
	*/
	cmd_lcd(0x38);
	//display on/off 
	/*			                                                 
		0000 1DCB
		0000 1111
		D:1/0 1:ON 0:OFF
		C:1/0 1:CURSON ON 0:OFF
		B:1/0 1:BLINK ON  0:OFF
		*/
		cmd_lcd(0x0c);
	//clear display
	cmd_lcd(0x01);
	
	//entry mode
	/*
	direction of address counter
		0000 0 1 ID S
		ID: 0:decrement
				1:increment
		s:shift display 
		    0: off
				1: on
	*/
	//cmd_lcd(0x0f);
	cmd_lcd(0x06);
	
}


void write_lcd(int data)
{
	//we go for write /output device at present
	// RW:0/1  0: WRITE 1: READ
	IOCLR0=1<<RW;
	//clear disply and update command
	IOPIN0= (IOPIN0&~(0XFF<<DATA))|(data<<DATA);
	// enable should high to low for write operation
	// enable should low to high for read operation
	IOSET0=1<<EN;
	//give some delay for updation
	delay_ms(1);
	IOCLR0=1<<EN;
}
void display_char(int data)
{
	//to display data RS should be data register
	//RS: 1 
	IOSET0=1<<RS;
	write_lcd(data);
}

void display_string(char *str)
{
	int count=0,l_cnt=1,i;
	while(*str)
	{   
        // After 20 characters on Line 1
		if(count==20 && l_cnt==1)
		{
			l_cnt++;
			// Fill remaining positions with spaces
			for(i=0;i<20;i++)
			{
				display_char(32);
			}
			///delay_ms(2);
			count=0;
            // Move cursor to Line 2
			cmd_lcd(0xc0);
		}
        // After 20 characters on Line 2
		if(count==20 && l_cnt==2)
		{	
			l_cnt++;
            // Fill line with spaces
			for(i=0;i<20;i++)
			{
				display_char(32);
			}
			count=0;
            // Move cursor to Line 3
			cmd_lcd(0x94);
			l_cnt=3;
		}
        // After 20 characters on Line 3
		if(count==20 && l_cnt==3)
		{	
			l_cnt++;
			// Fill line with spaces
			for(i=0;i<20;i++)
			{
				display_char(32);
			}
			count=0;
			cmd_lcd(0xd4);
			l_cnt=1;
		}
        // After 20 characters on Line 4
		if(count==20 && l_cnt==4)
		{	
			l_cnt++;
			 // Fill line with spaces
			for(i=0;i<20;i++)
			{
				display_char(32);
			}
			count=0;
			cmd_lcd(0x80);
			l_cnt=1;
		}
		count++;
        // Display current character
		display_char(*str);
		// Move to next character in string
		str++;
		
	}
	
}
void display_integer(int n)
{
	char a[10];
	short int i=0;
    // Special case for zero
	if(n==0)
	{
		display_char('0');
	}
	else
	{
        // Extract digits from right to left
		while(n)
		{
            // Convert digit to ASCII
			a[i++]=(n%10)+48;
             // Remove last digit
			n/=10;
		}
        // Display digits in reverse order
		for(--i; i>=0; i--)
		{
			display_char(a[i]);
		}
	}
}

void display_float(float f, char nDP)
{
	int n,i;
    // Check for negative number
	if(f<0)
	{
		display_char('-');
        // Convert to positive
		f=-f;
	}
    // Extract integer part
	n=f;
    // Display integer part
    display_integer(n);
    // Display decimal point
    display_char('.');
    // Display decimal digits
	for(i=0; i<nDP; i++)
	{
        // Shift next decimal digit to integer position
		f=(f-n)*10;
        // Extract digit
		n=f;
		display_char(n+'0');
	}
}


