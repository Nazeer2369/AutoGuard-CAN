#include <lpc21xx.h>
#include "can_structure.h"
#include "delay.h"
#include "lcd.h"
#include "can_defines.h"
#include "i2c.h"
//mma7660fc
//slave address 0x4c
//port0.1
//port0.7
#define CH_NO_0 14
#define CH_NO_2 16

main_node RXF_F;
int flag_eint0=0;
int flag_eint2=0;
int total_acc=0;

//port
//EINT0 0.1
//EINT2 0.7
//0x2000000C

void eint0_isr()__irq
{   
    // Toggle EINT0 flag
    // 0 -> 1 or 1 -> 0
	flag_eint0^=1;
    // Reset EINT2 flag
    // Ensures only one mode is active at a time
	flag_eint2=0;
    // Clear EINT0 interrupt flag
	EXTINT=1<<0;
    // Inform VIC that ISR processing is complete
    VICVectAddr=0;
    
}
void eint2_isr()__irq
{
    // Toggle EINT2 flag
    // 0 -> 1 or 1 -> 0
	flag_eint2^=1;
    // Reset EINT0 flag
    // Ensures only one mode is active at a time
	flag_eint0=0;
    // Clear EINT2 interrupt flag
    EXTINT=1<<2;
     // Inform VIC that ISR processing is complete
    VICVectAddr=0;
    
}
void enable_eint()
{
    // Configure P0.1 as EINT0 and P0.7 as EINT2
    PINSEL0|=0x0000C00C;
    // Enable EINT0 and EINT2 interrupts in VIC
    VICIntEnable=(1<<CH_NO_0)|(1<<CH_NO_2);
    // Configure EINT0 and EINT2 as IRQ interrupts (not FIQ)
    VICIntSelect&=~((1<<CH_NO_0)|(1<<CH_NO_2));
    // Assign EINT0 ISR address to Vector Slot 0
    VICVectAddr0=(unsigned int)eint0_isr;
    // Assign EINT2 ISR address to Vector Slot 2
    VICVectAddr2=(unsigned int)eint2_isr;
    // Enable Vector Slot 0 and connect it to EINT0 channel
    VICVectCntl0=(1<<5) |(CH_NO_0);
    // Enable Vector Slot 2 and connect it to EINT2 channel
    VICVectCntl2=(1<<5) |(CH_NO_2);
    // Configure EINT0 as Edge Triggered interrupt
    // EINT2 remains Level Triggered because bit 2 is not set
    EXTMODE=1<<0;
    // Configure EINT0 for Rising Edge triggering
    // EINT2 remains Low Level triggered because bit 2 is not set
    EXTPOLAR=1<<0;
    
}

int main()
{
    // Structure variable used for CAN transmission
     main_node TXF;
    // Configure indicator LEDs as output
    // P0.23 and P0.24 set as output pins
	IODIR0|=3<<23;
     // Initialize and enable external interrupts (EINT0, EINT2)
	enable_eint();
    // Initialize 16x2/20x4 LCD
	Init_LCD();
    // Initialize CAN1 controller
	Init_CAN1();
    // Initialize I2C peripheral
	Init_I2C();
    // Set CAN message identifier to 0x02
	TXF.can_id=0X2;
    // Data Length Code = 4 bytes
	TXF.node_bit_field.dlc=4;
    // RTR = 0 ? Normal Data Frame (not Remote Frame)
    TXF.node_bit_field.rtr=0;
    // Clear EINT0 event counter/flag
	flag_eint0=0;
    // Clear EINT2 event counter/flag
	flag_eint2=0;
    // Initialize MMA7660FC accelerometer
	accelerometer_init();
    // Move cursor to first row, first column
	cmd_lcd(0x80);
     // Display project title
  	display_string("---Auto Guard-CAN---");
    // Move cursor to second row, first column
	cmd_lcd(0xc0);
    // Display fuel label
	display_string("Fuel:");
    // Move cursor to specified LCD position
	cmd_lcd(0x99);
    // Display indicator status label
		display_string("Indicator");
  while(1)
  {
    // Read current accelerometer data
  	read_accelerometer();
	// Check whether EINT0 event occurred (e.g., left indicator switch pressed)	
	if(flag_eint0==1)
	{
        // Turn ON indicator connected to P0.23 (assuming active-low LED)
		IOCLR0=1<<23;
        // Store 'L' in CAN frame data to indicate LEFT turn
        TXF.data1='L';			
       // Transmit CAN message containing left indicator status
       tx_main_node(TXF);			
        // Turn OFF or reset indicator connected to P0.24
		IOSET0=1<<24;
	}
	if(flag_eint2==1)
	{
        // Turn ON indicator connected to P0.24 (assuming active-low LED)
		IOCLR0=1<<24;
        // Store 'R' in CAN frame data to indicate RIGHT turn
        TXF.data1='R';
        // Transmit CAN message containing right indicator status
        tx_main_node(TXF);
        // Turn OFF or reset indicator connected to P0.23
		IOSET0=1<<23;
	}
	 if(flag_eint0==0) 
	 {
         // No left indicator request, send neutral/OFF status
		TXF.data1='1';
        // Transmit CAN frame with indicator OFF status
        tx_main_node(TXF);
		IOSET0=3<<23;
  	}
     // Check whether EINT2 switch/event is active
	if(flag_eint2==0) 
	{   
        // Send fuel level request/message through CAN        
        TXF.data1='2';
        tx_main_node(TXF);
         // Turn ON outputs connected to P0.23 and P0.24
        IOSET0=3<<23;
  	}
    // Check whether both EINT0 and EINT2 are active
	if((flag_eint0==0)&&(flag_eint2==0)) 
	{	
        // Send 'O' message through CAN
        // Could represent OFF/OPEN/OK depending on your application
        TXF.data1='O';
        tx_main_node(TXF);
  	}
    // Small delay before checking for response
	delay_ms(200);
    // Check if a CAN message has been received
    // RBS = Receive Buffer Status
    if(((C1GSR>>RBS)&1)==1)
    {	
        // Clear previous LCD contents
        cmd_lcd(0xc0);
        display_string("      ");
        // Display Fuel label
        cmd_lcd(0xc0);
        display_string("Fuel:");
        // Clear old fuel value
        cmd_lcd(0xc6);
        display_string("     ");
        // Receive and display fuel percentage/value
        cmd_lcd(0xc6);
        rx_main_node(&RXF_F);
    }
    else
    {
        // No CAN response received
        cmd_lcd(0xC0);
        display_string("Fuel!!   ");
    }	 
    
  }      
}
