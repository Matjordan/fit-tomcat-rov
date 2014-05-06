#include "Tomcat_S_driver.h"
#include <string.h>
#include<stdio.h>

//MACROS
#define PORT 1
#define STBD 2
#define VERT 3
#define LAT 4
#define TILT 5
#define PAN 6
#define LIGHT PORTBbits.RB0
#define CLAW_OPEN PORTBbits.RB1
#define CLAW_CLOSE PORTBbits.RB2
#define WRIST_OPEN PORTBbits.RB3
#define WRIST_CLOSE PORTBbits.RB4



//variables
char n = 0;
char recv_buff = 0;
char recv_flag = 0;

//functions
void tx_chars(char tx_buff[],char num_byte);
unsigned int read_an(char); //love, suzyrhouser
unsigned int get_claw(void);
unsigned int get_wrist(void);

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
        
        buff[0] = 0xff;
        buff[1] = read_an(PORT); //Port Thruster Speed
        buff[2] = read_an(STBD); //Stbd Thruster Speed
        buff[3] = read_an(VERT); //Vert Thruster Speed
        buff[4] = read_an(LAT); //Lat Thruster Speed
        buff[5] = get_claw(); //Claw
        buff[6] = get_wrist(); //Wrist
        buff[7] = read_an(TILT); //Tilt
        buff[8] = read_an(PAN); //Pan
        buff[9] = LIGHT;


        tx_chars(buff,10);
        if (recv_flag == 1)
        {
            char *token;
            int depth,heading,pitch,roll,ex_temp,int_temp,int_press;
            token = strtok(recv_buff, ',');
            depth = atoi(token);
            token = strtok(recv_buff, ',');
            heading= atoi(token);
            token = strtok(recv_buff, ',');
            pitch= atoi(token);
            token = strtok(recv_buff, ',');
            roll= atoi(token);
            token = strtok(recv_buff, ',');
            ex_temp= atoi(token);
            token = strtok(recv_buff, ',');
            int_temp= atoi(token);
            token = strtok(recv_buff, ',');
            int_press= atoi(token);
            
            recv_flag = 0;
        }
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
    int claw;
    if (CLAW_OPEN == 1)
       claw = 187;
    else if (CLAW_CLOSE ==1)
        claw = 67;
    else
        claw =127;
    return(claw);

}

unsigned int get_wrist(void)
{
    int wrist;
    if (WRIST_OPEN == 1)
       wrist = 187;
    else if (WRIST_CLOSE ==1)
        wrist = 67;
    else
        wrist = 127;
    return(wrist);
}
