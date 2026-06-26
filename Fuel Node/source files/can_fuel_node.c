#include<lpc21xx.h>
#include "can_defines.h"
#include"adc.h"
#include "delay.h"
#include"lcd.h"
#include "node_structures.h"

int min=3;
int max=33;


       /*
        1. Check TX buffer empty
        2. Load CAN ID
        3. Load RTR and DLC
        4. Load DATA
        5. Start transmission
        6. Wait until transmission complete
        
        C1GSR --> Check TX empty
           |
        C1TID1--> Load CAN ID
           |
        C1TFI1--> Load RTR + DLC
           |
        C1TDA1--> Load data 1-4
           |
        C1TDB1--> Load data 5-8
           |
        C1CMR-->  Start TX
           |
        C1GSR--> Check TX complete
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
    
    31:24 -TXERR:if(255)bus  disconnected 
                    0:alive
    
    
*/
void tx_fuel_node(main_node TXF)
{
    // Wait until CAN Transmit Buffer becomes free
    // TBS = Transmit Buffer Status
    while(((C1GSR>>TBS)&1)==0);
    // Load CAN Identifier into Transmit Identifier Register
    C1TID1=TXF.can_id;
    // Load Frame Information
    // DLC = Number of data bytes
    // RTR = Data Frame (0) or Remote Frame (1)
    C1TFI1 = (TXF.node_bit_field.dlc << DLC) |
             (TXF.node_bit_field.rtr << RTR);
    // For Data Frame, load transmit data
    if(TXF.node_bit_field.rtr==0)
    {
         // Load first 4 bytes of data
        C1TDA1=TXF.data1;
    }
    // Start CAN transmission
    // STB = Select Transmit Buffer
    // TR  = Transmission Request
    C1CMR=(1<<STB)|(1<<TR);
    // Wait until transmission is completed
    // TCS = Transmission Complete Status
    while(((C1GSR>>TCS)&1)==0);
}

void rx_fuel_node (main_node *rxf)
{
    // Wait until a CAN message is received
    // RBS = Receive Buffer Status
    while(((C1GSR>>RBS)&1)==0);
    /// Read received CAN Identifier
    rxf->can_id=C1RID;
    // Read Frame Information
    // RTR = Remote Transmission Request bit
    // DLC = Data Length Code (number of bytes received)
    rxf->node_bit_field.rtr=((C1RFS>>RTR)&1);
    rxf->node_bit_field.dlc=((C1RFS>>DLC)&0X0F);
    // Check whether received frame is a Data Frame
    // RTR = 0->Data Frame
    // RTR = 1-> Remote Frame
    if(rxf->node_bit_field.rtr==0)//DATA frame 1: remote frame
    {
        // Read received data bytes 1–4
        rxf->data1=C1RDA;
         // Read received data bytes 5–8
        rxf->data2=C1RDB;
        
    }
    
    // Release CAN Receive Buffer
    // RRB = Release Receive Buffer
    // Allows CAN controller to receive the next message
    C1CMR=1<<RRB;
} 
int read_fuel()
{
	int eAval;
    // Read ADC value from fuel sensor
    // eAval stores the converted digital value
	read_adc(&eAval);
    // Return fuel sensor reading
	return eAval;

}
int main()
{   int fuel_per;
     main_node fuel;
    // Initialize ADC for fuel sensor reading
    init_adc();
    // Initialize CAN controller
    Init_CAN1();
    // Initialize LCD display
	Init_LCD();
    // Configure CAN frame parameters
    // Fuel node CAN ID
    fuel.can_id=0x3;
    // Data Length Code
    fuel.node_bit_field.dlc=8;
    // Data Frame
    fuel.node_bit_field.rtr=0;
    
    
   while(1)
   {
        // Read fuel sensor value through ADC
       fuel_per=read_fuel();//(&eAval);
       // Convert ADC value to fuel percentage
	   fuel.data1=(((fuel_per-min)*100)/(max-min)) ;
       // Transmit fuel percentage through CAN Bus
       tx_fuel_node(fuel);
       // Display fuel percentage on LCD
       cmd_lcd(0x8C);
       // Clear previous value
       display_string("    ");
       display_integer(fuel_per);
       display_char('%');
       // Update every 1 second
	   delay_ms(500);
        //rx_fuel_node (&fuel);       
   }       
    
}
