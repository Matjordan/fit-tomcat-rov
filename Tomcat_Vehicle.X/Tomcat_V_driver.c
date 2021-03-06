#include "Tomcat_V_driver.h"



//tomcat vehicle related stuff

void Tomcat_Setup() {
    //analog config
    //adcon values for pic18f46k22
    ADCON0 = 0b00000001;
    ADCON1 = 0b00000000;
    ADCON2 = 0b10100011;
    
    //interrupts
    
    INTCON = 0b11110000; //GIE PIEE TMR0 INT0
    PIE1   = 0b00100001; //rx1 tmr1
    //port setup
    TRISA  = 0b11101111;
    ANSELA = 0b00101111;
    PORTA  = 0b00000000;

        CLRWDT();
    TRISB  = 0b00000110;
    ANSELB = 0b00000100;
    PORTB  = 0b00000000;

    TRISC  = 0b10010000;
    ANSELC = 0b00000000;
    PORTC  = 0b00000000;

    TRISD  = 0b10110100;
    ANSELD = 0b00000000;
    PORTD  = 0b00000000;

    TRISE  = 0b11111000;
    ANSELE = 0b00000000;
    PORTE  = 0b00000000;

    //timers
    T0CON  = 0b10000010; //for periodic functions
    T1CON  = 0b00010011; //for comms timeout 1:2 prescale

    T2CON  = 0b00000110;
    T4CON  = 0b00000110;

    //open PWM
    CCP1CON = 0b00001100;
    CCP2CON = 0b00001100;
    CCP3CON = 0b00001100;
    CCP4CON = 0b00001111;
    
    ECCP1AS = 0;
    ECCP2AS = 0;
    ECCP3AS = 0;
    //4 is not enanched
    
    PSTR1CON = 0b00000001;
    PSTR2CON = 0b00000001;
    PSTR3CON = 0b00000001;
        CLRWDT();

    CCPTMRS0 = 0b00000001;
    CCPTMRS1 = 0b00001001;

    CloseI2C1();
    OpenI2C1(MASTER, SLEW_OFF);
    SSP1ADD = 49;
    Open1USART(USART_TX_INT_OFF &
            USART_RX_INT_ON &
            USART_ASYNCH_MODE &
            USART_EIGHT_BIT &
            USART_CONT_RX &
            USART_BRGH_HIGH, U1_SPRG);
        CLRWDT();
//    Open2USART(USART_TX_INT_OFF &
//            USART_RX_INT_ON &
//            USART_ASYNCH_MODE &
//            USART_EIGHT_BIT &
//            USART_CONT_RX &
//            USART_BRGH_HIGH, U2_SPRG);

    //openLSM9(0x1D, 0x6B);
    //OpenEPWM1(255, ECCP_1_SEL_TMR34);
    //OpenEPWM2(255, ECCP_2_SEL_TMR34);
    //OpenEPWM3(255, ECCP_3_SEL_TMR34);
    //OpenPWM4(255, CCP_4_SEL_TMR34);

}

void Thruster_Driver(int input, char thruster) {
    char dir = 0;
    int dc = 0;
    if (input > 134)
    {
        dir = 1;
        dc = 256 * ((input - 127) / 126.0);
    }
    else if (input < 120)
    {
        dir = 0;
        dc = 256 * ((126 - input) / 126.0);
    }
    else
    {
        dc=0;
    }

    switch (thruster) {
        case 1:
            THRUSTER1_DIR = dir;
            SetDCEPWM1(dc);
            //LATCbits.LATC2=1;
            break;
        case 2:
            THRUSTER2_DIR = dir;
            SetDCEPWM2(dc);
            break;
        case 3:
            THRUSTER3_DIR = dir;
            SetDCEPWM3(dc);
            break;
        case 4:
            THRUSTER4_DIR = dir;
            SetDCPWM4(dc);
            break;
    }
}

int Tomcat_Heading() {
    float hdg = 0.0;
    float temp = 0; //temporary
    readLSM9_mag(mag);

    //Software Low Pass Filter
    freg_mag_x = freg_mag_x - (freg_mag_x >> FILT_K) + mag[0];
    freg_mag_y = freg_mag_y - (freg_mag_y >> FILT_K) + mag[1];
    freg_mag_z = freg_mag_z - (freg_mag_z >> FILT_K) + mag[2];

    mag[0] = freg_mag_x >> FILT_K;
    mag[1] = freg_mag_y >> FILT_K;
    mag[2] = freg_mag_z >> FILT_K;

    //setup for pos x axis pointing north
    //        y north commented out
    if (mag[0] > 0 && mag[1] > 0) {
        temp = (float) mag[0] / (float) mag[1];
        //            hdg = 360.0 - 180.0 * atan(temp) / M_PI;
        hdg = 90 - 180.0 * atan(temp) / M_PI;

    } else if (mag[0] < 0 && mag[1] > 0) {
        temp = (float) mag[1] / (float) mag[0];
        //            hdg = 90.0 + 180.0 * atan(temp) / M_PI;
        hdg = 180 + 180.0 * atan(temp) / M_PI;


    } else {
        temp = (float) mag[0] / (float) mag[1];
        //            hdg = 180.0 - 180.0 * atan(temp) / M_PI;
        hdg = 270 - 180.0 * atan(temp) / M_PI;
    }
    return hdg;
}

