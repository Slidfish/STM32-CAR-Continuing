#ifndef CODE_PID_H_
#define CODE_PID_H_
#include "main.h"
typedef struct
{
    float derivative;
    float integral;

    float error;
    float preError; //Error[-1]
    float prePreError; //Error[-2]
    float output;
} PIDcal_st;
typedef struct
{
    float kp;
    float ki;
    float kd;
    float T;
    float Coeff;
} PIDparam_st;
void PID_Cal_Init(PIDcal_st *sptr);


#endif /* CODE_PID_H_ */
