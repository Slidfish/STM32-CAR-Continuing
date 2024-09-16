#include "servo.h"
#include "debug.h"
PIDcal_st Turn_PIDcal_Up;
PIDcal_st Turn_PIDcal_Circle;
PIDparam_st Turn_Up_PIDparam;
PIDparam_st Turn_Circle_PIDparam;

float tar_turnx = 0;
float tar_turny = 0;

uint32_t servo_up_pwm = SERVO_UP_MID;
uint32_t servo_circle_pwm = SERVO_CIRCLE_MID;
uint32_t servo_run_pwm = SERVO_RUN_MID;


void servo_init()
{
    HAL_TIM_PWM_Start(SERVO_HTIM, SERVO_CIRCLE_CHANNEL);
		HAL_TIM_PWM_Start(SERVO_HTIM, SERVO_UP_CHANNEL);
	  HAL_TIM_PWM_Start(SERVO_HTIM, SERVO_RUN_CHANNEL);
		
		__HAL_TIM_SetCompare(SERVO_HTIM, SERVO_CIRCLE_CHANNEL, SERVO_CIRCLE_MID);    //修改比较值，修改占空比
		__HAL_TIM_SetCompare(SERVO_HTIM, SERVO_UP_CHANNEL, SERVO_UP_MID);    //修改比较值，修改占空比
		__HAL_TIM_SetCompare(SERVO_HTIM, SERVO_RUN_CHANNEL, SERVO_RUN_MID);    //修改比较值，修改占空比


}
/*
Channel:SERVO_CIRCLE_CHANNEL/SERVO_UP_CHANNEL/SERVO_RUN_CHANNEL
*/
void set_servo_pwm(uint32_t Channel,uint32_t* pwm)
{
	uint32_t p;
	switch (Channel)
	{
		case SERVO_CIRCLE_CHANNEL:
			if(*pwm>SERVO_CIRCLE_MAX)
				*pwm=SERVO_CIRCLE_MAX;
			else if(pwm<SERVO_CIRCLE_MIN)
				*pwm=SERVO_CIRCLE_MIN;
		break;
		case SERVO_UP_CHANNEL:
			if(*pwm>SERVO_UP_MAX)
				*pwm=SERVO_UP_MAX;
			else if(*pwm<SERVO_UP_MIN)
				*pwm=SERVO_UP_MIN;
		break;
		case SERVO_RUN_CHANNEL:
			if(*pwm>SERVO_RUN_MAX)
				*pwm=SERVO_RUN_MAX;
			else if(*pwm<SERVO_RUN_MIN)
				*pwm=SERVO_RUN_MIN;
		break;
		
			
	}
		p=*pwm;
			__HAL_TIM_SetCompare(SERVO_HTIM, Channel, p);    //修改比较值，修改占空比
}

void pidTurn_init(){

    PID_Cal_Init(&Turn_PIDcal_Up);

    Turn_Up_PIDparam.kp = 0;
    Turn_Up_PIDparam.ki = 0;
    Turn_Up_PIDparam.kd = 0;
	
	  Turn_Circle_PIDparam.kp = 0;
    Turn_Circle_PIDparam.ki = 0;
    Turn_Circle_PIDparam.kd = 0;

}
float PID_Location2(PIDcal_st *sptr, PIDparam_st PIDparam, float NowData, float Target)
{
    sptr->error = Target - NowData;
    sptr->derivative = (sptr->error - sptr->preError)*0.5 + sptr->derivative * 0.5;

    sptr->integral+= sptr->error;
    sptr->output = sptr->error * (float)PIDparam.kp/100 +
            sptr->derivative * (float)PIDparam.kd/100 +
            sptr->integral*PIDparam.ki/100 ;
    sptr->preError = sptr->error ;
    return sptr->output;
}
int pidTurn_getDutyX(float devx){
    tar_turnx = -1 * PID_Location2(&Turn_PIDcal_Circle, Turn_Circle_PIDparam,devx, IMAGE_X_CENTRAL);
    return tar_turnx;
}
int pidTurn_getDutyY(float devy){
    tar_turny = -1 * PID_Location2(&Turn_PIDcal_Up, Turn_Up_PIDparam,devy, IMAGE_Y_CENTRAL);
    return tar_turny;
}