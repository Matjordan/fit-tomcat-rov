#include "LSM9DS0.h"


unsigned int openLSM9(unsigned char _xmadd,unsigned char _gadd)
{
    char gtest=0,xmtest=0;

    _xmaddress=_xmadd<<1;
    _gaddress=_gadd<<1;
		// Read the gyro WHO_AM_I
    xmtest = xmReadByte(WHO_AM_I_XM);
    gtest = gReadByte(WHO_AM_I_G);

    openLSM9_accel();
    openLSM9_gyro();
    openLSM9_mag();

    return (xmtest<<8)+gtest;
}

void openLSM9_accel(void)
{
    /* CTRL_REG0_XM (0x1F) (Default value: 0x00)
	Bits (7-0): BOOT FIFO_EN WTM_EN 0 0 HP_CLICK HPIS1 HPIS2
	BOOT - Reboot memory content (0: normal, 1: reboot)
	FIFO_EN - Fifo enable (0: disable, 1: enable)
	WTM_EN - FIFO watermark enable (0: disable, 1: enable)
	HP_CLICK - HPF enabled for click (0: filter bypassed, 1: enabled)
	HPIS1 - HPF enabled for interrupt generator 1 (0: bypassed, 1: enabled)
	HPIS2 - HPF enabled for interrupt generator 2 (0: bypassed, 1 enabled)   */
	xmWriteByte(CTRL_REG0_XM, 0x00);

	/* CTRL_REG1_XM (0x20) (Default value: 0x07)
	Bits (7-0): AODR3 AODR2 AODR1 AODR0 BDU AZEN AYEN AXEN
	AODR[3:0] - select the acceleration data rate:
		0000=power down, 0001=3.125Hz, 0010=6.25Hz, 0011=12.5Hz,
		0100=25Hz, 0101=50Hz, 0110=100Hz, 0111=200Hz, 1000=400Hz,
		1001=800Hz, 1010=1600Hz, (remaining combinations undefined).
	BDU - block data update for accel AND mag
		0: Continuous update
		1: Output registers aren't updated until MSB and LSB have been read.
	AZEN, AYEN, and AXEN - Acceleration x/y/z-axis enabled.
		0: Axis disabled, 1: Axis enabled									 */
	xmWriteByte(CTRL_REG1_XM,0b10010111 ); // 800Hz data rate, x/y/z all enabled
//0b10010111
	//Serial.println(xmReadByte(CTRL_REG1_XM));
	/* CTRL_REG2_XM (0x21) (Default value: 0x00)
	Bits (7-0): ABW1 ABW0 AFS2 AFS1 AFS0 AST1 AST0 SIM
	ABW[1:0] - Accelerometer anti-alias filter bandwidth
		00=773Hz, 01=194Hz, 10=362Hz, 11=50Hz
	AFS[2:0] - Accel full-scale selection
		000=+/-2g, 001=+/-4g, 010=+/-6g, 011=+/-8g, 100=+/-16g
	AST[1:0] - Accel self-test enable
		00=normal (no self-test), 01=positive st, 10=negative st, 11=not allowed
	SIM - SPI mode selection
		0=4-wire, 1=3-wire													 */
	xmWriteByte(CTRL_REG2_XM, 0b11010000); // Set scale to 2g and 50hz filter
//0b11000000

	xmWriteByte(CTRL_REG3_XM, 0x04);
}

