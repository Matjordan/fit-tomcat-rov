#include "Tomcat_S_driver.h"


char n = 0;
char recv_buff = 0;
char recv_flag = 0;

//functions
void tx_chars(char tx_buff[],char num_byte);
//interupts

void interrupt isr(void) {
    GLOBAL_INT=0;

    if (PIR1bits.RCIF) {
        recv_buff = RCREG;
        if (recv_buff == 0xff)recv_flag = 1;
    }

    GLOBAL_INT=1;
}

void main(void) {
    char buff[9];
    
    //setup
    PIE1 = 0b00110000;
    ADCON1 = 0b00001111;

    OpenUSART(USART_TX_INT_OFF &
            USART_RX_INT_ON &
            USART_ASYNCH_MODE &
            USART_EIGHT_BIT &
            USART_CONT_RX &
            USART_BRGH_HIGH, 131);



    while (1) {
        //                if (recv_flag) {
        buff[0] = 0xff;
        buff[1] = PORTBbits.RB0;
        buff[2] = PORTBbits.RB1;
        buff[3] = PORTBbits.RB2;
        buff[4] = PORTBbits.RB3;
        buff[5] = PORTBbits.RB4;
        buff[6] = PORTBbits.RB5;
        buff[7] = PORTDbits.RD6;
        buff[8] = PORTDbits.RD7;


        tx_chars(buff,9);

        //      }//end if
    }//end while
}//end main

void tx_chars(char tx_buff[], char num_bytes)
{
    char k=0;
    for (k = 0; k < num_bytes; k++)
    {
            while(BusyUSART());
            WriteUSART(tx_buff[k]);
    }

}