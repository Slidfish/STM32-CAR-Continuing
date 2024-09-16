#ifndef CODE_MENU_H_
#define  CODE_MENU_H_
#include "main.h"
#include "lcd.h"
#define int8 int8_t
#define uint16 uint16_t
#define uint32 uint32_t
#define int32 int32_t


#define PAGE_DISP_NUM 7   //菜单显示最多
#define EnumNameLenth 4   //枚举名称的长度
#define EnumNameNum   6  //枚举数组长度

#define RGB565_WHITE   WHITE
#define RGB565_RED 		RED
#define RGB565_BLUE    BLUE
#define tft180_full(color) 	LCD_Fill(0,0,LCD_W,LCD_H,(color))
#define tft180_show_int(x,y,num,length)  LCD_ShowIntNum((x), (y), (num), (length),BLACK,WHITE,16)
#define m_tft180_show_string(x,y,str,cr,cb)     LCD_ShowString((x), (y), ((u8*)str),(cr), (cb),16,0)
#define m_tft180_show_uint(x,y,num,n,cr,cb)     LCD_ShowIntNum((x), (y), (num), (n),(cr), (cb),16)
#define m_tft180_show_int(x,y,num,n,cr,cb)      LCD_ShowIntNum((x), (y), (num), (n),(cr), (cb),16)


struct MENU_TABLE;//前向声明供MenuParam使用

//菜单任务类型
typedef enum {
    Para_uint, //正整数参数
    Param_Int, //有符号参数
    Enumerate, //枚举类型
    Sub_Menus, //子菜单
    Functions, //触发函数功能
    Type_Null, //无类型
}MenuType;

typedef union
{
    uint32_t      *           UINT32; //无符号参数地址
    int32_t       *           INT32;  //有符号参数地址
    struct MENU_TABLE * SubMenu;//子菜单地址

}MenuParam;
//菜单附加参数
typedef union {
   void    (*ItemFunc) (void); // 要运行的菜单函数
   uint8_t   * EnumName;         // 枚举变量名称
   uint8_t   SubMenuNum;         // 子菜单项数
}Item;


// 菜单执行
typedef struct MENU_TABLE{
    uint8_t   *           MenuName;      // 菜单项目名称
    MenuParam   MenuParams;    // 要调试的可更改参数
    MenuType    MenuType;      // 此菜单任务类型
    Item        ItemHook;      // 附加参数 同时运行的函数 枚举变量名称 子菜单项数
}MENU_TABLE;

typedef struct{
    uint8_t ExitMark;     // 退出菜单(0-不退出，1-退出)
    uint8_t Cursor;       // 光标值(当前光标位置)
    uint8_t PageNo;       // 菜单页(显示开始项)
    uint8_t Index;        // 菜单索引(当前选择的菜单项)
    uint8_t DispNum;      // 显示项数(每页可以现在菜单项)
    uint8_t MaxPage;      // 最大页数(最大有多少种显示页)
}MENU_PRMT;      // 菜单参数

typedef struct {
        uint8_t x;
        uint8_t y;
}Site_t;

void MainMenu_Set(void);

#endif
