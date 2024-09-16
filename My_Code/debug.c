#include "debug.h"

int fputc(int c,FILE *f)
{
    uint8_t ch[1]={c};
    HAL_UART_Transmit(&huart1,ch,1,0xFFFF);
    return c;
}