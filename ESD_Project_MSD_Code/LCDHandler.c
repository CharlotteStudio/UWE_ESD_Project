#include <msp430.h>

#define FontSize       8
#define ColumnSize     16
#define MaxColumnSize  16 // 128 / 8
#define IconSize       16   // icon   is 16 x 16
#define NumberFontSize 24   // number is 24 x 16
#define LCDSize        96

char DisplayBuffer[LCDSize][ColumnSize];


const char Icon[16][10] = {
    // row count
    //    alarm      Chrono         Wifi      timer     calender
    {0x7F, 0xFE, 0x87, 0xE3, 0x07, 0xF0, 0x07 ,0xE0, 0x00, 0x00},
    {0xFF, 0xFF, 0x5F, 0xFA, 0x1E, 0xF8, 0x1F ,0xF8, 0x00, 0x00},
    {0xFF, 0xFF, 0x3F, 0xFC, 0x38, 0x1C, 0x38 ,0x1C, 0x7F, 0xFE},
    {0xE0, 0x07, 0x78, 0x1E, 0x73, 0xCE, 0x71 ,0x8E, 0xFF, 0xFF},
    {0xE7, 0xC7, 0x70, 0xEE, 0x64, 0x26, 0x61 ,0x86, 0xFF, 0xFF},
    {0xE4, 0x27, 0xE0, 0xF7, 0xC8, 0x13, 0xC1 ,0x93, 0xE0, 0x07},
    {0xE4, 0x27, 0xE0, 0xF7, 0xD3, 0xCB, 0xC1 ,0xB3, 0xED, 0xB7},
    {0xE7, 0xC7, 0xE0, 0xF7, 0xC4, 0x23, 0xC1 ,0xE3, 0xE0, 0x07},

    {0xE5, 0x07, 0xE3, 0xF7, 0xC8, 0x13, 0xC1 ,0xC3, 0xED, 0xB7},
    {0xE4, 0x87, 0xE7, 0xF7, 0xC3, 0xC3, 0xC0 ,0x03, 0xE0, 0x07},
    {0xE4, 0x47, 0xEF, 0xF7, 0xC4, 0x23, 0xC0 ,0x03, 0xED, 0xB7},
    {0xE4, 0x27, 0x77, 0xEE, 0x61, 0x86, 0x60 ,0x06, 0xE0, 0x07},
    {0xE0, 0x07, 0x78, 0x1E, 0x71, 0x8E, 0x70 ,0x0E, 0xE0, 0x07},
    {0xFF, 0xFF, 0x3F, 0xFC, 0x38, 0x1C, 0x38 ,0x1C, 0xFF, 0xFF},
    {0xFF, 0xFF, 0x1F, 0xF8, 0x1F, 0xF8, 0x1F ,0xF8, 0xFF, 0xFF},
    {0x7F, 0xFE, 0x07, 0xE0, 0x07, 0xE0, 0x07 ,0xE0, 0x7F, 0xFE}
};

