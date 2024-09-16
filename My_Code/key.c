#include "key.h"

volatile KEY keymsg = {KEY_MAX, KEY_UP};  //按键消息结构体
//运行 LCD 确认 向右 向左 向下 向上

GPIO_TypeDef* KEY_PORTn[KEY_MAX] = {MID_GPIO_Port, RHT_GPIO_Port,LFT_GPIO_Port , DWN_GPIO_Port,UP_GPIO_Port};//新板子
uint16_t KEY_PINn[KEY_MAX] = {MID_Pin, RHT_Pin,LFT_Pin , DWN_Pin,UP_Pin};
KEY                 key_msg[KEY_MSG_QUEUE_SIZE];             //按键消息队列
volatile uint8_t      key_msg_front = 0, key_msg_rear = 0;    //接收队列的指针
volatile uint8_t      key_msg_flag = KEY_MSG_EMPTY;           //按键消息队列状态

KEY_STATUS key_get(KEY_ID key)
{
    if(HAL_GPIO_ReadPin(KEY_PORTn[key],KEY_PINn[key]) == KEY_DOWN)
    {
        return KEY_DOWN;
    }
    return KEY_UP;
}
KEY_STATUS key_check(KEY_ID key)
{
    if(key_get(key) == KEY_DOWN)
    {
        HAL_Delay(KEY_DOWN_TIME);
        if(key_get(key) == KEY_DOWN)
        {
            return KEY_DOWN;
        }
    }
    return KEY_UP;
}
/*********************  如下代码是实现按键定时扫描，发送消息到队列  ********************/


//向消息队列存入
void add_key_msg(KEY keymsg)
{
    uint8_t tmp;
    //保存在队列里
    if(key_msg_flag == KEY_MSG_FULL)
    {
        //满了直接不处理
        return ;
    }
    key_msg[key_msg_rear].key = keymsg.key;
    key_msg[key_msg_rear].status = keymsg.status;

    key_msg_rear++;

    if(key_msg_rear >= KEY_MSG_QUEUE_SIZE)
    {
        key_msg_rear = 0;                       //重头开始
    }

    tmp = key_msg_rear;
    if(tmp == key_msg_front)                   //追到屁股了，满了
    {
        key_msg_flag = KEY_MSG_FULL;
    }
    else
    {
        key_msg_flag = KEY_MSG_NORMAL;
    }
}
//从消息队列头取出
uint8_t get_key_msg(volatile KEY *keymsg)
{
    uint8_t tmp;

    if(key_msg_flag == KEY_MSG_EMPTY)               //按键消息队列为空，直接返回0
    {
        return 0;
    }
    /**********************出队**********************/
    keymsg->key = key_msg[key_msg_front].key;       //从队列队首中获取按键值
    keymsg->status = key_msg[key_msg_front].status; //从队列队首中获取按键类型

    key_msg_front++;                                //队列队首指针加1，指向下一个消息
    /************************************************/

    if(key_msg_front >= KEY_MSG_QUEUE_SIZE)          //队列指针队首溢出则从0开始计数
    {
        key_msg_front = 0;                          //重头开始计数（循环利用数组）
    }

    tmp = key_msg_rear;
    if(key_msg_front == tmp)                        //比较队首和队尾是否一样，一样则表示队列已空了
    {
        key_msg_flag = KEY_MSG_EMPTY;
    }
    else
    {
        key_msg_flag = KEY_MSG_NORMAL;
    }

    return 1;
}

//此函数需要放入 定时中断服务函数里，定时10ms执行一次
void key_IRQHandler(void)
{
    KEY_ID   keynum;
    static uint8_t keytime[KEY_MAX] = {0};                          //静态数组，保存各数组按下时间
                                      //按键消息

    for(keynum = (KEY_ID)0 ; keynum < KEY_MAX; keynum ++)    //每个按键轮询
    {
        if(key_get(keynum) == KEY_DOWN)                     //判断按键是否按下
        {
            keytime[keynum]++;                              //按下时间累加

            if(keytime[keynum] <= KEY_DOWN_TIME)            //判断时间是否没超过消抖确认按下时间
            {
                continue;                                   //没达到，则继续等待
            }
            else if(keytime[keynum] == (KEY_DOWN_TIME + 1) )  //判断时间是否为消抖确认按下时间
            {
                //确认按键按下
                keymsg.key = keynum;
                keymsg.status = KEY_DOWN;
                add_key_msg(keymsg);                       //把按键值和按键类型发送消息到队列
            }
            else if(keytime[keynum] == (KEY_HOLD_TIME + 1) )  //判断时间是否为长按
            {
                //确认按键按下
                keymsg.key = keynum;
                keymsg.status = KEY_HOLD;
                add_key_msg(keymsg);                       //把按键值和按键类型发送消息到队列
            }
            else if((keytime[keynum]-KEY_HOLD_TIME - 1) %(KEY_HOLD_TIME - KEY_DOWN_TIME)==0 )  //一直长按
            {
                //确认按键按下
                keymsg.key = keynum;
                keymsg.status = KEY_HOLD;
                add_key_msg(keymsg);                       //把按键值和按键类型发送消息到队列
            }
            if(keytime[keynum]+1>255)keytime[keynum]=(KEY_HOLD_TIME + 1);//hold情况下防止超出uint8_t大小
        }

        else
        {
            if(keytime[keynum] > KEY_DOWN_TIME)             //如果确认过按下按键
            {
                keymsg.key = keynum;
                keymsg.status = KEY_UP;
                add_key_msg(keymsg);                       //发送按键弹起消息
            }

            keytime[keynum] = 0;                            //时间累计清0
        }
    }
}



