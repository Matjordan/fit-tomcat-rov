#include "Tomcat_S_driver.h"

#define PORT 1
#define STBD 2
#define VERT 3
#define LAT 4
//#define CLAW 5
//#define WRIST 6
#define TILT 5
#define PAN 6

//variables
char n = 0;
char recv_buff = 0;
char recv_flag = 0;

//functions
void tx_chars(char tx_buff[],char num_byte);
unsigned int read_an(char); //love, suzyrhouser
unsigned int get_claw(void);
unsigned int get_claw(void);

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
        buff[1] = read_an(PORT); //Port Thruster Speed
        buff[2] = read_an(STBD); //Stbd Thruster Speed
        buff[3] = read_an(VERT); //Vert Thruster Speed
        buff[4] = read_an(LAT); //Lat Thruster Speed
 //       buff[5] = read_an(CLAW); //Claw
 //       buff[6] = read_an(WRIST); //Wrist
        buff[5] = read_an(TILT); //Tilt
        buff[6] = read_an(PAN); //Pan


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

unsigned int read_an(char channel)
{
    unsigned int num = 0;
    ADCON0bits.CHS = channel; //calling channel
    ADCON0bits.GO=1;
    while(ADCON0bits.GO); //wait til done processing
    num = ADRES; //value that you want
    return(num); //here ya go
}

unsigned int get_claw(void)
{
    if ("claw_open_pin" = 1)
       claw_open = 187;
    if ("claw_close_pin" =1)
        claw_close = 67;
}

unsigned int get_wrist(void)
{
    if ("wrist_open_pin" = 1)
       wrist_open = 187;
    if ("wrist_close_pin" =1)
        wrist_close = 67;
}