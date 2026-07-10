#include<lpc21xx.h>
#include "lcd.h"
#include "i2c.h"
#include "delay.h"
#include"node_structures.h"
#include <math.h>

volatile char x,y,z;
main_node TXF;
void Init_I2C()
{
    // Configure P0.2 as SDA0 and P0.3 as SCL0
    // PINSEL0 bits:
    // P0.2 -> SDA0 (01)
    // P0.3 -> SCL0 (01)
    PINSEL0|=0x00000050;
    // Load SCL low-time count
    // Determines how long SCL remains LOW
    I2SCLL=LOAD_VAL;
    // Load SCL high-time count
    // Determines how long SCL remains HIGH
    I2SCLH=LOAD_VAL;
     // Enable I2C peripheral
    I2CONSET=1<<I2C_ENABLE;
    
}
void start_I2C()
{
    
    // Generate START condition on the I2C bus
    I2CONSET=1<<START;
    // Wait until START condition is transmitted
    // SI = 0 ? operation in progress
    // SI = 1 ? START condition generated successfully
    while(((I2CONSET>>SI)&1)==0);
    // Clear START bit after START condition is generated
    I2CONCLR=(1<<START);
    
}
void stop_I2C()
{
    // Clear SI (Serial Interrupt) flag
    // Allows the I2C controller to execute the STOP command
    I2CONCLR=1<<SI;
    // Generate STOP condition on the I2C bus
    // SDA transitions from LOW to HIGH while SCL is HIGH
    I2CONSET=1<<STOP;    
}
void restart_I2C()
{
    // Clear SI (Serial Interrupt) flag
    // Allows the I2C controller to proceed to the next state
    I2CONCLR=1<<SI;
    // Generate a Repeated START condition
    // Bus is not released; communication continues
    I2CONSET =1<<START;
    // Wait until Repeated START condition is transmitted
    // Hardware sets SI when operation is complete
    while(((I2CONSET>>SI)&1)==0);
    // Clear START bit after Repeated START is generated 
    I2CONCLR=(1<<START);
    
}
void write_I2C(char data)
{
    // Load data byte into I2C Data Register
    I2DAT=data;
    // Clear SI (Serial Interrupt) flag
    // This tells the I2C hardware to start transmitting
    I2CONCLR=1<<SI;
    // Wait until transmission is complete
    // Hardware sets SI when the byte has been transmitted
    // and ACK/NACK has been received from the slave
    while(((I2CONSET>>SI)&1)==0);
}
char read_nack()
{
    // Clear SI (Serial Interrupt) flag
    // Starts the next I2C operation
    I2CONCLR =1<<SI;
    // Clear AA (Assert Acknowledge) bit
    // AA = 0 ? Send NACK after receiving the byte
    // AA = 1 ? Send ACK after receiving the byte
    I2CONCLR =1<<AA;
    // Wait until a byte is received
    // SI will be set by hardware when reception is complete
    while(((I2CONSET>>SI)&1)==0);
    // Return the received data byte
    return I2DAT;
}
char read_mack()
{
    // Clear SI (Serial Interrupt) flag
    // This allows the I2C controller to continue the next operation
    I2CONCLR =1<<SI;
    // Set AA (Assert Acknowledge) bit
    // After receiving a byte, an ACK will be sent to the slave
    // indicating that more data bytes are expected
    I2CONSET =1<<AA;
    // Wait until a byte is received and SI is set again 
    while(((I2CONSET>>SI)&1)==0);
    // Return the received data byte    
    return I2DAT;  
}

void MMA_WRITE(char wbuff,char data)
{
    // Generate START condition on I2C bus
    start_I2C();
    // Send MMA7660 slave address with Write bit (0)
    write_I2C((MMA_ADDR<<1)|0);
    // Send register address where data will be written
	write_I2C(wbuff);
    // Send data to be stored in the register
    write_I2C(data);
    // Generate STOP condition and release I2C bus    
    stop_I2C();
}
char MMA_READ(char rbuff)
{
    char data;
    // Generate START condition on I2C bus
    start_I2C();
    // Send MMA7660 slave address with Write bit (0)
    write_I2C((MMA_ADDR<<1)|0);
    // Send register address to be read
	write_I2C(rbuff);
    // Generate REPEATED START condition
    restart_I2C();
    // Send MMA7660 slave address with Read bit (1)
    write_I2C((MMA_ADDR<<1)|1);
    // Read one byte from the register
    // NACK indicates this is the last byte to be read
    data=read_nack();
     // Generate STOP condition to release the I2C bus
    stop_I2C();
    // Return received data
    return data;
      
} 
void accelerometer_init()
{
    // Put MMA7660 into Standby Mode
    // Register 0x07 = MODE Register
    // Value 0x00 disables active measurements
  	MMA_WRITE(0x07,0x00);
    // Wait for mode change to take effect
	delay_ms(10);
    // Put MMA7660 into Active Mode
    // Value 0x01 enables continuous acceleration measurements
    MMA_WRITE(0x07,0x01);
    // Wait for sensor to stabilize
	delay_ms(10);


}
char convert_6_bit(char val)
{
    // Keep only the lower 6 bits
    val&=0x3F;
    // Check sign bit (bit 5)
    if(val&0x20)
    {   
        // Extend the sign to bits 6 and 7
        val|=0xc0;
    }
    return (char)val;
}
void read_accelerometer()
{   
    // Read X, Y, and Z axis acceleration values from MMA7660
    x=convert_6_bit(MMA_READ(X_DATA_REG));
    y=convert_6_bit(MMA_READ(Y_DATA_REG));
    z=convert_6_bit(MMA_READ(Z_DATA_REG));
    /*----------------------------------------------------
        Crash Condition 1
        Vehicle tilted heavily towards right side
    ----------------------------------------------------*/
	// right             		//left						  //normal			 //top
	if((x>15 && y>0 &&z>240) || (x>4&&y>240&& z>240)|| (x>235&& y>240 && z>235)||(x>2&&y>10&& z>240))
	 {
         // Display airbag deployment message
		cmd_lcd(0x01);
		cmd_lcd(0x80);
		display_string("AIR BAGS DEPLOYED");
        // Display X, Y, Z values for debugging
        cmd_lcd(0xc0);
		//display_integer(x);
	//display_char(' ');
	//	display_integer(y);
	//	display_char(' ');
	//	display_integer(z);
	//	display_char(' ');
        // Continuously send crash alert through CAN
		while(1)
		{
            // D = Danger
            TXF.data1='D';       
        	tx_main_node(TXF);

		}
	}
     
	
	
}

