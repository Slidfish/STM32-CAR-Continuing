#ifndef CODE_KEY_H_
#define CODE_KEY_H_
#include "main.h"

//�����Ƕ��尴����ʱ�䣬��λΪ �� 10ms���ж�ʱ�䣩
#define KEY_DOWN_TIME           10       //����ȷ�ϰ���ʱ��
#define KEY_HOLD_TIME           50      //����holdȷ��ʱ�䣬���253��������Ҫ�޸� keytime ������
                                        //�������һֱ����ȥ����ÿ�� KEY_HOLD_TIME - KEY_DOWN_TIME ʱ��ᷢ��һ�� KEY_HOLD ��Ϣ

//���尴����Ϣ���д�С
#define KEY_MSG_QUEUE_SIZE       20      //��� 255��������Ҫ�޸�key_msg_front/key_msg_rear����

//�����˿ڵ�ö��
typedef enum
{
//    KEY_RUN,  //��

//    KEY_LCD_DISPLAY,  //��ʼ

    KEY_M,  //��

    KEY_R,  //��

    KEY_L,   //ֹͣ

    KEY_D,  //��

    KEY_U,  //ѡ��

    KEY_MAX,
} KEY_ID;


//key״̬ö�ٶ���
typedef enum
{
    KEY_DOWN  =   0,         //��������ʱ��Ӧ��ƽ
    KEY_UP    =   1,         //��������ʱ��Ӧ��ƽ

    KEY_HOLD,               //��������(���ڶ�ʱ����ɨ��)

} KEY_STATUS;


//������Ϣ�ṹ��
typedef struct
{
    KEY_ID           key;        //�������
    KEY_STATUS    status;     //����״̬
} KEY;

typedef enum
{
    KEY_MSG_EMPTY,      //û�а�������
    KEY_MSG_NORMAL,     //�������а������У�������
    KEY_MSG_FULL,       //������Ϣ��
} KEY_QUEUE;


KEY_STATUS    key_get(KEY_ID key);             //���key״̬��������ʱ������
KEY_STATUS    key_check(KEY_ID key);           //���key״̬������ʱ������
//��ʱɨ�谴��
void    add_key_msg(KEY keymsg);
uint8_t   get_key_msg(volatile KEY *keymsg);         //��ȡ������Ϣ������1��ʾ�а�����Ϣ��0Ϊ�ް�����Ϣ
void    key_IRQHandler(void);                   //��Ҫ��ʱɨ����жϷ���������ʱʱ��Ϊ10ms��
void    key_check_msg(KEY keymsg);


extern volatile KEY keymsg;
#endif /* CODE_KEY_H_ */