const char Font[24][50] =
{
    // row count                                                                                                         20                      24                      28          30          32          34          36          38          40          42          44          46          48          50
    //        0  emply    1           2           3           4           5           6           7           8           9           M           o           T           u           W           e           T          h            F           r           S           a           S           u     :     -
    {0x3F, 0xFC, 0x00, 0x00, 0x3F, 0xFC, 0x3F, 0xFC, 0x00, 0x00, 0x3F, 0xFC, 0x3F, 0xFC, 0x3F, 0xFC, 0x3F, 0xFC, 0x3F, 0xFC, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFE, 0xFF, 0x00, 0x00, 0x3F, 0xFE, 0x00, 0x00, 0x3F, 0xFC, 0x00, 0x00, 0x3F, 0xFC, 0x00, 0x00, 0x00, 0x00},
    {0x5F, 0xFA, 0x00, 0x02, 0x1F, 0xFA, 0x1F, 0xFA, 0x40, 0x02, 0x5F, 0xF8, 0x5F, 0xF8, 0x1F, 0xF4, 0x5F, 0xFA, 0x5F, 0xFA, 0x40, 0x02, 0x00, 0x00, 0x7F, 0xFE, 0x00, 0x00, 0x40, 0x02, 0x00, 0x00, 0x7F, 0xFE, 0x40, 0x00, 0x5F, 0xFC, 0x00, 0x00, 0x5F, 0xFE, 0x00, 0x00, 0x5F, 0xFE, 0x00, 0x00, 0x00, 0x00},
    {0x6F, 0xF6, 0x00, 0x06, 0x0F, 0xF6, 0x0F, 0xF6, 0x60, 0x06, 0x6F, 0xF0, 0x6F, 0xF0, 0x0F, 0xF6, 0x6F, 0xF6, 0x6F, 0xF6, 0x60, 0x06, 0x00, 0x00, 0x3F, 0xFC, 0x00, 0x00, 0x60, 0x06, 0x00, 0x00, 0x3F, 0xFC, 0x60, 0x00, 0x67, 0xF8, 0x00, 0x00, 0x6F, 0xFE, 0x00, 0x00, 0x6F, 0xFE, 0x00, 0x00, 0x00, 0x00},
    {0x70, 0x0E, 0x00, 0x0E, 0x00, 0x0E, 0x00, 0x0E, 0x70, 0x0E, 0x70, 0x00, 0x70, 0x00, 0x00, 0x0E, 0x70, 0x0E, 0x70, 0x0E, 0x70, 0x0E, 0x00, 0x00, 0x03, 0xC0, 0x00, 0x00, 0x70, 0x0E, 0x00, 0x00, 0x03, 0xC0, 0x70, 0x00, 0x70, 0x00, 0x00, 0x00, 0x70, 0x06, 0x00, 0x00, 0x70, 0x06, 0x00, 0x00, 0x3C, 0x00},
    {0x70, 0x0E, 0x00, 0x0E, 0x00, 0x0E, 0x00, 0x0E, 0x70, 0x0E, 0x70, 0x00, 0x70, 0x00, 0x00, 0x0E, 0x70, 0x0E, 0x70, 0x0E, 0x74, 0x2E, 0x00, 0x00, 0x03, 0xC0, 0x00, 0x00, 0x70, 0x0E, 0x00, 0x00, 0x03, 0xC0, 0x70, 0x00, 0x70, 0x00, 0x00, 0x00, 0x70, 0x02, 0x00, 0x00, 0x70, 0x02, 0x00, 0x00, 0x3C, 0x00},
    {0x70, 0x0E, 0x00, 0x0E, 0x00, 0x0E, 0x00, 0x0E, 0x70, 0x0E, 0x70, 0x00, 0x70, 0x00, 0x00, 0x0E, 0x70, 0x0E, 0x70, 0x0E, 0x77, 0x6E, 0x00, 0x00, 0x03, 0xC0, 0x00, 0x00, 0x70, 0x0E, 0x00, 0x00, 0x03, 0xC0, 0x70, 0x00, 0x70, 0x00, 0x00, 0x00, 0x70, 0x00, 0x00, 0x00, 0x70, 0x00, 0x00, 0x00, 0x3C, 0x00},
    {0x70, 0x0E, 0x00, 0x0E, 0x00, 0x0E, 0x00, 0x0E, 0x70, 0x0E, 0x70, 0x00, 0x70, 0x00, 0x00, 0x0E, 0x70, 0x0E, 0x70, 0x0E, 0x77, 0xEE, 0x00, 0x00, 0x03, 0xC0, 0x00, 0x00, 0x70, 0x0E, 0x00, 0x00, 0x03, 0xC0, 0x70, 0x00, 0x70, 0x00, 0x40, 0x00, 0x70, 0x00, 0x00, 0x00, 0x70, 0x00, 0x00, 0x00, 0x3C, 0x00},
    {0x70, 0x0E, 0x00, 0x0E, 0x00, 0x0E, 0x00, 0x0E, 0x70, 0x0E, 0x70, 0x00, 0x70, 0x00, 0x00, 0x0E, 0x70, 0x0E, 0x70, 0x0E, 0x77, 0xEE, 0x00, 0x00, 0x03, 0xC0, 0x00, 0x00, 0x70, 0x0E, 0x00, 0x00, 0x03, 0xC0, 0x70, 0x00, 0x70, 0x00, 0x60, 0x00, 0x70, 0x00, 0x00, 0x00, 0x70, 0x00, 0x00, 0x00, 0x3C, 0x00},

    {0x70, 0x0E, 0x00, 0x0E, 0x00, 0x0E, 0x00, 0x0E, 0x70, 0x0E, 0x70, 0x00, 0x70, 0x00, 0x00, 0x0E, 0x70, 0x0E, 0x70, 0x0E, 0x73, 0xCE, 0x00, 0x00, 0x03, 0xC0, 0x00, 0x00, 0x70, 0x0E, 0x00, 0x00, 0x03, 0xC0, 0x70, 0x00, 0x70, 0x00, 0x70, 0x00, 0x70, 0x00, 0x00, 0x00, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x70, 0x0E, 0x00, 0x0E, 0x00, 0x0E, 0x00, 0x0E, 0x70, 0x0E, 0x70, 0x00, 0x70, 0x00, 0x00, 0x0E, 0x70, 0x0E, 0x70, 0x0E, 0x71, 0x8E, 0x00, 0x00, 0x03, 0xC0, 0x00, 0x00, 0x70, 0x0E, 0x00, 0x00, 0x03, 0xC0, 0x70, 0x00, 0x70, 0x00, 0x70, 0x00, 0x70, 0x00, 0x1F, 0xF8, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x20, 0x04, 0x00, 0x04, 0x0F, 0xF4, 0x0F, 0xF4, 0x2F, 0xF4, 0x2F, 0xF0, 0x2F, 0xF0, 0x00, 0x04, 0x2F, 0xF4, 0x2F, 0xF4, 0x20, 0x04, 0x0F, 0xF0, 0x03, 0xC0, 0x00, 0x00, 0x20, 0x04, 0x0F, 0xF0, 0x03, 0xC0, 0x2F, 0xF0, 0x2F, 0xF0, 0x2F, 0xF0, 0x2F, 0xF0, 0x3F, 0xFC, 0x2F, 0xF0, 0x00, 0x00, 0x00, 0xFF},
    {0x00, 0x00, 0x00, 0x00, 0x1F, 0xF8, 0x1F, 0xF8, 0x1F, 0xF8, 0x1F, 0xF8, 0x1F, 0xF8, 0x00, 0x00, 0x1F, 0xF8, 0x1F, 0xF8, 0x00, 0x00, 0x1F, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x1F, 0xF8, 0x00, 0x00, 0x1F, 0xF8, 0x1F, 0xF8, 0x1F, 0xF8, 0x1F, 0xF8, 0x7F, 0xFE, 0x1F, 0xF8, 0x00, 0x00, 0x00, 0xFF},
    {0x00, 0x00, 0x00, 0x00, 0x1F, 0xF8, 0x1F, 0xF8, 0x1F, 0xF8, 0x1F, 0xF8, 0x1F, 0xF8, 0x00, 0x00, 0x1F, 0xF8, 0x1F, 0xF8, 0x00, 0x00, 0x1F, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x1F, 0xF8, 0x00, 0x00, 0x1F, 0xF8, 0x1F, 0xF8, 0x1F, 0xF8, 0x1F, 0xF8, 0x7F, 0xFE, 0x1F, 0xF8, 0x00, 0x00, 0x00, 0xFF},
    {0x20, 0x04, 0x00, 0x04, 0x2F, 0xF0, 0x0F, 0xF4, 0x0F, 0xF4, 0x0F, 0xF4, 0x2F, 0xF4, 0x00, 0x04, 0x2F, 0xF4, 0x0F, 0xF4, 0x20, 0x04, 0x2F, 0xF4, 0x03, 0xC0, 0x2D, 0x0D, 0x23, 0xC4, 0x2F, 0xF4, 0x03, 0xC0, 0x2F, 0xF4, 0x2F, 0xF0, 0x2F, 0xF0, 0x0F, 0xF4, 0x60, 0x0E, 0x0F, 0xF4, 0x20, 0x04, 0x00, 0xFF},
    {0x70, 0x0E, 0x00, 0x0E, 0x70, 0x00, 0x00, 0x0E, 0x00, 0x0E, 0x00, 0x0E, 0x70, 0x0E, 0x00, 0x0E, 0x70, 0x0E, 0x00, 0x0E, 0x70, 0x0E, 0x70, 0x0E, 0x03, 0xC0, 0x70, 0x0E, 0x73, 0xCE, 0x70, 0x0E, 0x03, 0xC0, 0x70, 0x0E, 0x70, 0x00, 0x70, 0x00, 0x00, 0x0E, 0x40, 0x0E, 0x00, 0x0E, 0x70, 0x0E, 0x00, 0x00},
    {0x70, 0x0E, 0x00, 0x0E, 0x70, 0x00, 0x00, 0x0E, 0x00, 0x0E, 0x00, 0x0E, 0x70, 0x0E, 0x00, 0x0E, 0x70, 0x0E, 0x00, 0x0E, 0x70, 0x0E, 0x70, 0x0E, 0x03, 0xC0, 0x70, 0x0E, 0x77, 0xEE, 0x70, 0x0E, 0x03, 0xC0, 0x70, 0x0E, 0x70, 0x00, 0x70, 0x00, 0x00, 0x0E, 0x00, 0x0E, 0x00, 0x0E, 0x70, 0x0E, 0x00, 0x00},

    {0x70, 0x0E, 0x00, 0x0E, 0x70, 0x00, 0x00, 0x0E, 0x00, 0x0E, 0x00, 0x0E, 0x70, 0x0E, 0x00, 0x0E, 0x70, 0x0E, 0x00, 0x0E, 0x70, 0x0E, 0x70, 0x0E, 0x03, 0xC0, 0x70, 0x0E, 0x77, 0xEE, 0x71, 0xFE, 0x03, 0xC0, 0x70, 0x0E, 0x70, 0x00, 0x70, 0x00, 0x00, 0x0E, 0x3F, 0xFE, 0x00, 0x0E, 0x70, 0x0E, 0x3C, 0x00},
    {0x70, 0x0E, 0x00, 0x0E, 0x70, 0x00, 0x00, 0x0E, 0x00, 0x0E, 0x00, 0x0E, 0x70, 0x0E, 0x00, 0x0E, 0x70, 0x0E, 0x00, 0x0E, 0x70, 0x0E, 0x70, 0x0E, 0x03, 0xC0, 0x70, 0x0E, 0x77, 0xEE, 0x73, 0xFE, 0x03, 0xC0, 0x70, 0x0E, 0x70, 0x00, 0x70, 0x00, 0x00, 0x0E, 0x7F, 0xFE, 0x00, 0x0E, 0x70, 0x0E, 0x3C, 0x00},
    {0x70, 0x0E, 0x00, 0x0E, 0x70, 0x00, 0x00, 0x0E, 0x00, 0x0E, 0x00, 0x0E, 0x70, 0x0E, 0x00, 0x0E, 0x70, 0x0E, 0x00, 0x0E, 0x70, 0x0E, 0x70, 0x0E, 0x03, 0xC0, 0x70, 0x0E, 0x76, 0x6E, 0x71, 0xFC, 0x03, 0xC0, 0x70, 0x0E, 0x70, 0x00, 0x70, 0x00, 0x00, 0x0E, 0x7F, 0xFE, 0x00, 0x0E, 0x70, 0x0E, 0x3C, 0x00},
    {0x70, 0x0E, 0x00, 0x0E, 0x70, 0x00, 0x00, 0x0E, 0x00, 0x0E, 0x00, 0x0E, 0x70, 0x0E, 0x00, 0x0E, 0x70, 0x0E, 0x00, 0x0E, 0x70, 0x0E, 0x70, 0x0E, 0x03, 0xC0, 0x70, 0x0E, 0x74, 0x2E, 0x70, 0x00, 0x03, 0xC0, 0x70, 0x0E, 0x70, 0x00, 0x70, 0x00, 0x40, 0x0E, 0x70, 0x0E, 0x40, 0x0E, 0x70, 0x0E, 0x3C, 0x00},
    {0x70, 0x0E, 0x00, 0x0E, 0x70, 0x00, 0x00, 0x0E, 0x00, 0x0E, 0x00, 0x0E, 0x70, 0x0E, 0x00, 0x0E, 0x70, 0x0E, 0x00, 0x0E, 0x70, 0x0E, 0x70, 0x0E, 0x03, 0xC0, 0x70, 0x0E, 0x70, 0x0E, 0x70, 0x00, 0x03, 0xC0, 0x70, 0x0E, 0x70, 0x00, 0x70, 0x00, 0x60, 0x0E, 0x70, 0x0E, 0x60, 0x0E, 0x70, 0x0E, 0x3C, 0x00},
    {0x6F, 0xF6, 0x00, 0x06, 0x6F, 0xF0, 0x0F, 0xF6, 0x00, 0x06, 0x0F, 0xF6, 0x6F, 0xF6, 0x00, 0x06, 0x6F, 0xF6, 0x0F, 0xF6, 0x60, 0x06, 0x6F, 0xF6, 0x03, 0xC0, 0x6F, 0xF6, 0x60, 0x06, 0x6F, 0xF0, 0x03, 0xC0, 0x60, 0x06, 0x60, 0x00, 0x60, 0x00, 0x7F, 0xF6, 0x6F, 0xF6, 0x7F, 0xF6, 0x6F, 0xF6, 0x00, 0x00},
    {0x5F, 0xFA, 0x00, 0x02, 0x5F, 0xF8, 0x1F, 0xFA, 0x00, 0x02, 0x1F, 0xFA, 0x5F, 0xFA, 0x00, 0x02, 0x5F, 0xFA, 0x1F, 0xFA, 0x40, 0x02, 0x5F, 0xFA, 0x01, 0x80, 0x5F, 0xFA, 0x40, 0x02, 0x5F, 0xF8, 0x01, 0x80, 0x40, 0x02, 0x40, 0x00, 0x40, 0x00, 0x7F, 0xFA, 0x5F, 0xFA, 0x7F, 0xFA, 0x5F, 0xFA, 0x00, 0x00},
    {0x3F, 0xFC, 0x00, 0x00, 0x3F, 0xFC, 0x3F, 0xFC, 0x00, 0x00, 0x3F, 0xFC, 0x3F, 0xFC, 0x00, 0x00, 0x3F, 0xFC, 0x3F, 0xFC, 0x00, 0x00, 0x3F, 0xFC, 0x00, 0x00, 0x3F, 0xFC, 0x00, 0x00, 0x3F, 0xFC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3F, 0xFC, 0x3F, 0xFC, 0x3F, 0xFC, 0x3F, 0xFC, 0x00, 0x00}
};

