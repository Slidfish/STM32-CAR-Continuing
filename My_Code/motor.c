#include "motor.h"


void motor_init(void){
    //电机使能
    HAL_TIM_PWM_Start(RUN_HTIM, RUN_CHANNEL_A1);
		HAL_TIM_PWM_Start(RUN_HTIM, RUN_CHANNEL_A2);

		
		__HAL_TIM_SetCompare(RUN_HTIM, RUN_CHANNEL_A1, 0);    //修改比较值，修改占空比
		__HAL_TIM_SetCompare(RUN_HTIM, RUN_CHANNEL_A2, 0);    //修改比较值，修改占空比

}

void set_motor_pwm(uint16_t pwmA1,uint16_t pwmA2) //A2向前，A1向后
{
		uint16_t p;
		if((pwmA1>0)&&(pwmA2>0))
		{
			pwmA1=0;
			pwmA2=0;
		}
		if(pwmA1>MOTOR_PWM_MAX)
			pwmA1=MOTOR_PWM_MAX;
		p=pwmA1;
		__HAL_TIM_SetCompare(RUN_HTIM, RUN_CHANNEL_A1, p);    //修改比较值，修改占空比
		if(pwmA2>MOTOR_PWM_MAX)
			pwmA2=MOTOR_PWM_MAX;
		p=pwmA2;
		__HAL_TIM_SetCompare(RUN_HTIM, RUN_CHANNEL_A2, p);    //修改比较值，修改占空比
}
