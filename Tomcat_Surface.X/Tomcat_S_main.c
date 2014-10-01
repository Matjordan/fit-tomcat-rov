#include "Tomcat_S_driver.h"
#include <string.h>
#include<stdio.h>
#include <stdlib.h>


//MACROS

#define TILT 5
#define PAN 6
#define PACK_LEN 9

//TODO change tx to human readable

//variables
char n = 0;
char recv_buff = 0;
char recv_flag = 0;

//functions
void tx_chars(char tx_buff[], char num_byte);
unsigned char read_an(char);
unsigned int get_claw(void);
unsigned int get_wrist(void);

//interupts

void interrupt isr(void) {
    GLOBAL_INT = 0;

    if (PIR1bits.RCIF) {
        recv_buff = RCREG;
        if (recv_buff == 0xff)recv_flag = 1;
    }

    GLOBAL_INT = 1;
}

void main(void) {
    char buff[60];
    int depth, heading, pitch, roll, ex_temp, int_temp, int_press;
    int temp1=0,temp2=0;
    char osd_buff[30];
    int output[10];
    unsigned int check = 0;
    int out_count = 0;


    TRISA = 0b11111111;
    TRISB = 0b00111000;
    TRISC = 0b10100111;
    TRISD = 0b00011111;
    TRISE = 0b00000011;
    SHUTOFF = 1;
    //setup
    INTCON2bits.RBPU=1;
    PIE1 = 0b00110000;
    ADCON1 = 0b00001010;
    ADCON2 = 0b00010111;
    ADCON0bits.ADON = 1;
    OSD_init();
    OpenUSART(USART_TX_INT_OFF &
            USART_RX_INT_ON &
            USART_ASYNCH_MODE &
            USART_EIGHT_BIT &
            USART_CONT_RX &
            USART_BRGH_HIGH, 131);



    while (1) {
        //READ ANALOG CHANNELS

        temp1 = read_an(THROTTLE)-133; //Port Thruster Speed
        temp2 = read_an(RUDDER)-89; //Stbd Thruster Speed
        output[1] = temp1+temp2+127;
        output[2] = temp1-temp2+127;
        output[3] = read_an(VERTICAL)-20; //Vert Thruster Speed
        output[4] = read_an(LATERAL)+10; //Lat Thruster Speed
        output[5] = get_claw(); //Claw
        output[6] = get_wrist(); //Wrist
        output[7] = read_an(TILT); //Tilt
        output[8] = read_an(PAN); //Pan
         output[9] = LIGHTS;



        //TRANSMIT TO VEHICLE
        check = 0;
        for (out_count = 1; out_count <= PACK_LEN; out_count++) {
            check = output[out_count] + check;
        }
        sprintf(buff, "$%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n", output[1], output[2], output[3], output[4], output[5],
                output[6], output[7], output[8], output[9], check);
        putsUSART(buff);

        //RECIEVE FROM VEHICLE

        if (recv_flag == 1) //if recieve data from vehicle..
        {
            char *token; //pulling information
            token = strtok(recv_buff, ',');
            depth = atoi(token);
            token = strtok(recv_buff, ',');
            heading = atoi(token);
            token = strtok(recv_buff, ',');
            pitch = atoi(token);
            token = strtok(recv_buff, ',');
            roll = atoi(token);
            token = strtok(recv_buff, ',');
            ex_temp = atoi(token);
            token = strtok(recv_buff, ',');
            int_temp = atoi(token);
            token = strtok(recv_buff, ',');
            int_press = atoi(token);

            recv_flag = 0; //reset vehicle communication
        }

        //UPDATE OSD

        //Temp
        sprintf(osd_buff, "Temp:%dC;%dC", int_temp, ex_temp);
        Str_output(1, 1, osd_buff, strlen(osd_buff));



        //Heading
        char osd_buff[30];

        if (heading < 100)
            sprintf(osd_buff, "Hdg: 0%d\0", heading);
        if (heading < 10)
            sprintf(osd_buff, "Hdg: 00%d\0", heading);
        else
            sprintf(osd_buff, "Hdg: %d\0", heading);
        Str_output(1, 21, osd_buff, strlen(osd_buff));



        //Depth
        char osd_buff[30];
        sprintf(osd_buff, "%d m", depth);
        Str_output(12, 12, osd_buff, strlen(osd_buff));


        //Lights
        if (LIGHTS == 1) {
            OSD_write(12, 1, 0xF5);
            OSD_write(12, 2, 0xF6);
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
        Str_output(7, 13, osd_buff, strlen(osd_buff));
        sprintf(osd_buff, "l");
        Str_output(6, 15, osd_buff, strlen(osd_buff));
        sprintf(osd_buff, "l");
        Str_output(8, 15, osd_buff, strlen(osd_buff));
    }//end while
}//end main

void tx_chars(char tx_buff[], char num_bytes) {
    char k = 0;
    for (k = 0; k < num_bytes; k++) {
        while (BusyUSART());
        WriteUSART(tx_buff[k]);
    }

}
//TODO change to int with ADRES not ADRESH need to find center values
unsigned char read_an(char channel) {
    unsigned char num = 0;
    _delay(1000);
    ADCON0bits.CHS = channel; //calling channel
    ADCON0bits.GO = 1;
    while (ADCON0bits.GO); //wait til done processing
    num = ADRESH; //value that you want
    return (num); //here ya go
}

unsigned int get_claw(void) {
    int claw;
    if (GRIP_OPEN == 1)
        claw = 187;
    else if (GRIP_CLOSE == 1)
        claw = 67;
    else
        claw = 127; //not moving
    return (claw);

}

unsigned int get_wrist(void) {
    int wrist;
    if (WRIST_CCW == 1)
        wrist = 187;
    else if (WRIST_CW == 1)
        wrist = 67;
    else
        wrist = 127; //not moving
    return (wrist);
}
