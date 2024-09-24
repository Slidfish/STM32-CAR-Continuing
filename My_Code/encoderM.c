#include "encoderM.h"
#include "math.h"

#define PI                  ( 3.1415926535898 )
#define CHANGE_LIMIT 6
#define CHANGE_MAX_MAX 150

VelocityData velo;
void Encoder_Init(void)
{
    HAL_TIM_Encoder_Start(&ENCODER_TIM, TIM_CHANNEL_ALL);      //开启编码器定时器
    __HAL_TIM_ENABLE_IT(&ENCODER_TIM,TIM_IT_UPDATE);           //开启编码器定时器更新中断,防溢出处理
    __HAL_TIM_SET_COUNTER(&ENCODER_TIM, 10000);                //编码器定时器初始值设定为10000
    velo.enc[0] = 0;
    velo.enc[1] = 0;
    velo.enc[2] = 0;

    velo.encNow=0;

    velo.aimSpeed=0;
    velo.preAimSpeed=0;
    velo.prepreAimSpeed=0;

    velo.duty = 0;
		velo.overflowNum = 0;
		velo.direct=0;
		velo.totalCount = 0;
	  velo.lastCount = 0;
}

void filtering(int choice){
    switch(choice){
        case 1:{//均值滤波
            velo.enc[0] = (velo.encNow + velo.enc[1] + velo.enc[2]) / 3;
            break;}
        case 2:{//低通滤波
            float cutoffFreq = 10, Ts = 0.01;
            float a = 1/(1+Ts * 2 * PI * cutoffFreq);
            a = 0.1;
            velo.enc[0] = a*velo.enc[0] + (1-a)*velo.encNow;
            break;}
        case 4:{//卡曼滤波
            velo.enc[0] = kalman_filter(velo.encNow);
            break;}
        default:{
            velo.enc[0] = velo.encNow;
            break;}//不滤波
    }
}
int kalman_filter(int nowSpeed_Value)
{
    float x_k1_k1,x_k_k1;
    static float lastSpeed_L;
    float Z_k;
    static float P_k1_k1_L;

    static float Q_L = 0.0001;        //Q：规程噪声，Q增大，动态响应变快，收敛稳定性变坏
    static float R_L = 0.005;         //R：测试噪声，R增大，动态响应变慢，收敛稳定性变好
    static float Kg_L = 0;
    static float P_k_k1_L = 1;

    float kalman_speed;
    static float kalman_speed_old_L=0;
    Z_k = (float)nowSpeed_Value;
    x_k1_k1 = kalman_speed_old_L;

    x_k_k1 = x_k1_k1;
    P_k_k1_L = P_k1_k1_L + Q_L;

    Kg_L = P_k_k1_L/(P_k_k1_L + R_L);

    kalman_speed = x_k_k1 + Kg_L * (Z_k - kalman_speed_old_L);
    P_k1_k1_L = (1 - Kg_L)*P_k_k1_L;
    P_k_k1_L = P_k1_k1_L;

    lastSpeed_L = (float)nowSpeed_Value;
    kalman_speed_old_L = kalman_speed;

    return kalman_speed;
}

//获取前进方向速度与横向速度
void encoder_getVelocity(){
    velo.enc[2] = velo.enc[1];
    velo.enc[1] = velo.enc[0];
		velo.direct = __HAL_TIM_IS_TIM_COUNTING_DOWN(&ENCODER_TIM);//如果向上计数（正转），返回值为0，否则返回值为1
	velo.totalCount = COUNTERNUM + velo.overflowNum * RELOADVALUE;//一个周期内的总计数值等于目前计数值加上溢出的计数值
  velo.encNow = (float)(velo.totalCount - velo.lastCount) / (4 * MOTOR_SPEED_RERATIO * PULSE_PRE_ROUND) * 10;//算得每秒多少转
  velo.lastCount = velo.totalCount; //记录这一次的计数值
  
	
	if (fabs((velo.encNow*10)) > CHANGE_MAX_MAX)
			velo.encNow = velo.enc[0];
	if ((velo.encNow - velo.enc[0]) > CHANGE_LIMIT)
			velo.encNow += CHANGE_LIMIT;
	else if ((velo.encNow - velo.enc[0]) < ((-1.0) * CHANGE_LIMIT))
			velo.encNow -= CHANGE_LIMIT;

	filtering(1);

    //filtering(4);

}
