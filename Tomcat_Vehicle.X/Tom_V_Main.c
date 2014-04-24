#include "Tomcat_driver.h"


void interrupt isr(void)
{
    GLOBAL_INT=0;
    if(TMR0_INT)
    {
        TMR0_INT=0;
        TMR0_RESET();
        tmr0_flag=1;
        time++;
    }
    if(TMR1_INT)
    {
        TMR1_INT=0;
        TMR1_RESET();
        tmr1_flag=1;
    }
    if(TMR2_INT)
    {
        TMR2_INT=0;
        TMR2_RESET();
        tmr2_flag=1;
    }
    if(RX1_INT)
    {
        RX1_INT=0;
        rx1_flag=1;
    }
    if(RX2_INT)
    {
        RX2_INT=0;
        rx2_flag=1;
    }
    if(LEAK1_INT)
    {
        LEAK1_INT=0;
        Leak1_flag=1;
        Tomcat_TX_warn(LEAK);
    }
    if(LEAK2_INT)
    {
        LEAK2_INT=0;
        Leak2_flag=1;
        Tomcat_TX_error(LEAK);
    }
    GLOBAL_INT=1;
}

void main(void)
{

    int a=0;
    char buff[30];
    int hdg = 0;
    Tomcat_Setup();
    while(1)
    {
      //do stuff here
        
    }
}

