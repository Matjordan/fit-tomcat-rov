

#include <stdlib.h>
#include <xc.h>
#include <stdio.h>
#include <plib/usart.h>
#include <plib/pwm.h>

//macros
#define INTOFF INTCONbits.GIE=0
#define INTON INTCONbits.GIE=1
#define _XTAL_FREQ 20000000
#define INTON INTCONbits.GIE=1
#define _XTAL_FREQ 20000000
#define INTOFF INTCONbits.GIE=0
#define INTON INTCONbits.GIE=1
#define TMR0INT INTCONbits.TMR0IF
#define TMR0RESET TMR0=15550
#define TMR1INT PIR1bits.TMR1IF
#define RXINT PIR1bits.RCIF
#define ADINT PIR1bits.ADIF

//PAN MACRO
#define PAN_DIR LATCbits.LATC0

//TILT MACRO
#define TILT_DIR LATCbits.LATC3

//globals
unsigned int time=0;
unsigned int adresult=0;
char n=0;
char recv_buff[30];
char recv_flag=0;

//functions
unsigned int read_an(char);

//interupts

void interrupt isr(void)
{
    INTOFF;
    if(TMR0INT)
    {
        TMR0INT=0;
        TMR0RESET;
        time++;
    }

  if(RXINT)
      ////// Recieve the command.
    {
        RXINT=0;
        recv_buff[n] = RCREG;
        if (recv_buff[n++] == 0xff)n = 1;
        if (n == 2)recv_flag = 1;
    }


    INTON;
}

    ////////////////////////////////////////////////////////////////////////////

void main(void)
{
    int adresult=0;
    char buff[20];
    int pan_position=748, tilt_position=368;
    int pan=0,tilt=0;

    //setup
    TRISC=0b11110000;

//RCO output

//ADC setup
    ADCON1=0b00001100;
    ADCON2=0b10110011;
    ADCON0=0b00000011;
    T0CON= 0b10000111;
    INTCON=0b11100000;
    PIE1 = 0b00100000;

    OpenUSART(USART_TX_INT_OFF &
          USART_RX_INT_ON &
          USART_ASYNCH_MODE &
          USART_EIGHT_BIT &
          USART_CONT_RX &
          USART_BRGH_HIGH, 131);

    OpenPWM1(200); //pan pin17
    OpenPWM2(200); //tilt pin16
    SetDCPWM1(0);
    SetDCPWM2(0);

    //LATCbits.LATC0=0;
    //SetDCPWM1(255);

    while (1)
    {
    /////////////////////////////////////////////////////////
        //Getting feedback from potentiometer PAN
        pan=read_an(0); //tells where the camera is presently
        //_delay(1000);
        tilt=read_an(1);
        //_delay(1000);
        // START USART TO RECIEVE COMMANDS
        sprintf(buff,"p:%d t:%d",pan,tilt);

        putrsUSART(buff);
        if (recv_flag)
        {
            pan_position=recv_buff[1]+610;  // pan position that we get from HTERM from 610 to 820

            tilt_position=recv_buff[2]+320; //tilt position "  "
            recv_flag=0;  //no more information from HTERM
        }

        //_______________________________________________________//

        //PAN IF STATEMENT
        if (pan>pan_position)
        {
            SetDCPWM1((pan-pan_position)*(425/210)+60);
            PAN_DIR=1; //------- This changes direction. Might wanna make new macro for Tilt

        }
        else if (pan<pan_position)
        {
            SetDCPWM1((pan_position-pan)*(425/210)+60);
            PAN_DIR=0;
        }
        else
        {
            SetDCPWM1(0);
        }

        //TILT IF STATEMENT
        if (tilt>tilt_position)
        {   SetDCPWM2((tilt-tilt_position)*(425/210)+60);
            TILT_DIR=1; //------- This changes direction.
        }
        else if (tilt<tilt_position)
        {   SetDCPWM2((tilt_position-tilt)*(425/210)+60);
            TILT_DIR=0;
        }
        else
            SetDCPWM2(0); //don't move

    }

}
unsigned int read_an(char channel)
{
    unsigned int num = 0;
    ADCON0bits.CHS = channel; //calling channel
    ADCON0bits.GO=1;
    while(ADCON0bits.GO); //wait til done processing
    num = ADRES; //value that you want
    return(num); //here ya go
}
