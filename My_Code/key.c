#include "key.h"

volatile KEY keymsg = {KEY_MAX, KEY_UP};  //������Ϣ�ṹ��
//���� LCD ȷ�� ���� ���� ���� ����

GPIO_TypeDef* KEY_PORTn[KEY_MAX] = {MID_GPIO_Port, RHT_GPIO_Port,LFT_GPIO_Port , DWN_GPIO_Port,UP_GPIO_Port};//�°���
uint16_t KEY_PINn[KEY_MAX] = {MID_Pin, RHT_Pin,LFT_Pin , DWN_Pin,UP_Pin};
KEY                 key_msg[KEY_MSG_QUEUE_SIZE];             //������Ϣ����
volatile uint8_t      key_msg_front = 0, key_msg_rear = 0;    //���ն��е�ָ��
volatile uint8_t      key_msg_flag = KEY_MSG_EMPTY;           //������Ϣ����״̬

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
/*********************  ���´�����ʵ�ְ�����ʱɨ�裬������Ϣ������  ********************/


//����Ϣ���д���
void add_key_msg(KEY keymsg)
{
    uint8_t tmp;
    //�����ڶ�����
    if(key_msg_flag == KEY_MSG_FULL)
    {
        //����ֱ�Ӳ�����
        return ;
    }
    key_msg[key_msg_rear].key = keymsg.key;
    key_msg[key_msg_rear].status = keymsg.status;

    key_msg_rear++;

    if(key_msg_rear >= KEY_MSG_QUEUE_SIZE)
    {
        key_msg_rear = 0;                       //��ͷ��ʼ
    }

    tmp = key_msg_rear;
    if(tmp == key_msg_front)                   //׷��ƨ���ˣ�����
    {
        key_msg_flag = KEY_MSG_FULL;
    }
    else
    {
        key_msg_flag = KEY_MSG_NORMAL;
    }
}
//����Ϣ����ͷȡ��
uint8_t get_key_msg(volatile KEY *keymsg)
{
    uint8_t tmp;

    if(key_msg_flag == KEY_MSG_EMPTY)               //������Ϣ����Ϊ�գ�ֱ�ӷ���0
    {
        return 0;
    }
    /**********************����**********************/
    keymsg->key = key_msg[key_msg_front].key;       //�Ӷ��ж����л�ȡ����ֵ
    keymsg->status = key_msg[key_msg_front].status; //�Ӷ��ж����л�ȡ��������

    key_msg_front++;                                //���ж���ָ���1��ָ����һ����Ϣ
    /************************************************/

    if(key_msg_front >= KEY_MSG_QUEUE_SIZE)          //����ָ�����������0��ʼ����
    {
        key_msg_front = 0;                          //��ͷ��ʼ������ѭ���������飩
    }

    tmp = key_msg_rear;
    if(key_msg_front == tmp)                        //�Ƚ϶��׺Ͷ�β�Ƿ�һ����һ�����ʾ�����ѿ���
    {
        key_msg_flag = KEY_MSG_EMPTY;
    }
    else
    {
        key_msg_flag = KEY_MSG_NORMAL;
    }

    return 1;
}

//�˺�����Ҫ���� ��ʱ�жϷ��������ʱ10msִ��һ��
void key_IRQHandler(void)
{
    KEY_ID   keynum;
    static uint8_t keytime[KEY_MAX] = {0};                          //��̬���飬��������鰴��ʱ��
                                      //������Ϣ

    for(keynum = (KEY_ID)0 ; keynum < KEY_MAX; keynum ++)    //ÿ��������ѯ
    {
        if(key_get(keynum) == KEY_DOWN)                     //�жϰ����Ƿ���
        {
            keytime[keynum]++;                              //����ʱ���ۼ�

            if(keytime[keynum] <= KEY_DOWN_TIME)            //�ж�ʱ���Ƿ�û��������ȷ�ϰ���ʱ��
            {
                continue;                                   //û�ﵽ��������ȴ�
            }
            else if(keytime[keynum] == (KEY_DOWN_TIME + 1) )  //�ж�ʱ���Ƿ�Ϊ����ȷ�ϰ���ʱ��
            {
                //ȷ�ϰ�������
                keymsg.key = keynum;
                keymsg.status = KEY_DOWN;
                add_key_msg(keymsg);                       //�Ѱ���ֵ�Ͱ������ͷ�����Ϣ������
            }
            else if(keytime[keynum] == (KEY_HOLD_TIME + 1) )  //�ж�ʱ���Ƿ�Ϊ����
            {
                //ȷ�ϰ�������
                keymsg.key = keynum;
                keymsg.status = KEY_HOLD;
                add_key_msg(keymsg);                       //�Ѱ���ֵ�Ͱ������ͷ�����Ϣ������
            }
            else if((keytime[keynum]-KEY_HOLD_TIME - 1) %(KEY_HOLD_TIME - KEY_DOWN_TIME)==0 )  //һֱ����
            {
                //ȷ�ϰ�������
                keymsg.key = keynum;
                keymsg.status = KEY_HOLD;
                add_key_msg(keymsg);                       //�Ѱ���ֵ�Ͱ������ͷ�����Ϣ������
            }
            if(keytime[keynum]+1>255)keytime[keynum]=(KEY_HOLD_TIME + 1);//hold����·�ֹ����uint8_t��С
        }

        else
        {
            if(keytime[keynum] > KEY_DOWN_TIME)             //���ȷ�Ϲ����°���
            {
                keymsg.key = keynum;
                keymsg.status = KEY_UP;
                add_key_msg(keymsg);                       //���Ͱ���������Ϣ
            }

            keytime[keynum] = 0;                            //ʱ���ۼ���0
        }
    }
}



