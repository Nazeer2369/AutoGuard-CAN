#include<lpc21xx.h>
#include "can_defines.h"
#define TD1


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