void openLSM9_gyro(void)
{
/* CTRL_REG1_G sets output data rate, bandwidth, power-down and enables
	Bits[7:0]: DR1 DR0 BW1 BW0 PD Zen Xen Yen
	DR[1:0] - Output data rate selection
		00=95Hz, 01=190Hz, 10=380Hz, 11=760Hz
	BW[1:0] - Bandwidth selection (sets cutoff frequency)
		 Value depends on ODR. See datasheet table 21.
	PD - Power down enable (0=power down mode, 1=normal or sleep mode)
	Zen, Xen, Yen - Axis enable (o=disabled, 1=enabled)	*/
	gWriteByte(CTRL_REG1_G, 0x0F); // Normal mode, enable all axes

	/* CTRL_REG2_G sets up the HPF
	Bits[7:0]: 0 0 HPM1 HPM0 HPCF3 HPCF2 HPCF1 HPCF0
	HPM[1:0] - High pass filter mode selection
		00=normal (reset reading HP_RESET_FILTER, 01=ref signal for filtering,
		10=normal, 11=autoreset on interrupt
	HPCF[3:0] - High pass filter cutoff frequency
		Value depends on data rate. See datasheet table 26.
	*/
	gWriteByte(CTRL_REG2_G, 0x00); // Normal mode, high cutoff frequency

	/* CTRL_REG3_G sets up interrupt and DRDY_G pins
	Bits[7:0]: I1_IINT1 I1_BOOT H_LACTIVE PP_OD I2_DRDY I2_WTM I2_ORUN I2_EMPTY
	I1_INT1 - Interrupt enable on INT_G pin (0=disable, 1=enable)
	I1_BOOT - Boot status available on INT_G (0=disable, 1=enable)
	H_LACTIVE - Interrupt active configuration on INT_G (0:high, 1:low)
	PP_OD - Push-pull/open-drain (0=push-pull, 1=open-drain)
	I2_DRDY - Data ready on DRDY_G (0=disable, 1=enable)
	I2_WTM - FIFO watermark interrupt on DRDY_G (0=disable 1=enable)
	I2_ORUN - FIFO overrun interrupt on DRDY_G (0=disable 1=enable)
	I2_EMPTY - FIFO empty interrupt on DRDY_G (0=disable 1=enable) */
	// Int1 enabled (pp, active low), data read on DRDY_G:
	gWriteByte(CTRL_REG3_G, 0x88);

	/* CTRL_REG4_G sets the scale, update mode
	Bits[7:0] - BDU BLE FS1 FS0 - ST1 ST0 SIM
	BDU - Block data update (0=continuous, 1=output not updated until read
	BLE - Big/little endian (0=data LSB @ lower address, 1=LSB @ higher add)
	FS[1:0] - Full-scale selection
		00=245dps, 01=500dps, 10=2000dps, 11=2000dps
	ST[1:0] - Self-test enable
		00=disabled, 01=st 0 (x+, y-, z-), 10=undefined, 11=st 1 (x-, y+, z+)
	SIM - SPI serial interface mode select
		0=4 wire, 1=3 wire */
	gWriteByte(CTRL_REG4_G, 0x00); // Set scale to 245 dps

	/* CTRL_REG5_G sets up the FIFO, HPF, and INT1
	Bits[7:0] - BOOT FIFO_EN - HPen INT1_Sel1 INT1_Sel0 Out_Sel1 Out_Sel0
	BOOT - Reboot memory content (0=normal, 1=reboot)
	FIFO_EN - FIFO enable (0=disable, 1=enable)
	HPen - HPF enable (0=disable, 1=enable)
	INT1_Sel[1:0] - Int 1 selection configuration
	Out_Sel[1:0] - Out selection configuration */
	gWriteByte(CTRL_REG5_G, 0x00);
}

void openLSM9_mag(void)
{
/* CTRL_REG5_XM enables temp sensor, sets mag resolution and data rate
	Bits (7-0): TEMP_EN M_RES1 M_RES0 M_ODR2 M_ODR1 M_ODR0 LIR2 LIR1
	TEMP_EN - Enable temperature sensor (0=disabled, 1=enabled)
	M_RES[1:0] - Magnetometer resolution select (0=low, 3=high)
	M_ODR[2:0] - Magnetometer data rate select
		000=3.125Hz, 001=6.25Hz, 010=12.5Hz, 011=25Hz, 100=50Hz, 101=100Hz
	LIR2 - Latch interrupt request on INT2_SRC (cleared by reading INT2_SRC)
		0=interrupt request not latched, 1=interrupt request latched
	LIR1 - Latch interrupt request on INT1_SRC (cleared by readging INT1_SRC)
		0=irq not latched, 1=irq latched 									 */
	xmWriteByte(CTRL_REG5_XM, 0b00010100); // Mag data rate - 100 Hz

	/* CTRL_REG6_XM sets the magnetometer full-scale
	Bits (7-0): 0 MFS1 MFS0 0 0 0 0 0
	MFS[1:0] - Magnetic full-scale selection
	00:+/-2Gauss, 01:+/-4Gs, 10:+/-8Gs, 11:+/-12Gs							 */
	xmWriteByte(CTRL_REG6_XM, 0x01); // Mag scale to +/- 2GS

	/* CTRL_REG7_XM sets magnetic sensor mode, low power mode, and filters
	AHPM1 AHPM0 AFDS 0 0 MLP MD1 MD0
	AHPM[1:0] - HPF mode selection
		00=normal (resets reference registers), 01=reference signal for filtering,
		10=normal, 11=autoreset on interrupt event
	AFDS - Filtered acceleration data selection
		0=internal filter bypassed, 1=data from internal filter sent to FIFO
	MLP - Magnetic data low-power mode
		0=data rate is set by M_ODR bits in CTRL_REG5
		1=data rate is set to 3.125Hz
	MD[1:0] - Magnetic sensor mode selection (default 10)
		00=continuous-conversion, 01=single-conversion, 10 and 11=power-down */
	xmWriteByte(CTRL_REG7_XM, 0b00000000); // Continuous conversion mode

	/* CTRL_REG4_XM is used to set interrupt generators on INT2_XM
	Bits (7-0): P2_TAP P2_INT1 P2_INT2 P2_INTM P2_DRDYA P2_DRDYM P2_Overrun P2_WTM
	*/
	xmWriteByte(CTRL_REG4_XM, 0x04); // Magnetometer data ready on INT2_XM (0x08)

	/* INT_CTRL_REG_M to set push-pull/open drain, and active-low/high
	Bits[7:0] - XMIEN YMIEN ZMIEN PP_OD IEA IEL 4D MIEN
	XMIEN, YMIEN, ZMIEN - Enable interrupt recognition on axis for mag data
	PP_OD - Push-pull/open-drain interrupt configuration (0=push-pull, 1=od)
	IEA - Interrupt polarity for accel and magneto
		0=active-low, 1=active-high
	IEL - Latch interrupt request for accel and magneto
		0=irq not latched, 1=irq latched
	4D - 4D enable. 4D detection is enabled when 6D bit in INT_GEN1_REG is set
	MIEN - Enable interrupt generation for magnetic data
		0=disable, 1=enable) */
	xmWriteByte(INT_CTRL_REG_M, 0x09); // Enable interrupts for mag, active-low, push-pull
}

