#include "OSD.h"
#include "Tomcat_S_driver.h"


void Tomcat_S_Setup()
{

    ADCON0=0b00000001;
    ADCON1=0b00001011;
    ADCON2=0b10100101;

    T0CON= 0b10000111;

    INTCON=0b11100000;
    PIE1 = 0b01100000;
    OSD_init();
    OSD_disp(1);
    OpenUSART(USART_TX_INT_OFF &
          USART_RX_INT_ON &
          USART_ASYNCH_MODE &
          USART_EIGHT_BIT &
          USART_CONT_RX &
          USART_BRGH_HIGH, U1_SPRG);
}