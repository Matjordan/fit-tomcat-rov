#include "I2C_mat.h"


unsigned char I2CReadByte(unsigned char add,unsigned char subadd)
{
    unsigned char _temp=0;
    StartI2C1();
    while(WriteI2C1(add<<1)!=0);
    while(WriteI2C1(subadd)!=0);
    RestartI2C1();
    while(WriteI2C1(1+(add<<1))!=0);
    _temp=ReadI2C1();
    NotAckI2C1();
    StopI2C1();
    return _temp;
}
void I2CWriteByte(unsigned char add,unsigned char subadd,unsigned char data)
{
    StartI2C1();
    WriteI2C1(add<<1);
    //while(!SSP1CON2bits.ACKSTAT);
    WriteI2C1(subadd);
    //while(!SSP1CON2bits.ACKSTAT);
    WriteI2C1(data);
    //while(!SSP1CON2bits.ACKSTAT);
    StopI2C1();
}