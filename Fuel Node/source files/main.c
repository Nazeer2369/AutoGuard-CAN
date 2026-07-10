#include<lpc21xx.h>
#include "can_defines.h"
#include "can_structure.h"
#include"adc.h"
#include "delay.h"
#ifdef lcd
#include"lcd.h"
#endif

int min=3;
int max=33;
int main()
{   int fuel_per;
     main_node fuel;
    // Initialize ADC for fuel sensor reading
    init_adc();
    // Initialize CAN controller
    Init_CAN1();
    #ifdef lcd
    // Initialize LCD display
	Init_LCD();
    #endif
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
        #ifdef lcd
       // Display fuel percentage on LCD
       cmd_lcd(0x8C);
       // Clear previous value
       display_string("    ");
       display_integer(fuel_per);
       display_char('%');
       #endif
       // Update every 1 second
	   delay_ms(500);
        //rx_fuel_node (&fuel);       
   }       
    
}
