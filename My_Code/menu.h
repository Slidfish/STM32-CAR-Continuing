#ifndef CODE_MENU_H_
#define  CODE_MENU_H_
#include "main.h"
#include "lcd.h"
#define int8 int8_t
#define uint16 uint16_t
#define uint32 uint32_t
#define int32 int32_t


#define PAGE_DISP_NUM 7   //�˵���ʾ���
#define EnumNameLenth 4   //ö�����Ƶĳ���
#define EnumNameNum   6  //ö�����鳤��

#define RGB565_WHITE   WHITE
#define RGB565_RED 		RED
#define RGB565_BLUE    BLUE
#define tft180_full(color) 	LCD_Fill(0,0,LCD_W,LCD_H,(color))
#define tft180_show_int(x,y,num,length)  LCD_ShowIntNum((x), (y), (num), (length),BLACK,WHITE,16)
#define m_tft180_show_string(x,y,str,cr,cb)     LCD_ShowString((x), (y), ((u8*)str),(cr), (cb),16,0)
#define m_tft180_show_uint(x,y,num,n,cr,cb)     LCD_ShowIntNum((x), (y), (num), (n),(cr), (cb),16)
#define m_tft180_show_int(x,y,num,n,cr,cb)      LCD_ShowIntNum((x), (y), (num), (n),(cr), (cb),16)


struct MENU_TABLE;//ǰ��������MenuParamʹ��

//�˵���������
typedef enum {
    Para_uint, //����������
    Param_Int, //�з��Ų���
    Enumerate, //ö������
    Sub_Menus, //�Ӳ˵�
    Functions, //������������
    Type_Null, //������
}MenuType;

typedef union
{
    uint32_t      *           UINT32; //�޷��Ų�����ַ
    int32_t       *           INT32;  //�з��Ų�����ַ
    struct MENU_TABLE * SubMenu;//�Ӳ˵���ַ

}MenuParam;
//�˵����Ӳ���
typedef union {
   void    (*ItemFunc) (void); // Ҫ���еĲ˵�����
   uint8_t   * EnumName;         // ö�ٱ�������
   uint8_t   SubMenuNum;         // �Ӳ˵�����
}Item;


// �˵�ִ��
typedef struct MENU_TABLE{
    uint8_t   *           MenuName;      // �˵���Ŀ����
    MenuParam   MenuParams;    // Ҫ���ԵĿɸ��Ĳ���
    MenuType    MenuType;      // �˲˵���������
    Item        ItemHook;      // ���Ӳ��� ͬʱ���еĺ��� ö�ٱ������� �Ӳ˵�����
}MENU_TABLE;

typedef struct{
    uint8_t ExitMark;     // �˳��˵�(0-���˳���1-�˳�)
    uint8_t Cursor;       // ���ֵ(��ǰ���λ��)
    uint8_t PageNo;       // �˵�ҳ(��ʾ��ʼ��)
    uint8_t Index;        // �˵�����(��ǰѡ��Ĳ˵���)
    uint8_t DispNum;      // ��ʾ����(ÿҳ�������ڲ˵���)
    uint8_t MaxPage;      // ���ҳ��(����ж�������ʾҳ)
}MENU_PRMT;      // �˵�����

typedef struct {
        uint8_t x;
        uint8_t y;
}Site_t;

void MainMenu_Set(void);

#endif
