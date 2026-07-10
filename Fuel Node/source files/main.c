#include<lpc21xx.h>
#include "can_defines.h"
#include "can_structure.h"
#include"adc.h"
#include "delay.h"

int min=3;
int max=33;
int main()
{   int fuel_per;
     main_node fuel;
    // Initialize ADC for fuel sensor reading
    init_adc();
    // Initialize CAN controller
    Init_CAN1();
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
       // Update every 1 second
	   delay_ms(500);  
   }       
    
}