int Tomcat_Pitch() {
    float pitch = 0.0;
    float temp = 0; //temporary
    readLSM9_accel(accel);

    //low pass filter
    freg_acc_x = freg_acc_x - (freg_acc_x >> FILT_K) + accel[0];
    freg_acc_y = freg_acc_y - (freg_acc_y >> FILT_K) + accel[1];
    freg_acc_z = freg_acc_z - (freg_acc_z >> FILT_K) + accel[2];

    accel[0] = freg_acc_x >> FILT_K;
    accel[1] = freg_acc_y >> FILT_K;
    accel[2] = freg_acc_z >> FILT_K;

    //setup for x axis controlling pitch
    temp = (float) accel[0] / (float) accel[2];
    pitch = 3 + 180.0 * atan(temp) / M_PI;

    return pitch;
}

int Tomcat_Roll() {
    float roll = 0.0;
    float temp = 0; //temporary
    readLSM9_accel(accel);

    //low pass filter
    freg_acc_x = freg_acc_x - (freg_acc_x >> FILT_K) + accel[0];
    freg_acc_y = freg_acc_y - (freg_acc_y >> FILT_K) + accel[1];
    freg_acc_z = freg_acc_z - (freg_acc_z >> FILT_K) + accel[2];

    accel[0] = freg_acc_x >> FILT_K;
    accel[1] = freg_acc_y >> FILT_K;
    accel[2] = freg_acc_z >> FILT_K;

    //setup for y axis controlling roll
    temp = (float) accel[1] / (float) accel[2];
    roll = 13 + 180.0 * atan(temp) / M_PI;

    return roll;
}

int Tomcat_Depth() {
    float tempPress;
    int depth;
    tempPress = analogRead(PRESSURE_EXT);
    tempPress = tempPress - (1024.0 / 5.0); //subtract reference 1 volt
    tempPress = tempPress * 500.0 / 820.0; //convert volts to psi
    depth = (int) (tempPress * 2.31); //convert pressure to depth
    return depth; //returns depth in feet

}

int Tomcat_Press_Int() {
    int press = 0;
    press = (analogRead(PRESSURE_INT) - 41) / 922 * 101.5;
    return press;
}

int Tomcat_Temp() {
    float temp = 0.0;
    temp = analogRead(TEMP_INT);
    temp = (temp / 1024.0) * 500.0;//convert from V to degrees C, assumed linear, 10mV per degree
    return (int)temp;
}

int Tomcat_Temp_Ex() {
    //TODO bench test for calibration
    //(10% tolerance on sensor)
    float temp = 0.0;
    temp = analogRead(TEMP_EXT);
    temp = (temp / 1024.0) * 5.0; //find voltage at divider
    temp = (10000 * 5.0 / temp) - 10000;//calculate resistance
    temp = 229.23 - 22.009493 * log(temp);//convert resistance to temperature using non-linear transfer function
    return temp;
}

void Tomcat_Claw(int grip, int wrist) {

    if (grip > 127)
        GRIPPER_DIR = 1;
    else
        GRIPPER_DIR = 0;

    if (grip == 127)
        GRIPPER_EN = 0;
    else
        GRIPPER_EN = 1;

    if (wrist > 127)
        WRIST_DIR = 1;
    else
        WRIST_DIR = 0;

    if (wrist == 127)
        WRIST_EN = 0;
    else
        WRIST_EN = 1;


}

void Tomcat_TX_data(char tx_buff[], char num_bytes) {
    char k = 0;
    while (Busy1USART());
    Write1USART('$');

    for (k = 0; k < num_bytes; k++) {
        while (Busy1USART());
        Write1USART(tx_buff[k]);
    }
    Write1USART('\n');
}

void Tomcat_TX_warn(char code) {

    while (Busy1USART());
    Write1USART('$');
    while (Busy1USART());
    Write1USART('?');
    while (Busy1USART());
    Write1USART(code);
}

void Tomcat_TX_error(char code) {

    while (Busy1USART());
    Write1USART('$');
    while (Busy1USART());
    Write1USART('!');
    while (Busy1USART());
    Write1USART(code);
    while (Busy1USART());
    Write1USART('\n');

}

void Tomcat_Camera(int pan, int tilt) {
    int pan_pos = analogRead(CAM_PAN);
    int tilt_pos = analogRead(CAM_TILT);
    //TODO add code to set cam pos


}
//analog read function for vehicle

unsigned int analogRead(char channel) {
    unsigned int num = 0;
    ADCON0bits.CHS = channel; //calling channel
    ADCON0bits.GO = 1;
    while (ADCON0bits.GO == 1); //wait til done processing
    num = ADRES; //10bit value
    return (num);
}