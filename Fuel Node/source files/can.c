#include<lpc21xx.h>
#include "can_defines.h"
#include"adc.h"
#include "delay.h"
//#include"lcd.h"
#include "can_structure.h"
#define TD1
//void tx_fuel_node(main_node TXF);
//void rx_fuel_node (main_node *rxf);
int read_fuel(void);


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



void Init_CAN1(void)   
{   
    //cfg p0.25 pin as CAN1_RX pin(RD1),TD1 is exclusive    
    PINSEL1|=0x00040000; //using defines from can_defines.h   
                      // #define RD1_PIN 0x00040000 ,    
                      //as RD1/ (i.e CAN1_RX)/p0.25   
    //Reset CAN1 controller 
	//reset mode bit 0 
	//1:reset 0:enable can  
    C1MOD=1;   
    //All received messages are accepted       
    AFMR|=1<<ACCBP;    
    //Set baud Rate for CAN   
    C1BTR=BIT_RATE; //using defines from can_defines.h   
    //Enable CAN1 controller 
	//reset mode bit 0
	//reset mode bit 0 
	//1:reset 0:enable can  
    C1MOD&=~(1);    
}

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

