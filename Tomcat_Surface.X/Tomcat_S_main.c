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
 //READ ANALOG CHANNELS       
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

//TRANSMIT TO VEHICLE
        tx_chars(buff,10);

//RECIEVE FROM VEHICLE
int depth,heading,pitch,roll,ex_temp,int_temp,int_press;   
        if (recv_flag == 1) //if recieve data from vehicle..
        {
            char *token; //pulling information
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
            
            recv_flag = 0; //reset vehicle communication
        }

//UPDATE OSD
        
  //  int leak,overheat;
        OSD_init();

        while(1)
        {
        //OSD_disp(1);

        //Temp
           char osd_buff[30];       
           sprintf(osd_buff, "Temp:%dC;%dC" , int_temp, ex_temp);
           Str_output(1,1,osd_buff,strlen(osd_buff));



        //Heading
            char osd_buff[30];

            if(heading<100)
                sprintf(osd_buff, "Hdg: 0%d\0" , heading);
            if(heading<10)
                sprintf(osd_buff, "Hdg: 00%d\0" , heading);
            else
                sprintf(osd_buff, "Hdg: %d\0" , heading);
            Str_output(1,21,osd_buff,strlen(osd_buff));



        //Depth
            char osd_buff[30];
            sprintf(osd_buff, "%d m" , depth);
            Str_output(12,12,osd_buff,strlen(osd_buff));


        //Lights
            if (LIGHT ==1)
            {OSD_write(12,1,0xF5);
            OSD_write(12,2,0xF6);
            }

        //Time
  //          char osd_buff[30];
  //          int timehr = 14;
  //          int timemin = 04;
  //          if(timemin<10)
  //              sprintf(osd_buff, "%d:0%d" , timehr, timemin);
  //          else
 //               sprintf(osd_buff, "%d:%d" , timehr, timemin);
 //           Str_output(12,22,osd_buff,strlen(osd_buff));



        //Leak
 //           if(leak==1)
 //               {
 //                char osd_buff[30];
 //               sprintf(osd_buff, "LEAK");
 //               Str_blink(2,1,osd_buff,strlen(osd_buff));
 //               }

        //Overheated
//            if(overheat==1)
//                {
//                char osd_buff[30];
//                sprintf(osd_buff, "OVERHEATING");
//                Str_blink(2,18,osd_buff,strlen(osd_buff));
//                }
     //crosseyed
                char osd_buff[30];
                sprintf(osd_buff, "-- --");
                Str_output(7,13,osd_buff,strlen(osd_buff));
                sprintf(osd_buff, "l");
                Str_output(6,15,osd_buff,strlen(osd_buff));
                sprintf(osd_buff, "l");
                Str_output(8,15,osd_buff,strlen(osd_buff));




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
        claw =127; //not moving
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
        wrist = 127; //not moving
    return(wrist);
}
