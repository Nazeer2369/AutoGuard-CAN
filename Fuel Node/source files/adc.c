#include<lpc21xx.h>
#include"delay.h"
/*
ADDR
DONE 31 RESULT-6-15

ADCR
START 24-26
ENABLE-21
CHANNEL -0-7
CLKLDIV-8-15

*/
#define FOSC 12000000
#define CCLK FOSC*5
#define PCLK (CCLK/4)
#define ADC_CLK 3000000

//ADCR
#define START 24
#define ENABLE 21
#define CH_NO 1
#define CLKDIV 8
#define CLKDIV_VAL ((PCLK/ADC_CLK)-1)
//ADDR
#define DONE 31
#define RESULT 6

#define STEP_SIZE (3.3/1023)
void read_adc(int *eAval)
{
    float adc_digital;
    
    
    //START ADC
    ADCR|=(1<<START);
    //WAIT FOR Some time for conversion
    delay_ms(10);
    //read status of done bit
    while(((ADDR>>DONE)&1)==0);
    //read result bits
    adc_digital=((ADDR>>RESULT)&1023);
    //stop adc
    ADCR&=~(7<<START);
    adc_digital=adc_digital*STEP_SIZE;
    //convert to analog value
    *eAval=((adc_digital)*10);
    
}
void init_adc()
{
    ADCR&=~(255<<0);
    //PIN CONFIG
    PINSEL1|=0x01000000;
    //ENABLE
    ADCR|=(1<<ENABLE)|(1<<CH_NO);
    //LOAD CLKDIV
    ADCR|=CLKDIV_VAL<<CLKDIV;
}
