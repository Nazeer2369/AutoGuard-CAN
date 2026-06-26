#include<lpc21xx.h>
#define BTR 125000

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
