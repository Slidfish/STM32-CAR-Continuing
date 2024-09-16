#ifndef CODE_KEY_H_
#define CODE_KEY_H_
#include "main.h"

//下面是定义按键的时间，单位为 ： 10ms（中断时间）
#define KEY_DOWN_TIME           10       //消抖确认按下时间
#define KEY_HOLD_TIME           50      //长按hold确认时间，最多253，否则需要修改 keytime 的类型
                                        //如果按键一直按下去，则每隔 KEY_HOLD_TIME - KEY_DOWN_TIME 时间会发送一个 KEY_HOLD 消息

//定义按键消息队列大小
#define KEY_MSG_QUEUE_SIZE       20      //最多 255，否则需要修改key_msg_front/key_msg_rear类型

//按键端口的枚举
typedef enum
{
//    KEY_RUN,  //左

//    KEY_LCD_DISPLAY,  //开始

    KEY_M,  //右

    KEY_R,  //上

    KEY_L,   //停止

    KEY_D,  //下

    KEY_U,  //选择

    KEY_MAX,
} KEY_ID;


//key状态枚举定义
typedef enum
{
    KEY_DOWN  =   0,         //按键按下时对应电平
    KEY_UP    =   1,         //按键弹起时对应电平

    KEY_HOLD,               //长按按键(用于定时按键扫描)

} KEY_STATUS;


//按键消息结构体
typedef struct
{
    KEY_ID           key;        //按键编号
    KEY_STATUS    status;     //按键状态
} KEY;

typedef enum
{
    KEY_MSG_EMPTY,      //没有按键队列
    KEY_MSG_NORMAL,     //正常，有按键队列，但不满
    KEY_MSG_FULL,       //按键消息满
} KEY_QUEUE;


KEY_STATUS    key_get(KEY_ID key);             //检测key状态（不带延时消抖）
KEY_STATUS    key_check(KEY_ID key);           //检测key状态（带延时消抖）
//定时扫描按键
void    add_key_msg(KEY keymsg);
uint8_t   get_key_msg(volatile KEY *keymsg);         //获取按键消息，返回1表示有按键消息，0为无按键消息
void    key_IRQHandler(void);                   //需要定时扫描的中断服务函数（定时时间为10ms）
void    key_check_msg(KEY keymsg);


extern volatile KEY keymsg;
#endif /* CODE_KEY_H_ */