void readLSM9_accel(unsigned int dest[])
{
    //xmReadBytes(WHO_AM_I_XM, temp, 6);
    dest[0] = (xmReadByte(OUT_X_H_A) << 8) | xmReadByte(OUT_X_L_A);
    dest[1] = (xmReadByte(OUT_Y_H_A) << 8) | xmReadByte(OUT_Y_L_A);
    dest[2] = (xmReadByte(OUT_Z_H_A) << 8) | xmReadByte(OUT_Z_L_A);
}
void readLSM9_mag(unsigned int dest[])
{
    //xmReadBytes(WHO_AM_I_XM, temp, 6);
    dest[0] = (xmReadByte(OUT_X_H_M) << 8) | xmReadByte(OUT_X_L_M);
    dest[1] = (xmReadByte(OUT_Y_H_M) << 8) | xmReadByte(OUT_Y_L_M);
    dest[2] = (xmReadByte(OUT_Z_H_M) << 8) | xmReadByte(OUT_Z_L_M);
}
void readLSM9_gyro(unsigned int dest[])
{
    //xmReadBytes(WHO_AM_I_XM, temp, 6);
    dest[0] = (gReadByte(OUT_X_H_G) << 8) | gReadByte(OUT_X_L_G);
    dest[1] = (gReadByte(OUT_Y_H_G) << 8) | gReadByte(OUT_Y_L_G);
    dest[2] = (gReadByte(OUT_Z_H_G) << 8) | gReadByte(OUT_Z_L_G);
}
unsigned char gReadByte(unsigned char subadd)
{
    return (I2CReadByte(_gaddress>>1,subadd));
}
void gReadBytes(unsigned char subadd,unsigned char dest[],unsigned char count)
{
    char i=0;
    StartI2C1();
    while(WriteI2C1(_gaddress)!=0);
    while(WriteI2C1(subadd)!=0);
    RestartI2C1();
    while(WriteI2C1(1+_gaddress)!=0);
    for (i=0; i<count-1 ;i++)
    {
        dest[i]=ReadI2C1();
        AckI2C1();
    }
    dest[count-1]=ReadI2C1();
    NotAckI2C1();
    StopI2C1();
}

unsigned char xmReadByte(unsigned char subadd)
{
    return (I2CReadByte(_xmaddress>>1,subadd));
}
void xmReadBytes(unsigned char subadd,unsigned char dest[],unsigned char count)
{
    char i=0;
    StartI2C1();
    while(WriteI2C1(_xmaddress)!=0);
    while(WriteI2C1(subadd)!=0);
    RestartI2C1();
    while(WriteI2C1(1+_xmaddress)!=0);
    while( getsI2C(dest,count) );
//    for (i=0; i<count-1 ;i++)
//    {
//        //while(!DataRdyI2C1());
//        dest[i]=ReadI2C1();
//        AckI2C1();
//    }
//    dest[count-1]=ReadI2C1();
    NotAckI2C1();
    StopI2C1();
    
}

void gWriteByte(unsigned char subadd,unsigned char data)
{
    StartI2C1();
    WriteI2C1(_gaddress);
    //while(!SSP1CON2bits.ACKSTAT);
    WriteI2C1(subadd);
    //while(!SSP1CON2bits.ACKSTAT);
    WriteI2C1(data);
    //while(!SSP1CON2bits.ACKSTAT);
    StopI2C1();
}
void xmWriteByte(unsigned char subadd,unsigned char data)
{
    StartI2C1();
    WriteI2C1(_xmaddress);
    //while(!SSP1CON2bits.ACKSTAT);
    WriteI2C1(subadd);
    //while(!SSP1CON2bits.ACKSTAT);
    WriteI2C1(data);
    //while(!SSP1CON2bits.ACKSTAT);
    StopI2C1();
}