void DisplayIcon(int index, int startRow, int startColumn)
{
    int firstColumn = index;
    int row, column;
    for (row = 0; row < IconSize; row++) //row
        for(column = firstColumn ; column < (index + 2); column++) //column
        {
            int startDisplayColumn = startColumn;
            if (column != firstColumn) // Second column
                startDisplayColumn = startColumn + 1;

            DisplayBuffer[row + startRow][startDisplayColumn] = ~Icon[row][column];
        }
}

void DisplayChar(int num, int startRow, int startColumn)
{
    int row, column;
    for (row = 0; row < NumberFontSize; row++) //row
        for(column = num; column < num + 1; column++) //column
            DisplayBuffer[row + startRow][startColumn] = ~Font[row][column];
}

void DisplayTime(int num, int startRow, int startColumn)
{
    int firstColumn = num * 2;
    int row, column;
    for (row = 0; row < NumberFontSize; row++) //row
        for(column = firstColumn ; column < (num * 2) + 2; column++) //column
        {
            int startDisplayColumn = startColumn;
            if (column != firstColumn) // Second column
                startDisplayColumn = startColumn + 1;

            DisplayBuffer[row + startRow][startDisplayColumn] = ~Font[row][column];
        }
}

void DisplayWeek(int week, int startRow, int startColumn){
    int startIndex = 16 + (week * 4);
    DisplayTime(startIndex / 2, startRow, startColumn);
    DisplayTime((startIndex + 2) / 2, startRow, startColumn + 2);
}

