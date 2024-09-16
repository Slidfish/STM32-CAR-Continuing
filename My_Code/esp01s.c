#include "main.h"
#include "esp01s.h"
#include "stdio.h"
#include "string.h"
#include "stdarg.h"
#include "usart.h"
#define REV_OK		0	//接收完成标志
#define REV_WAIT	1	//接收未完成标志
#define ESP8266_WIFI_INFO		"AT+CWJAP=\"SLIDFISH\",\"15857411581\"\r\n"
#define MQTTUSERCFG             "AT+MQTTUSERCFG=0,1,\"NULL\",\"carcontrol&k1qs41MJ3Wd\",\"35c1e7e670e3ae4b05d45bf9704fa03cc9053321e092d585977a6c50ed2751ca\",0,0,\"\"\r\n"
#define MQTTCLIENTID           "AT+MQTTCLIENTID=0,\"k1qs41MJ3Wd.carcontrol|securemode=2\\,signmethod=hmacsha256\\,timestamp=1726065338177|\"\r\n"   
const char* MQTTCONN				="iot-06z00gcfhg1204h.mqtt.iothub.aliyuncs.com";
const char* MQTTSUB="/sys/k0cwzCsF9GJ/D001/thing/event/property/post_reply";   
uint16_t DataPointer=0;
#define DataSize 512
char RxData[DataSize]={0};
uint8_t CompeteRx=0;
void Usart_SendString(UART_HandleTypeDef *huart, char* String) {  
    // 通过 HAL_UART_Transmit 发送字符串  
    HAL_UART_Transmit(huart, (uint8_t*)String, strlen(String), 1000);  
}  
void UsartPrintf(UART_HandleTypeDef *USARTx, char *fmt,...)
{
    unsigned char UsartPrintfBuf[296];  
    va_list ap;  
    unsigned char *pStr = UsartPrintfBuf;  

    // 开始处理可变参数  
    va_start(ap, fmt);  
    
    // 将格式化字符串写入缓冲区  
    int len = vsnprintf((char *)UsartPrintfBuf, sizeof(UsartPrintfBuf), fmt, ap);  
    va_end(ap);  

    // 检查格式化是否成功  
    if (len < 0 || len >= sizeof(UsartPrintfBuf)) {  
        // 处理错误（例如，缓冲区溢出）  
        return;  
    }  

    // 通过 UART 发送格式化字符串  
    HAL_UART_Transmit(USARTx, UsartPrintfBuf, len, HAL_MAX_DELAY); 

}
void Trail_Rxed(void)
{
	uint16_t Temp=DataPointer;
	HAL_Delay(1);
	if(Temp==DataPointer)
	{
		HAL_Delay(5);
		CompeteRx=1;
	}
}

char* atemp;
uint8_t ESP8266_SendCmd(char *cmd, char *res)
{
	
	Usart_SendString(ESP_HUART,cmd);
	while(CompeteRx==0)Trail_Rxed();
	if(strstr(RxData, res) != NULL)
	{
		if(strstr(RxData,"switch")!=NULL)
		{
			HAL_Delay(2);
			atemp=strstr(RxData,"switch");
//			if(atemp[8]=='1')	  	LED_State(1);
//			else if(atemp[8]=='0')	LED_State(0);
				
		}
		//UsartPrintf(ESP_HUART, RxData);
		//UsartPrintf(ESP_HUART, "\r\n");		

		DataPointer=0;memset(RxData,0,DataSize);CompeteRx=0;
		return 0;
	}
	else
	{
		DataPointer=0;memset(RxData,0,DataSize);CompeteRx=0;
		HAL_Delay(10);	
		return 1;	
	}
	

}
void PUB_Data(int Hum,int Tem)
{
	char TxData[512]={0};
	sprintf(TxData,"AT+MQTTPUB=0,\"/sys/k0cwzCsF9GJ/D001/thing/event/property/post\",\"{\\\"params\\\": {\\\"CurrentHumidity\\\":%d\\,\\\"CurrentTemperature\\\":%d}}\",0,0\r\n",Hum,Tem);
	while(ESP8266_SendCmd(TxData, "OK"));	
}
void ESP01S_Start(void)
{
	//UsartPrintf(USART1, " RST\r\n");
	//while(ESP8266_SendCmd("AT+RESTORE\r\n", "ready"));
	//HAL_Delay(1500);


	//UsartPrintf(USART1, "1.AT\r\n");
	//while(ESP8266_SendCmd("AT\r\n", "OK"));
	//HAL_Delay(1500);
	//UsartPrintf(USART1, "1.ATE0\r\n");
	//while(ESP8266_SendCmd("ATE0\r\n", "OK"));
	//HAL_Delay(1500);
	//UsartPrintf(USART1, "2.CWMODE\r\n");
	//while(ESP8266_SendCmd("AT+CWMODE=1\r\n","OK"));
	//HAL_Delay(1500);
	//UsartPrintf(USART1, "2.CWQAP\r\n");
	//while(ESP8266_SendCmd("AT+CWQAP\r\n","OK"));
	//HAL_Delay(1500);
	//UsartPrintf(USART1, "3.CWJAP\r\n");
	//while(ESP8266_SendCmd(ESP8266_WIFI_INFO, "OK"));
	//HAL_Delay(1500);
	//UsartPrintf(USART1, "4.MQTTUSERCFG\r\n");
	while(ESP8266_SendCmd(MQTTUSERCFG, "OK"));
	//HAL_Delay(1500);	
	//UsartPrintf(USART1, "5.MQTTCLIENTID\r\n");
	while(ESP8266_SendCmd(MQTTCLIENTID, "OK"));
	//HAL_Delay(10000);	
	//UsartPrintf(USART1, "6.MQTTCONN\r\n");
	while(ESP8266_SendCmd("AT+MQTTCONN=0,\"iot-06z00gcfhg1204h.mqtt.iothub.aliyuncs.com\",1883,1\r\n", "OK"));
	//HAL_Delay(500);	

	
}
void ESP01S_Init(void)
{

	ESP01S_Start();
}



void ESP_IRQHandler(void)
{
		if(DataPointer>=DataSize) {DataPointer=0;memset(RxData,0,DataSize);}
		HAL_UART_Receive_IT(&huart1, (uint8_t*)&RxData[DataPointer++], 1);  
}
