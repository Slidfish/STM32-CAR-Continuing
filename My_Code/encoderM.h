#ifndef _ENCODER_H_
#define _ENCODER_H_

#include "main.h"
#include "tim.h"

//电机1的编码器输入引脚
#define MOTO1_ENCODER1_PORT ENCODERA_GPIO_Port
#define MOTO1_ENCODER1_PIN  ENCODERA_Pin
#define MOTO1_ENCODER2_PORT ENCODERB_GPIO_Port
#define MOTO1_ENCODER2_PIN  ENCODERB_Pin

//定时器号
#define ENCODER_TIM htim5

#define MOTOR_SPEED_RERATIO 34u    //电机减速比
#define PULSE_PRE_ROUND 12 //一圈多少个脉冲
#define RADIUS_OF_TYRE 30 //轮胎半径，单位毫米
#define LINE_SPEED_C RADIUS_OF_TYRE * 2 * 3.14
#define RELOADVALUE __HAL_TIM_GetAutoreload(&ENCODER_TIM)    //获取自动装载值,本例中为20000
#define COUNTERNUM __HAL_TIM_GetCounter(&ENCODER_TIM)        //获取编码器定时器中的计数值
typedef struct VelocityData
{
    float enc[3];               //编码器窗口(滤波后)
    float encNow;               //编码器当前值,与encL[0]不同,encL[0]是滤波后的

    float nowVel;                //当前前进速度

    float aimSpeed;               //目标速度
    float preAimSpeed;
    float prepreAimSpeed;

    int duty;                  //电机应给duty
		int overflowNum;
		int direct;
	  int32_t lastCount;   //上一次计数值
    int32_t totalCount;  //总计数值
} VelocityData;


void Encoder_Init(void);
void filtering(int choice);
int kalman_filter(int nowSpeed_Value);
void encoder_getVelocity();

#endif
