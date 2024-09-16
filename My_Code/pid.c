#include "pid.h"

void PID_Cal_Init(PIDcal_st *sptr)
{
    sptr->error = 0;
    sptr->preError = 0;
    sptr->prePreError = 0;
    sptr->derivative = 0;
    sptr->integral = 0;
    sptr->output = 0;
}