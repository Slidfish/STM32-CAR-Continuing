/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
//#include "debug.h"
#include "eeprom.h"
#include "servo.h"
#include "motor.h"
#include "lcd.h"
#include "menu.h"
#include "esp01s.h"
#include "encoderM.h"



/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
#define DataSize 512

int32_t Cx=IMAGE_X_CENTRAL;
int32_t Cy=IMAGE_Y_CENTRAL;
int32_t Cz=0;     
static uint16_t encoder_counter = 0;
static int32_t RxBuffer1[6] = {0};  
static u8 RxCounter1 = 0;
static u8 encoder_ok=0;
extern uint16_t servo_ok_1;
extern uint16_t servo_ok_2;
extern uint16_t servo_up_pwm;
extern uint16_t servo_circle_pwm;
extern uint16_t servo_run_pwm;
extern uint32_t Turn_Up_PIDparam_kp;
extern uint32_t Turn_Up_PIDparam_kd;
extern uint32_t Turn_Circle_PIDparam_kp;
extern uint32_t Turn_Circle_PIDparam_kd;
extern uint32_t Turn_Run_PIDparam_kp;
extern uint32_t Turn_Run_PIDparam_kd;
extern PIDparam_st Turn_Up_PIDparam;
extern PIDparam_st Turn_Circle_PIDparam;
extern PIDparam_st Turn_Run_PIDparam;
extern char RxData[DataSize];
extern VelocityData velo;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM3_Init();
  MX_USART1_UART_Init();
  MX_TIM2_Init();
  MX_USART2_UART_Init();
  MX_TIM5_Init();
  /* USER CODE BEGIN 2 */
		servo_init();
		motor_init();
		pidTurn_init();
		HAL_TIM_Base_Start_IT(&htim2);//10ms
		HAL_TIM_Base_Start_IT(&htim3);//20ms

		HAL_UART_Receive_IT(&huart1, (uint8_t*)&RxData[DataSize], 1);  
		HAL_UART_Receive_IT(&huart2, (uint8_t*)&RxBuffer1[RxCounter1], 1);  
		//ESP01S_Init();


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	Encoder_Init();
	LCD_Init();
	LCD_Fill(0,0,LCD_W,LCD_H,WHITE);
	MainMenu_Set();
	
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */


  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	   if(htim->Instance==ENCODER_TIM.Instance)//编码器输入定时器溢出中断，用于防溢出                   
    {      
        if(COUNTERNUM < 10000) velo.overflowNum++;       //如果是向上溢出
        else if(COUNTERNUM >= 10000) velo.overflowNum--; //如果是向下溢出
        __HAL_TIM_SetCounter(&ENCODER_TIM, 10000);             //重新设定初始值
    }

    if (htim == (&htim2))
    {
        key_IRQHandler();
				if(encoder_counter++>10) 		//100ms即1s计算10次速度
				{
					encoder_counter=0;
					encoder_getVelocity();
					//velo.direct = __HAL_TIM_IS_TIM_COUNTING_DOWN(&ENCODER_TIM);//如果向上计数（正转），返回值为0，否则返回值为1
					//encoder1.totalCount = COUNTERNUM + encoder1.overflowNum * RELOADVALUE;//一个周期内的总计数值等于目前计数值加上溢出的计数值
					//encoder1.speed = (float)(encoder1.totalCount - encoder1.lastCount) / (4 * MOTOR_SPEED_RERATIO * PULSE_PRE_ROUND) * 10;//算得每秒多少转
					//encoder1.speed = (float)(encoder1.totalCount - encoder1.lastCount) / (4 * MOTOR_SPEED_RERATIO * PULSE_PRE_ROUND) * 10 * LINE_SPEED_C//算得车轮线速度每秒多少毫米
					//encoder1.lastCount = encoder1.totalCount; //记录这一次的计数值
					
				}
				
    }
		if (htim == (&htim3))
		{
			Turn_Up_PIDparam.kp = Turn_Up_PIDparam_kp;
			Turn_Up_PIDparam.kd = Turn_Up_PIDparam_kd;
			Turn_Circle_PIDparam.kp = Turn_Circle_PIDparam_kp;
			Turn_Circle_PIDparam.kd = Turn_Circle_PIDparam_kd;
			Turn_Run_PIDparam.kp = Turn_Run_PIDparam_kp;
			Turn_Run_PIDparam.kd = Turn_Run_PIDparam_kd;
			if(servo_ok_1){
				if((Cx!=0)&&(Cy!=0)){
					//printf("cxd:%d  %d\n",pidTurn_getDutyX(Cx-IMAGE_X_CENTRAL),Cx-IMAGE_X_CENTRAL);

					if(pidTurn_getDutyX(Cx)>10)
						servo_circle_pwm -=10;
					else if(pidTurn_getDutyX(Cx)<-10)
						servo_circle_pwm +=10;
					else
						servo_circle_pwm -= pidTurn_getDutyX(Cx);
				
					if(pidTurn_getDutyY(Cy)>10)
						servo_up_pwm+=10;
					else if(pidTurn_getDutyY(Cy)<-10)
						servo_up_pwm -=10;
					else
						servo_up_pwm+= pidTurn_getDutyY(Cy);
				
							set_servo_pwm(SERVO_UP_CHANNEL,(uint32_t*)&servo_up_pwm);
							set_servo_pwm(SERVO_CIRCLE_CHANNEL,(uint32_t*)&servo_circle_pwm);
				}	}
			else if(servo_ok_2){
					if(pidTurn_getDutyR(Cx)>10)
						servo_run_pwm +=10;
					else if(pidTurn_getDutyR(Cx)<-10)
						servo_run_pwm -=10;
					else
						servo_run_pwm += pidTurn_getDutyR(Cx);
					set_servo_pwm(SERVO_RUN_CHANNEL,(uint32_t*)&servo_run_pwm);
				
			}
		}
	
}
int8_t uint8_to_int8(uint8_t value) {  
    if (value > 127) {  
        return (int8_t)(value - 256);  // 转换大于 127 的值为负数  
    }  
    return (int8_t)value;  // 直接返回  
} 
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {  
    static u8 RxState = 0;  
		if (huart == (&huart1)){
			ESP_IRQHandler();

			
		}
    if (huart == (&huart2)) { // 接收中断  
        // 在这里处理接收到的数据  
        // 由于调用 HAL_UART_Receive_IT() 可能导致数据被覆盖，需妥善处理  

        if (RxState == 0 && RxBuffer1[0] == 0x2C) { // 0x2c 为帧头  
            RxState = 1;  
        } else if (RxState == 1 && RxBuffer1[1] == 0x12) { // 0x12 为帧头  
            RxState = 2;  

        } else if (RxState == 2) {  
            // 检查最后一位是否为 0x5B 

					
            if (RxCounter1==6 && RxBuffer1[5] == 0x5B) {  
                Cx = uint8_to_int8(RxBuffer1[2]);  
                Cy = uint8_to_int8(RxBuffer1[3]);  
                Cz = uint8_to_int8(RxBuffer1[4]);  
                // 数据接收完成，可以处理数据  
				
                // 重置  
                RxCounter1 = 0;  
                RxState = 0;	  
            } else if(RxCounter1 > 6){ // 接收异常  
                RxState = 0;  
								RxCounter1 = 0;  
								for (u8 i = 0; i < 6; i++) {  
										RxBuffer1[i] = 0x00; // 将存放数据数组清零  
								}  
            }  
        } else { // 接收异常  
            RxState = 0;  
						RxCounter1 = 0;  
						for (u8 i = 0; i < 6; i++) {  
								RxBuffer1[i] = 0x00; // 将存放数据数组清零  
						}  
        }  

        // 继续接收下一个字节  
        HAL_UART_Receive_IT(&huart2, (uint8_t*)&RxBuffer1[RxCounter1], 1);  
        RxCounter1++; // 增加计数  

    }  
}  


/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
