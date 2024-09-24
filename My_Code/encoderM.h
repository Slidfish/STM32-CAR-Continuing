#ifndef _ENCODER_H_
#define _ENCODER_H_

#include "main.h"
#include "tim.h"

//���1�ı�������������
#define MOTO1_ENCODER1_PORT ENCODERA_GPIO_Port
#define MOTO1_ENCODER1_PIN  ENCODERA_Pin
#define MOTO1_ENCODER2_PORT ENCODERB_GPIO_Port
#define MOTO1_ENCODER2_PIN  ENCODERB_Pin

//��ʱ����
#define ENCODER_TIM htim5

#define MOTOR_SPEED_RERATIO 34u    //������ٱ�
#define PULSE_PRE_ROUND 12 //һȦ���ٸ�����
#define RADIUS_OF_TYRE 30 //��̥�뾶����λ����
#define LINE_SPEED_C RADIUS_OF_TYRE * 2 * 3.14
#define RELOADVALUE __HAL_TIM_GetAutoreload(&ENCODER_TIM)    //��ȡ�Զ�װ��ֵ,������Ϊ20000
#define COUNTERNUM __HAL_TIM_GetCounter(&ENCODER_TIM)        //��ȡ��������ʱ���еļ���ֵ
typedef struct VelocityData
{
    float enc[3];               //����������(�˲���)
    float encNow;               //��������ǰֵ,��encL[0]��ͬ,encL[0]���˲����

    float nowVel;                //��ǰǰ���ٶ�

    float aimSpeed;               //Ŀ���ٶ�
    float preAimSpeed;
    float prepreAimSpeed;

    int duty;                  //���Ӧ��duty
		int overflowNum;
		int direct;
	  int32_t lastCount;   //��һ�μ���ֵ
    int32_t totalCount;  //�ܼ���ֵ
} VelocityData;


void Encoder_Init(void);
void filtering(int choice);
int kalman_filter(int nowSpeed_Value);
void encoder_getVelocity();

#endif
