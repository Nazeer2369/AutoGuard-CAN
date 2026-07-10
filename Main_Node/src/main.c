#include<lpc21xx.h>
#include"can_defines.h"
#include "delay.h"
#define NODE_2 2
#define RXERR 16
#define TXERR 24

main_node indicator;
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
