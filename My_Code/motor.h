#ifndef CODE_MOTOR_H_
#define CODE_MOTOR_H_
#include "main.h"
#include "tim.h"
#include "pid.h"

#define RUN_HTIM         &htim2
#define RUN_CHANNEL_A1   		TIM_CHANNEL_4
#define RUN_CHANNEL_A2   		TIM_CHANNEL_3

#define MOTOR_PWM_MAX   		0.7*7200

//电机使能
#define DISABLE_MOTOR    {     \
    __HAL_TIM_SetCompare(RUN_HTIM, RUN_CHANNEL_A1, 0); \
	  __HAL_TIM_SetCompare(RUN_HTIM, RUN_CHANNEL_A2, 0); \
}
void motor_init(void);
void set_motor_pwm(uint16_t pwmA1,uint16_t pwmA2);




#endif /* CODE_MOTOR_H_ */

