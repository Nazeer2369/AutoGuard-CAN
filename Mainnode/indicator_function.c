#include "lcd.h"
#include "delay.h"
void arrow(char data)
{
 switch(data)
 {
  case 'L':
      
             // Clear previous Right arrow display area
            cmd_lcd(0xA5);
			display_string("   ");
            // Animate left indicator
            cmd_lcd(0x97);
  			display_string("<");
			delay_ms(200);
			cmd_lcd(0x96);
  			display_char('<');
			delay_ms(200);			
			cmd_lcd(0x95);
  			display_char('<');
			delay_ms(200);
            // Clear arrow after animation
			cmd_lcd(0x95);
			display_string("   ");
			break;
  case 'R':
            // Clear previous left arrow display area
			cmd_lcd(0x94);
			display_string("   ");
            // Animate right indicator
  			cmd_lcd(0xA3);
  			display_string(">");
			delay_ms(200);
			cmd_lcd(0xA4);
  			display_char ('>');
			delay_ms(200);
			cmd_lcd(0xA5);
  			display_char('>');
			delay_ms(200); 
            // Clear arrow after animation
			cmd_lcd(0xA3);
			display_string("   ");
  			break;
 }

}
