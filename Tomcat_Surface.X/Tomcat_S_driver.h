/* 
 * File:   Tomcat_Surface_Driver.h
 * Author: Mathew
 *
 * Created on April 24, 2014, 1:18 PM
 */

#ifndef TOMCAT_SURFACE_DRIVER_H
#define	TOMCAT_SURFACE_DRIVER_H

#include <stdlib.h>
#include <stdio.h>
#include <plib/pwm.h>
#include <plib/usart.h>
#include <plib/i2c.h>
#include <xc.h>
#include "OSD.h"

//config macros
#define _XTAL_FREQ  20000000 //20MHz
#define UART1_BAUD  9600

//macros
#define U1_SPRG     (_XTAL_FREQ/(16*UART1_BAUD))+1

#define TMR0_RESET()  TMR0=0
#define TMR1_RESET()  TMR1=0
#define TMR2_RESET()  TMR2=0
#define TMR3_RESET()  TMR3=0

#define RX1_INT     PIR1bits.RCIF
#define TX1_INT     PIR1bits.TXIF

#define TMR0_INT    INTCONbits.TMR0IF
#define TMR1_INT    PIR1bits.TMR1IF
#define TMR2_INT    PIR1bits.TMR2IF
#define TMR3_INT    PIR2bits.TMR3IF

#define GLOBAL_INT  INTCONbits.GIE

//error codes
#define LEAK    'L'
#define HEAT    'H'
#define PRESS   'P'
#define CURRENT 'C'
#define COMMS   'N'

//pins out
#define LED_COMMS   LATBbits.LATB0
#define ERROR       LATDbits.LATD5
#define SHUTOFF     LATEbits.LATE2

//pin in
#define LIGHTS      PORTBbits.RB3
#define GRIP_OPEN   PORTBbits.RB4
#define GRIP_CLOSE  PORTAbits.RA4
#define WRIST_CW    PORTCbits.RC2
#define WRIST_CCW   PORTDbits.RD0
#define HOVER       PORTCbits.RC5
#define POWER       PORTCbits.RC6
#define PAN_RIGHT   PORTCbits.RC0
#define PAN_LEFT    PORTCbits.RC1
#define TILT_UP     PORTDbits.RD2
#define TILT_DOWN   PORTDbits.RD1
//analog channels
#define THROTTLE    1
#define RUDDER      3
#define LATERAL     2
#define VERTICAL    0
#define CAM_PITCH   4
#define CAM_YAM     5

void Tomcat_S_Setup();


#endif	/* TOMCAT_SURFACE_DRIVER_H */

