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
#include <plib/pwm.h>
#include <plib/usart.h>
#include <plib/i2c.h>
#include <xc.h>
#include <math.h>

//config macros
#define _XTAL_FREQ  20000000 //20MHz
#define UART1_BAUD  9600
#define UART2_BAUD  9600
#define I2C1_BAUD   100000     //100KHz
#define SENSOR_RATE 10       //TMR0 Rate Hz

//macros
#define U1_SPRG     (_XTAL_FREQ/(16*UART1_BAUD))+1
#define U2_SPRG     (_XTAL_FREQ/(16*UART1_BAUD))+1
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



//global vars
bit Leak1_flag=0;
bit Leak2_flag=0;
bit tmr0_flag=0;
bit tmr1_flag=0;
bit tmr2_flag=0;
bit tmr3_flag=0;
bit rx1_flag=0;
bit rx2_flag=0;
char time=0;
int accel[3];
int gyro[3];
int mag[3];
float temp = 0; //temporary

//low pass filter variable
#define k 2
long int freg_mag_x = -135;
long int freg_mag_y = 1055;
long int freg_mag_z = 10725;
long int freg_acc_x = -1295;
long int freg_acc_y = -315;
long int freg_acc_z = 5375;

//pin defs
#define THRUSTER1_DIR   LATCbits.LATC0
#define THRUSTER2_DIR   LATCbits.LATC1
#define THRUSTER3_DIR   LATCbits.LATC2
#define THRUSTER4_DIR   LATCbits.LATC3
#define GRIPPER_DIR     LATCbits.LATC4
#define GRIPPER_EN      LATCbits.LATC5
#define WRIST_DIR       LATCbits.LATC6
#define WRIST_EN        LATCbits.LATC7

#define LEAK_1          PORTBbits.RB0
#define LEAK_2          PORTBbits.RB1
#define LED_COMS        PORTBbits.RB2
#define LED_2           PORTBbits.RB3
#define LED_3           PORTBbits.RB4
#define LED_4           PORTBbits.RB5

//analog channels

#define PRESSURE_EXT    0
#define PRESSURE_INT    1
#define TEMP_EXT        2
#define TEMP_INT        3

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
void Tomcat_TX_data(char tx_buff[], char num_bytes);
void Tomcat_TX_warn(char code);
void Tomcat_TX_error(char code);
void Tomcat_Camera(int pan,int tilt);
unsigned int analogRead(char channel);

#endif	/* TOMCAT_DRIVER_H */

