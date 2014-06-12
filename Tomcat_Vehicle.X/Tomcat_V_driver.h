/* 
 * File:   Tomcat_driver.h
 * Author: Mathew
 *
 * Created on April 10, 2014, 6:33 PM
 */

#ifndef TOMCAT_DRIVER_H
#define	TOMCAT_DRIVER_H
#include "LSM9DS0.h"
#include "I2C_mat.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <plib/pwm.h>
#include <plib/usart.h>
#include <plib/i2c.h>
#include <xc.h>
#include <math.h>

//config macros
#define _XTAL_FREQ      20000000//20MHz
#define UART1_BAUD      9600
#define UART2_BAUD      9600
#define I2C1_BAUD       100000  //100KHz
#define SENSOR_RATE     10      //TMR0 Rate Hz
#define SUR_PACK_LEN    9      //

#define MAIN_FACTOR     5       //current conversion factor
#define V_FACTOR        1       //current conversion factor
#define WARN_INT_TEMP   65      //internal temp warning
#define ALARM_INT_TEMP  75      //internal temp alarm
#define WARN_INT_PRESS  20      //internal press warn
#define ALARM_INT_PRESS 20      //internal press alarm
#define WARN_V_CURRENT  23      //24v current warn
#define ALARM_V_CURRENT 27      //24v current alarm
#define COMMS_TIMEOUT   200    //comms timeout in timer1 cycles 200=5 sec


//macros
#define U1_SPRG     (unsigned char)(((float)_XTAL_FREQ/(16.0*(float)UART1_BAUD))+1)
#define U2_SPRG     (unsigned char)(((float)_XTAL_FREQ/(16.0*(float)UART2_BAUD))+1)
#define I2C1_ADD    (_XTAL_FREQ/(4*I2C1_BAUD))-1

#define TMR0_RESET()  TMR0=0
#define TMR1_RESET()  TMR1=0
#define TMR2_RESET()  TMR2=0
#define TMR3_RESET()  TMR3=0

#define RX1_INT     PIR1bits.RC1IF
#define TX1_INT     PIR1bits.TX1IF
#define RX2_INT     PIR3bits.RC2IF
#define TX2_INT     PIR3bits.TX2IF

#define TMR0_INT    INTCONbits.TMR0IF
#define TMR1_INT    PIR1bits.TMR1IF
#define TMR2_INT    PIR1bits.TMR2IF
#define TMR3_INT    PIR2bits.TMR3IF
#define TMR4_INT    PIR5bits.TMR4IF
#define TMR5_INT    PIR5bits.TMR5IF

#define LEAK1_INT   INTCONbits.INT0IF
#define LEAK2_INT   INTCON3bits.INT1IF

#define GLOBAL_INT  INTCONbits.GIE

//error codes
#define LEAK    'L'
#define HEAT    'H'
#define PRESS   'P'
#define CURRENT 'C'
#define COMMS   'N'
#define TEMP    'T'



//global vars
bit leak1_flag=0;
bit leak2_flag=0;
bit tmr0_flag=0;
bit tmr1_flag=0;
bit tmr2_flag=0;
bit tmr3_flag=0;
bit rx1_flag=0;
bit rx2_flag=0;
char time=1;
int accel[3];
int gyro[3];
int mag[3];
char rx1_buff[25];
char rx1_count=0;
unsigned int comms_time=0; //time for comms timeout

//low pass filter variable
#define FILT_K 2
long int freg_mag_x = -135;
long int freg_mag_y = 1055;
long int freg_mag_z = 10725;
long int freg_acc_x = -1295;
long int freg_acc_y = -315;
long int freg_acc_z = 5375;

//pin defs
#define THRUSTER1_DIR   LATCbits.LATC1
#define THRUSTER2_DIR   LATBbits.LATB4
#define THRUSTER3_DIR   LATBbits.LATB6
#define THRUSTER4_DIR   LATDbits.LATD0
#define GRIPPER_DIR     LATEbits.LATE1
#define GRIPPER_EN      LATEbits.LATE2
#define WRIST_DIR       LATBbits.LATB7
#define WRIST_EN        LATCbits.LATC0

#define LEAK_1          PORTBbits.RB1

#define LIGHTS          LATDbits.LATD3
#define LED_COMS        LATAbits.LATA4
//#define EX_TEMP_PIN     PORTABITS.RA4


//analog channels

#define PRESSURE_EXT    3
#define PRESSURE_INT    2
#define TEMP_INT        8
#define TEMP_EXT        12
#define CAM_PAN         5
#define CAM_TILT        6
#define V_CURRENT       0
#define MAIN_CURRENT    1


//functions

void Thruster_Driver(int input, char thruster);
void Tomcat_Setup();
int Tomcat_Heading();
int Tomcat_Pitch();
int Tomcat_Roll();
int Tomcat_Depth();
int Tomcat_Press_Int();
int Tomcat_Temp();
int Tomcat_Temp_Ex();
void Tomcat_Claw(int grip,int wrist);
void Tomcat_TX_data(char tx_buff[], char num_bytes);
void Tomcat_TX_warn(char code);
void Tomcat_TX_error(char code);
void Tomcat_Camera(int pan,int tilt);
unsigned int analogRead(char channel);

#endif	/* TOMCAT_DRIVER_H */