char reverse(char inchar)
{
    char outchar;
    outchar = 0;
    if ((inchar & BIT0) > 0) outchar |= BIT7;
    if ((inchar & BIT1) > 0) outchar |= BIT6;
    if ((inchar & BIT2) > 0) outchar |= BIT5;
    if ((inchar & BIT3) > 0) outchar |= BIT4;
    if ((inchar & BIT4) > 0) outchar |= BIT3;
    if ((inchar & BIT5) > 0) outchar |= BIT2;
    if ((inchar & BIT6) > 0) outchar |= BIT1;
    if ((inchar & BIT7) > 0) outchar |= BIT0;
    return outchar;
}

void StartDisplay()
{
    int line;
    int column;
    char command = 0x80;            // Write lines

    command = command ^ 0x40;       // VCOM bit

    P2OUT |= 0x10;                  // LCD CS high

    while (!(UCB0IFG & UCTXIFG));
    UCB0TXBUF = command;

    for (line = 0; line < LCDSize; line++)
    {
        while (!(UCB0IFG & UCTXIFG));
        UCB0TXBUF = reverse(line+1);

        for (column = 0; column < ColumnSize; column++)
        {
            while (!(UCB0IFG & UCTXIFG));
            UCB0TXBUF = DisplayBuffer[line][column];
        }

        //>>>>>>>>>>>>>>>>>>>>>>  for sharp128,  For Fill the empty column
        for (column = ColumnSize; column < MaxColumnSize; column++)
        {
            while (!(UCB0IFG & UCTXIFG));
            UCB0TXBUF = 0xFF;
        }
        //<<<<<<<<<<<<<<<<<<<<< for sharp128


        while (!(UCB0IFG & UCTXIFG));
        UCB0TXBUF = 0x00;            // End of line signal
    }


    //>>>>>>>>>>>>>>>>>>>>>>     For filling the empty line form 96 to 128
    for (line = LCDSize; line < 128; line++)
    {
        while (!(UCB0IFG & UCTXIFG));
        UCB0TXBUF = reverse(line+1);           //should be the hander of line


        for (column = 0; column < MaxColumnSize; column++)
        {
            while (!(UCB0IFG & UCTXIFG));
            UCB0TXBUF = 0xFF;               // fill 1
        }

        while (!(UCB0IFG & UCTXIFG));
        UCB0TXBUF = 0x00;            // End of line signal
    }
    //<<<<<<<<<<<<<<<<<<<<< for sharp128


    while (!(UCB0IFG & UCTXIFG));
    UCB0TXBUF = 0x00;               // End of block signal

    while ((UCB0STATW & UCBBUSY));

    __delay_cycles(8);              //Ensure a 2us min delay to meet the LCD's thSCS

    P2OUT &= ~0x10;                 // LCD CS low
}

void ClearDisplay()
{
    int i, j;
    for (i = 0; i < LCDSize; i++)
        for(j = 0; j < ColumnSize; j++)
            DisplayBuffer[i][j] = 0xFF;
}
