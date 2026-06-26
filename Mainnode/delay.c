void delay_us(unsigned int i)
{
    for(i*=12;i>0;i--);
}
void delay_ms(unsigned int i)
{
    for(i*=12000;i>0;i--);
}
void delay_s(unsigned int i)
{
    for(i*=12000000;i>0;i--);
}
