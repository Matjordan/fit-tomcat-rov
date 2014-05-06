#include "Tomcat_V_driver.h"


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
        rx1_buff[rx1_count] = RCREG;
        if (rx1_count == SUR_PACK_LEN)rx1_flag = 1;
        if (rx1_buff[rx1_count++] == 0xff)rx1_count = 0;
    }
    if(RX2_INT)
    {
        RX2_INT=0;
        rx2_flag=1;
    }
    if(LEAK1_INT)
    {
        LEAK1_INT=0;
        leak1_flag=1;
        Tomcat_TX_warn(LEAK);
    }
    if(LEAK2_INT)
    {
        LEAK2_INT=0;
        leak2_flag=1;
        Tomcat_TX_error(LEAK);
    }
    GLOBAL_INT=1;
}

void main(void)
{

    int heading,pitch,roll;
    int depth,int_press,ex_temp,int_temp;
    int main_current=0,v_current=0;
    char surf_buff[60];
    Tomcat_Setup();
    while(1)
    {
      //regularly scheduled stuff
        if(!(time%10))
        {
            //every 10 time ticks
            time=0;//reset time
            //read environment
            depth=Tomcat_Depth();
            int_press=Tomcat_Press_Int();
            int_temp=Tomcat_Temp();
            ex_temp=Tomcat_Temp_Ex();

        }
        if((!time%5))
        {
            //every 5 time ticks
            //send to surface
            sprintf(surf_buff,"%d,%d,%d,%d,%d,%d,%d,%d,%d",
                    depth,heading,pitch,roll,ex_temp,int_temp,int_press,main_current,v_current);
            Tomcat_TX_data(surf_buff,strlen(surf_buff)-1);
        }
        if((!time%2))
        {
            //every 2 time ticks
            //read currents
            main_current=analogRead(MAIN_CURRENT)*MAIN_FACTOR;
            v_current=analogRead(V_CURRENT)*V_FACTOR;
        }
        if(tmr0_flag)
        {
            //every 1 time tick
            tmr0_flag=0;
            //read IMU
            heading=Tomcat_Heading();
            pitch=Tomcat_Pitch();
            roll=Tomcat_Roll();
        }


        if (rx1_flag)
        {
            //rx from surface
            //packet
            //0xff,port duty,stbd duty,vert duty,lat duty,grip duty,
            //wrist duty, pan duty,tilt duty,lights, check sum
            LED_COMS=1;
            rx1_count=0;
            Thruster_Driver(rx1_buff[1],1); //port
            Thruster_Driver(rx1_buff[2],2); //stbd
            Thruster_Driver(rx1_buff[3],3); //vert
            Thruster_Driver(rx1_buff[4],4); //lat
            Tomcat_Claw(rx1_buff[5],rx1_buff[6]);
            Tomcat_Camera(rx1_buff[7],rx1_buff[8]);

            LIGHTS=rx1_buff[9];
            rx1_flag=0;
            LED_COMS=0;

        }
        if (rx2_flag)
        {
            //rx from pan tilt
            rx2_flag=0;

        }
        if(leak1_flag)
        {

        }
        if(leak2_flag)
        {

        }
    }
}

