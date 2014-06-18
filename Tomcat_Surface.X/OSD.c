#include "OSD.h"


void OSD_write(char _row, char _col, unsigned char _font)
{
    unsigned int pos=30*(_row-1)+(_col-1);
    unsigned char posl=pos;
    unsigned char posh=pos>>8;
    INTOFF;
    WriteSPI(0x05);
    WriteSPI(posh);

    WriteSPI(0x06);
    WriteSPI(posl);

    WriteSPI(0x07);
    WriteSPI(_font);
    INTON;
}

void OSD_write_blink(char _row, char _col, unsigned char _font)
{
    unsigned int pos=30*(_row-1)+(_col-1);
    unsigned char posl=pos;
    unsigned char posh=pos>>8;
    char dmm=0;
    INTOFF;
    WriteSPI(0x84);
    dmm=ReadSPI();
    dmm=dmm|0x10;
    WriteSPI(0x04);
    WriteSPI(dmm);

    WriteSPI(0x05);
    WriteSPI(posh);

    WriteSPI(0x06);
    WriteSPI(posl);

    WriteSPI(0x07);
    WriteSPI(_font);

    WriteSPI(0x84);
    dmm=ReadSPI();
    dmm=dmm&~0x10;
    WriteSPI(0x04);
    WriteSPI(dmm);

    INTON;
}
void OSD_init()
{
    TRISCbits.RC0=0;
    LATCbits.LATC0=0;
    INTOFF;
    OpenSPI(SPI_FOSC_4,MODE_00,SMPMID);
    _delay(100000);
    _delay(100000);
    _delay(100000);
    LATCbits.LATC0=1;
    _delay(100000);
    _delay(100000);
    _delay(100000);
    WriteSPI(0x00);
    WriteSPI(0b00000100);
    WriteSPI(0x01);
    WriteSPI(0b01001111);
    WriteSPI(0x02);
    WriteSPI(0b00101011);
    WriteSPI(0x03);
    WriteSPI(0b00010110);

    OSD_disp(0);
    INTOFF;
    WriteSPI(0x09);
    WriteSPI(0x00);
    WriteSPI(0x08);
    WriteSPI(0b01010000);
    WriteSPI(0x0A);
    WriteSPI(0x01);
    WriteSPI(0x0B);
    WriteSPI(0b01010101);
    WriteSPI(0x08);
    WriteSPI(0b10100000);
    _delay(100000);
    _delay(100000);
    INTON;
}
void OSD_disp(char _on)
{
    char vm0=0;
    INTOFF;
    WriteSPI(0x80);
    vm0=ReadSPI();
    if(_on){

    vm0=vm0|8;
    }
    else
    {
        vm0=vm0&~8;
    }
    WriteSPI(0x00);
    WriteSPI(8);
    INTON;
}
void OSD_clear(void)
{
    char dmm=0;
    INTOFF;
    WriteSPI(0x84);
    dmm=ReadSPI();
    dmm=dmm|2;
    WriteSPI(0x04);
    WriteSPI(dmm);
    INTON;
}
void Str_output(int row, int col, char word[], char num) //starting row and column
{

    char n = 0;
    while (n <= num) {
        OSD_write(row, col, char_map[word[n]]);
        n++;
        col++;
    }


}
