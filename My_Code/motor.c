#include "motor.h"


void motor_init(void){
    //���ʹ��
    HAL_TIM_PWM_Start(RUN_HTIM, RUN_CHANNEL_A1);
		HAL_TIM_PWM_Start(RUN_HTIM, RUN_CHANNEL_A2);

		
		__HAL_TIM_SetCompare(RUN_HTIM, RUN_CHANNEL_A1, 0);    //�޸ıȽ�ֵ���޸�ռ�ձ�
		__HAL_TIM_SetCompare(RUN_HTIM, RUN_CHANNEL_A2, 0);    //�޸ıȽ�ֵ���޸�ռ�ձ�

}

void set_motor_pwm(uint16_t pwmA1,uint16_t pwmA2) //A2��ǰ��A1���
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
		__HAL_TIM_SetCompare(RUN_HTIM, RUN_CHANNEL_A1, p);    //�޸ıȽ�ֵ���޸�ռ�ձ�
		if(pwmA2>MOTOR_PWM_MAX)
			pwmA2=MOTOR_PWM_MAX;
		p=pwmA2;
		__HAL_TIM_SetCompare(RUN_HTIM, RUN_CHANNEL_A2, p);    //�޸ıȽ�ֵ���޸�ռ�ձ�
}
