#ifndef CODE_BALANCE_SERVO_H_
#define CODE_BALANCE_SERVO_H_
#include "main.h"
#include "tim.h"
#include "pid.h"

#define SERVO_HTIM         &htim3
#define SERVO_RUN_CHANNEL   		TIM_CHANNEL_4
#define SERVO_UP_CHANNEL   		TIM_CHANNEL_2
#define SERVO_CIRCLE_CHANNEL   	TIM_CHANNEL_1
#define SERVO_UP_MAX       750 //小上大下
#define SERVO_UP_MID       600
#define SERVO_UP_MIN 			350
#define SERVO_CIRCLE_MAX 	700 //小左大右
#define SERVO_CIRCLE_MID 	570
#define SERVO_CIRCLE_MIN 	0
#define SERVO_RUN_MAX       750 //
#define SERVO_RUN_MID       600
#define SERVO_RUN_MIN 			 350

//------------------卡曼滤波部分------------------------
  struct KFPTypeS
 {
     float P;
     float G;
     float Q;
     float R;
     float Output;
 };
 typedef struct KFPTypeS KFPTypeS_Struct;
 typedef KFPTypeS_Struct *KFPType_Struct;

 float KalmanFilter(KFPType_Struct kfp, float input);
 //------------------卡曼滤波部分------------------------


void servo_init(void);
void set_servo_pwm(uint32_t Channel,uint32_t *pwm);
void pidTurn_init(void);
int pidTurn_getDutyX(float devx);
int pidTurn_getDutyY(float devy);
int pidTurn_getDutyR(float devr);


#endif /* CODE_BALANCE_SERVO_H_ */
