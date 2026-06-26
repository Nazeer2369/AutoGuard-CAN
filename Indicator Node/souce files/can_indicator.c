#include<lpc21xx.h>
#include "node_structures.h"
#include"can_defines.h"
#include "delay.h"
#define NODE_2 2
#define RXERR 16
#define TXERR 24

main_node indicator;
void delay_ms(unsigned int i)
{   
    for(i*=12000;i>0;i--);
}

   /*
        1. Wait for message reception
        2. Read CAN ID
        3. Read RTR and DLC
        4. Read DATA
        5. Release RX buffer
        
        C1GSR --> Check RX buffer status
           |
        C1RID --> Read CAN ID
           |
        C1RFS --> Read RTR + DLC
           |
        C1RDA --> Read data 1-4
           |
        C1RDB --> Read data 5-8
           |
        C1CMR --> Release RX buffer
*/

//C1GSR(Status register)
/*
      31:24    21:16                   6  5   4  3  2    1   0
    ------------------------------------------------------------
    | TXERR|RXERR|  X  |       X    |BS|ES|TS|RS|TCS|TBS|DOS|RBS|
    ------------------------------------------------------------
    0:RBS-RECECIVE buffer status 0:no info 1:data available
    2:tx buffer status 1:data available to send 0:no data avaialble to send
    3:tx complete status 1:tx done 0:not done
    4:receive status 1:controllere receving 
    5:transmit status 1: controller txmiting
    7:BUS status 1:CAN controller disconnected from bus.
    
    
*/
void rx_indicator(main_node *RXF)
{
	unsigned int i;
	//main_node RXF;
    // Wait until a CAN message is received
    // RBS = Receive Buffer Status
    while(((C1GSR>>RBS)&1)==0);
    // Read received CAN Identifier
    RXF->can_id=C1RID;
    // Read RTR and DLC information
    RXF->node_bit_field.rtr=((C1RFS>>RTR)&1);
    RXF->node_bit_field.dlc=((C1RFS>>DLC)&0X0F);
    // Check whether received frame is a Data Frame
    // RTR = 0 ? Data Frame
    // RTR = 1 ? Remote Frame
    if(RXF->node_bit_field.rtr==0)
    {
        // Read received CAN data
        RXF->data1=C1RDA;
        RXF->data2=C1RDB;
    }
    // Process messages coming from NODE_2
	if(RXF->can_id==NODE_2)
    {
        // Right Indicator Command
        if(RXF->data1=='R')
		{  
            // Turn OFF all LEDs initially
			IOSET0=0XFF<<0;
            // Right-side running indicator
			i=4;
			while(i<=8)
			{
				IOCLR0=1<<i;
				delay_ms(100);
				i++;
			}	
		}
        // Left Indicator Command
		if(RXF->data1=='L')
		{  
             // Turn OFF all LEDs initially
            IOSET0=0XFF<<0;
            // Left-side running indicator
			i=3;
			do
			{
				IOCLR0=1<<i;
				delay_ms(100);
			}while(i--);
		}
        // Vehicle Side-1 ON
		else if(RXF->data1=='1')
		{
		 IOSET0=0X0F<<0;
		}
		else if(RXF->data1=='2')
		{
		 IOSET0=0XF0<<0;
		}
         // Turn OFF all LEDs
		else if(RXF->data1=='O')
		{
		 IOSET0=0XFF<<0;
		}
        // Crash / Danger Alert
		if(RXF->data1=='D')
		{
            // Blink all LEDs as hazard lights
			IOCLR0=0XFF<<0;
			delay_ms(100);
			IOSET0=0XFF<<0;
			delay_ms(100);
		}
    }
    // Release CAN Receive Buffer
    // RRB = Release Receive Buffer
    C1CMR=1<<RRB;//bit pos 2    
}
void tx_indicator_node(main_node TXF)
{
    // Wait until CAN Transmit Buffer is available
    // TBS = Transmit Buffer Status
    while(((C1GSR>>TBS)&1)==0);
     // Load CAN Identifier into Transmit Identifier Register
    C1TID1=TXF.can_id;
    // Load DLC (Data Length Code) and RTR (Remote Transmission Request)
    C1TFI1 = (TXF.node_bit_field.dlc << DLC) |
         (TXF.node_bit_field.rtr << RTR);
    // Check whether it is a Data Frame
    // RTR = 0 ? Data Frame
    // RTR = 1 ? Remote Frame
    if(TXF.node_bit_field.rtr==0)
    {
        // Load first 4 bytes of CAN data
        C1TDA1=TXF.data1;
        // Load next 4 bytes of CAN data
        C1TDB1=TXF.data2;
    }
    // Request transmission
    // STB = Select Transmit Buffer
    // TR  = Transmission Request
    C1CMR=(1<<STB)|(1<<TR);
    // Wait until transmission is completed
    // TCS = Transmission Complete Status
    while(((C1GSR>>TCS)&1)==0);
}
int main()
{
    // Set P0.0 to P0.7 high initially
     IOSET0=0XFF<<0;
    // Configure P0.0 to P0.7 as output pins
    IODIR0|=0xFF<<0;
     // Again set P0.0 to P0.7 high
	IOSET0=0XFF<<0;
	// Initialize CAN1 controller
    Init_CAN1();    
   while(1)
   {
      // Receive CAN frame and store in indicator structure
       rx_indicator(&indicator);
   }       
}
