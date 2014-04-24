#include "Tomcat_V_driver.h"



//tomcat vehicle related stuff

void Tomcat_Setup()
{
    //analog config
    ANSELC = 0;
    PIE1 = 0b00100000;
    //adcon values for pic18f46k22
    ADCON0 = 0b00000001;
    ADCON1 = 0b00000000;
    ADCON2 = 0b00100011;
    INTCON = 0b11100000;
    //Setup RC0 as output and set to low
    T0CON = 0b10000010;
    CloseI2C1();
    OpenI2C1(MASTER,SLEW_OFF);
    SSP1ADD=49;
    Open1USART(USART_TX_INT_OFF &
            USART_RX_INT_ON &
            USART_ASYNCH_MODE &
            USART_EIGHT_BIT &
            USART_CONT_RX &
            USART_BRGH_HIGH, U1_SPRG);
    openLSM9(0x1D,0x6B);
    OpenEPWM1(255,ECCP_1_SEL_TMR34);
    OpenEPWM2(255,ECCP_2_SEL_TMR34);
    OpenEPWM3(255,ECCP_3_SEL_TMR34);
    OpenPWM4(255,CCP_4_SEL_TMR34);

}

void Thruster_Driver(int input, char thruster)
{
    char dir=0;
    int dc=0;
    if(input==127)
        dc=0;
    if(input>127)
        dir=1;
        dc=1024*((input-127)/126);
    if(input<127)
        dir=0;
        dc=1024*((126-input)/126);

    switch (thruster)
    {
        case 1:
            THRUSTER1_DIR=dir;
            SetDCEPWM1(dc);
        case 2:
            THRUSTER2_DIR=dir;
            SetDCEPWM2(dc);
        case 3:
            THRUSTER3_DIR=dir;
            SetDCEPWM3(dc);
        case 4:
            THRUSTER4_DIR=dir;
            SetDCPWM4(dc);
    }
}
int Tomcat_Heading()
{
    float hdg=0.0;
    readLSM9_mag(mag);
}
int Tomcat_Pitch()
{
    float pitch=0.0;
    readLSM9_accel(accel);
}
int Tomcat_Roll()
{
    float roll=0.0;
    readLSM9_accel(accel);
}
int Tomcat_Depth()
{
    float tempPress;
    int depth;
    tempPress = analogRead(PRESSURE_EXT);
    tempPress = tempPress-(1024.0/5.0);//subtract reference 1 volt
    tempPress = tempPress*500.0/820.0;//convert volts to psi
    depth = (int)(tempPress*14.5/.0);//convert pressure to depth
    return depth;//returns depth in feet
}
int Tomcat_Press_Int()
{
    int press=0;
    press=(analogRead(PRESSURE_INT)-41)/922*101.5;
    return press;
}
int Tomcat_Temp()
{
    int adresult=0;
}
int Tomcat_Temp_Ex()
{
    int adresult=0;
}
void Tomcat_TX_data(char tx_buff[], char num_bytes)
{
    char k=0;
    while(Busy1USART());
    Write1USART('$');

    for (k = 0; k < num_bytes; k++)
    {
            while(Busy1USART());
            Write1USART(tx_buff[k]);
    }
    Write1USART('\n');
}
void Tomcat_TX_warn(char code)
{
    Write1USART('$');
    Write1USART('?');
    Write1USART(code);
}
void Tomcat_TX_error(char code)
{
    Write1USART('$');
    Write1USART('!');
    Write1USART(code);
}
void Tomcat_Camera(int pan,int tilt)
{

}
//analog read function for vehicle
unsigned int analogRead(char channel)
{
    unsigned int num = 0;
    ADCON0bits.CHS = channel; //calling channel
    ADCON0bits.GO=1;
    while(ADCON0bits.GO==1); //wait til done processing
    num = ADRES; //10bit value
    return(num);
}