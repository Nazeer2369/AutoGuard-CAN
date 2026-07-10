#include <lpc21xx.h>
#include "can_structure.h"
#include "lcd.h"
#include "delay.h"
#include "can_defines.h"
#include "indicator_function.h"

#define BTR 125000
#define NODE_1 1
#define NODE_2 2
#define NODE_3 0x3
int I_flag=0;
int F_flag=0;
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
              -----------------------------------------------------------------------------------
              |    31:24 21:16                          5  4  3  2    1   0                      |
              |  ------------------------------------------------------------                    |
              |  | TXERR|RXERR|  X  |       X         | TS|RS|TCS|TBS|DOS|RBS|                   |
              |  ------------------------------------------------------------                    | 
              |  0:RBS-RECECIVE buffer status 0:no info 1:data available                         | 
              |  2:tx buffer status 1:data available to send 0:no data avaialble to send         | 
              |  3:tx complete status 1:tx done 0:not done                                       | 
              |  4:receive status 1:controllere receving                                         |
              |  5:transmit status 1: controller txmiting                                        | 
              |                                                                                  |
              |  31:24 -TXERR:if(255)bus  disconnected                                           | 
              |                  0:alive                                                         |
              ---------------------------------------------------------------------------------
    
*/
void rx_main_node(main_node *RXF);
void tx_main_node(main_node TXF);

void Init_CAN1(void)   
{   
    //cfg p0.25 pin as CAN1_RX pin(RD1),TD1 is exclusive    
    PINSEL1|=0x00040000; //using defines from can_defines.h   
                      // #define RD1_PIN 0x00040000 ,    
                      //as RD1/ (i.e CAN1_RX)/p0.25   
    //Reset CAN1 controller   
    C1MOD=1;   
    //All received messages are accepted       
    AFMR=2;    
    //Set baud Rate for CAN   
    C1BTR=BTR; //using defines from can_defines.h   
    //Enable CAN1 controller   
    C1MOD&=~(1<<0);    
}

void tx_main_node(main_node TXF)
{   
    // Wait until Transmit Buffer 1 becomes free
    while(((C1GSR>>TBS)&1)==0);
    // Load CAN Identifier (ID) into Transmit Identifier Register
    C1TID1=TXF.can_id;
    // Load DLC (Data Length Code) and RTR bit into
    // Transmit Frame Information Register
    C1TFI1 = (TXF.node_bit_field.dlc << DLC) |(TXF.node_bit_field.rtr << RTR);
	// If RTR = 0, it is a Data Frame
    if(TXF.node_bit_field.rtr==0)
    {
        // Load data bytes 1-4 into Transmit Data Register A
        C1TDA1=TXF.data1;
        // Load data bytes 5-8 into Transmit Data Register B
        C1TDB1=TXF.data2;
    }
   
    // Request CAN transmission
    // STB = Select Transmit Buffer
    // TR  = Transmission Request
    C1CMR=(1<<STB)|(1<<TR);
    // Wait until transmission is completed successfully
    while(((C1GSR>>TCS)&1)==0);
        // Display Left Arrow indication if transmitted data is 'L'
		if(TXF.data1=='L')
		arrow('L');
        // Display Right Arrow indication if transmitted data is 'R'
		if(TXF.data1=='R')
		arrow('R');
		
}
void rx_main_node(main_node *RXF)
{
	// Wait until a CAN message is received in the RX buffer
    while(((C1GSR>>RBS)&1)==0);
    // Read the received CAN Identifier (ID)
    // C1RID --> Read CAN ID
    RXF->can_id=C1RID;
    // Read RTR (Remote Transmission Request) bit
    RXF->node_bit_field.rtr=((C1RFS>>RTR)&1);
    // Read DLC (Data Length Code - number of data bytes)
    RXF->node_bit_field.dlc=((C1RFS>>DLC)&0X0F);
    // Check whether the received frame is a Data Frame
    // RTR = 0 ? Data Frame
    // RTR = 1 ? Remote Frame
    if(RXF->node_bit_field.rtr==0)
    {
         // Read received data byte(s) from CAN Receive Data Register A
        RXF->data1=C1RDA;
       // rxf->data2=C1RDB;   
    }
    // Display received value on LCD at cursor position 0xC6
	cmd_lcd(0xc6);
    // Display received data value
	display_integer(RXF->data1);
    // Display '%' symbol after the value
	display_char('%');
    // Small delay for LCD visibility
	delay_ms(500);
    // Release the CAN receive buffer
    // Allows CAN controller to accept the next incoming message
	C1CMR|=1<<RRB;
}
