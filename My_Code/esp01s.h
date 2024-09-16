#ifndef __ESP01S_H
#define __ESP01S_H
#define ESP_HUART &huart1

void UsartPrintf(UART_HandleTypeDef *USARTx, char *fmt,...);
void Usart_SendString(UART_HandleTypeDef *huart, char* String);  
void ESP01S_Init(void);
void PUB_Data(int Hum,int Tem);
void ESP_IRQHandler(void);

#endif
