/*      1.单纯的参数调节               Para_uint, Param_Int
 *          在此类的时候，只需要1>将 MenuParams初始化为{.uint32_t = (uint32_t *)&param}
 *                                 或者{.INT32 = (int32 *)&param}
 *                        2>将 ItemHook 初始化为 {.ItemFunc = Func_Null}
 *                        3>将 MenuType 初始化为Para_uint或者Param_Int
 *      2.参数调节的同时进行函数调用 还是   Para_uint, Param_Int
 *          如舵机中值的调节，电机pwm输出
 *                        1>将 MenuParams初始化为{.uint32_t = (uint32_t *)&param}
 *                                 或者{.INT32 = (int32 *)&param}
 *                        2>将 ItemHook 初始化为 {.ItemFunc = 调用的函数名}
 *                        3>将 MenuType 初始化为Para_uint或者Param_Int
 *      3.枚举类型的显示               Enumerate
 *          这个可以用来进行runmode runstate等控制模式信息
 *          可以把枚举变量存eeprom 要将其强制转化为 (int32 *)
 *          这类要在         1>将 MenuParams初始化为{.uint32_t = (uint32_t *)&param} 或者 {.INT32 = (int32 *)&param}
 *                        2>将 ItemHook 初始化为 {.EnumName = 枚举名称数组名}
 *                        3>将 MenuType 初始化为 Enumerate
 *               这个我在下面写了模板char PointTypeName[EnumNameNum][EnumNameLenth+1]
 *               由于字符串类型在遍历的时候读不到\0 所以我们需要手动加一个 然后长度和宽度在宏定义里写了 字符串长度为6 总数为10 不建议修改 多的用NULL对其就好
 *      4.只有触发函数                 Functions
 *                        1>将 MenuParams 初始化为 {.SubMenu = Table_Null}
 *                        2>将 将ItemHook 初始化为 {.ItemFunc = 调用的函数名}
 *                        3>将 MenuType 初始化为 Functions
 *      5.子菜单                     Sub_Menus
 *                        1>将 MenuParams 初始化为 {.SubMenu = MenuDebug}
 *                        2>将 ItemHook 初始化为   {.SubMenuNum = MenuNum(MenuDebug)} 这个的作用是子菜单的项数
 *                        3>将 MenuType 初始化为 Sub_Menus
 */
//注：1.MainMenu_Set中的Read_EEPROM请在第一次设置参数请注释掉此行，写入后重新开启此语句
//   2.子菜单的名称后应当具有空格
//   3.摄像头新参数需要重启单片机才能生效
#include "menu.h" //引入菜单使用的全局变量
#include "tftOut.h"
#include "servo.h"
#include "eeprom.h"
#include "motor.h"
#include "encoderM.h"

#define         MenuNum(Table)      sizeof(Table)/sizeof(Table[0])
volatile uint8_t ExitMenu_flag = 0;//退出菜单的标志位
#define Type_Null 5
#define Para_uint 0
#define Functions 4
#define Sub_Menus 3
#define Param_Int 1
#define Enumerate 2

extern volatile KEY keymsg;
uint32_t flash_union_buffer[EEPROM_PAGE_LENGTH];                    // FLASH 操作的数据缓冲区

//定义一些空值
#define         Debug_Null          NULL
void            Func_Null(void)     {};
MENU_TABLE  Table_Null[] = {{(uint8_t *)"", {.SubMenu = Table_Null}, Type_Null, {.ItemFunc = Func_Null}},};

//声名一些菜单操作需要使用的功能函数,函数功能见实现处
KEY_ID KeySan(void);
void SubNameCat(uint8_t* SubMenuName,uint8_t *TableMenuName);
void adjustParam(Site_t site, MENU_TABLE *table, uint16_t Color, uint16_t bkColor);
void Write_EEPROM(void);
void Read_EEPROM(void);
void writeFlash(uint8_t n);
void readFlash(uint8_t n);


/****************************************EEPROM_DATA*****************************************/

extern uint32_t servo_up_pwm;
extern uint32_t servo_circle_pwm;
extern uint32_t servo_run_pwm;
extern int32_t Cx,Cy,Cz;
extern VelocityData velo;
     
uint32_t Turn_Up_PIDparam_kp= 1;
uint32_t Turn_Up_PIDparam_kd = 0;
uint32_t Turn_Circle_PIDparam_kp= 1;
uint32_t Turn_Circle_PIDparam_kd = 0;
uint32_t Turn_Run_PIDparam_kp= 1;
uint32_t Turn_Run_PIDparam_kd = 0;

uint32_t *EEPROM_DATA[] =
{
	(uint32_t*)&servo_up_pwm,
	(uint32_t*)&servo_circle_pwm,
	(uint32_t*)&servo_run_pwm,
	(uint32_t*)&Turn_Up_PIDparam_kp,
	(uint32_t*)&Turn_Up_PIDparam_kd,
	(uint32_t*)&Turn_Circle_PIDparam_kp,
	(uint32_t*)&Turn_Circle_PIDparam_kd,
	(uint32_t*)&Turn_Run_PIDparam_kp,
	(uint32_t*)&Turn_Run_PIDparam_kd,
};

/****************************************EEPROM_DATA*****************************************/

uint32_t servo_ok_1=0;
uint32_t servo_ok_2=0;


void SERVO_UP()
{
							set_servo_pwm(SERVO_UP_CHANNEL,&servo_up_pwm);

}

void SERVO_CIRCLE()
{
						set_servo_pwm(SERVO_CIRCLE_CHANNEL,&servo_circle_pwm);

}
void SERVO_RUN()
{
						set_servo_pwm(SERVO_RUN_CHANNEL,&servo_run_pwm);

}

void RECIEVE()
{
	  tft180_full(RGB565_WHITE);

    while(keymsg.key!=KEY_L){
				m_tft180_show_string(0,0,"begin to recieve ",BLACK,WHITE);
				m_tft180_show_int(0,16,Cx,3,BLACK,WHITE);
				m_tft180_show_int(0,32,Cy,3,BLACK,WHITE);
				m_tft180_show_int(0,48,-200,3,BLACK,WHITE);

			

    }
}
/****************************************Basic_Test菜单******************************************/
MENU_TABLE TRACK1[] =
{
        {(uint8_t*)"kp_u", {.UINT32 =(uint32_t*)&Turn_Up_PIDparam_kp},Para_uint,{.ItemFunc = Func_Null}},
        {(uint8_t*)"kd_u", {.UINT32 =(uint32_t*)&Turn_Up_PIDparam_kd},Para_uint,{.ItemFunc = Func_Null}},
        {(uint8_t*)"kp_c", {.UINT32 =(uint32_t*)&Turn_Circle_PIDparam_kp},Para_uint,{.ItemFunc = Func_Null}},        
        {(uint8_t*)"kd_c", {.UINT32 =(uint32_t*)&Turn_Circle_PIDparam_kd},Para_uint,{.ItemFunc = Func_Null}}, 
				{(uint8_t*)"servo_ok_1", {.UINT32 =(uint32_t*)&servo_ok_1},Para_uint,{.ItemFunc = Func_Null}}, 

				
};

MENU_TABLE TRACK2[] =
{
        {(uint8_t*)"kp_r", {.UINT32 =(uint32_t*)&Turn_Run_PIDparam_kp},Para_uint,{.ItemFunc = Func_Null}},
        {(uint8_t*)"kd_r", {.UINT32 =(uint32_t*)&Turn_Run_PIDparam_kd},Para_uint,{.ItemFunc = Func_Null}},
				{(uint8_t*)"servo_ok_2", {.UINT32 =(uint32_t*)&servo_ok_2},Para_uint,{.ItemFunc = Func_Null}}, 

				
};

MENU_TABLE SERVO_PRA[] =
{
        {(uint8_t*)"servo_up_pwm", {.UINT32 =(uint32_t*)&servo_up_pwm},Para_uint,{.ItemFunc = SERVO_UP}},
        {(uint8_t*)"servo_circle_pwm", {.UINT32 =(uint32_t*)&servo_circle_pwm},Para_uint,{.ItemFunc = SERVO_CIRCLE}},
			  {(uint8_t*)"servo_run_pwm", {.UINT32 =(uint32_t*)&servo_run_pwm},Para_uint,{.ItemFunc = SERVO_RUN }},

        
};
void MOTOR(){
    tft180_full(RGB565_WHITE);
    while (keymsg.key != KEY_L)
    {
        if(keymsg.key == KEY_U)
				{
            set_motor_pwm(2000,0);
				}
        else if(keymsg.key == KEY_D)
        {    set_motor_pwm(0,2000);
					
        }
		m_tft180_show_int(0,0,velo.enc[0]*10,4,BLACK,WHITE);

    }
    if(keymsg.key==KEY_L&&keymsg.status==KEY_DOWN) keymsg.status=KEY_UP;
    DISABLE_MOTOR;
    tft180_full(RGB565_WHITE);
}

MENU_TABLE Basic_Test[] =
{
				{(uint8_t *)"1.srv_text       ", {.SubMenu = SERVO_PRA},  Sub_Menus ,  {.SubMenuNum = MenuNum(SERVO_PRA)}},
				{(uint8_t *)"2.Recieve       ", {.SubMenu = Table_Null},  Functions ,  {.ItemFunc = RECIEVE}},
				{(uint8_t *)"3.track1       ", {.SubMenu = TRACK1},  Sub_Menus ,  {.SubMenuNum = MenuNum(TRACK1)}},
				{(uint8_t *)"4.track2       ", {.SubMenu = TRACK2},  Sub_Menus ,  {.SubMenuNum = MenuNum(TRACK2)}},
				{(uint8_t *)"5.motor       ", {.SubMenu = Table_Null},  Functions ,  {.ItemFunc = MOTOR}},

//        {(uint8_t *)"2.motor      ", {.SubMenu = Table_Null},  Functions ,   {.ItemFunc = MOTOR}},
//        {(uint8_t *)"3.img      ", {.SubMenu = Table_Null},  Functions ,   {.ItemFunc = IMG}},
};
/****************************************Basic_Test菜单******************************************/

/****************************************主菜单定义区******************************************/
MENU_TABLE MainMenu_Table[] =
{
    {(uint8_t *)"1.Basic_Test     ",{.SubMenu = Basic_Test},  Sub_Menus ,   {.SubMenuNum = MenuNum(Basic_Test)}},

};
/****************************************主菜单定义区******************************************/

//菜单初始化函数
void Menu_PrmtInit(MENU_PRMT *prmt, uint8_t num, uint8_t page)
{
    prmt->ExitMark = 0; //清除退出菜单标志
    prmt->Cursor = 0;    //光标清零
    prmt->PageNo = 0;    //页清零
    prmt->Index = 0;     //索引清零
    prmt->DispNum = num; //页最多显示项目数
    prmt->MaxPage = page; //最多页数
}

//菜单光标移动函数
uint8_t Menu_Move(MENU_PRMT *prmt, KEY_ID key)
{
    uint8_t rValue = 1;//置零时代表按下确认键
    switch (key)
    {
        case KEY_U: // 向上
        {
            if (prmt->Cursor != 0) // 光标不在顶端
            {
                prmt->Cursor--; // 光标上移
            }
            else // 光标在顶端
            {
                if (prmt->PageNo != 0) // 页面没有到最小
                {
                    prmt->PageNo--; // 向上翻
                }
                else
                {
                    prmt->Cursor = prmt->DispNum - 1; // 光标到底
                    prmt->PageNo = prmt->MaxPage - 1; // 最后页
                }
                tft180_full(RGB565_WHITE);
            }
            break;
        }

        case KEY_D: // 向下
        {
            if (prmt->Cursor < prmt->DispNum - 1) // 光标没有到底，移动光标
            {
                prmt->Cursor++; // 光标向下移动
            }
            else // 光标到底
            {
                if (prmt->PageNo < prmt->MaxPage - 1) // 页面没有到底，页面移动
                {
                    prmt->PageNo++; // 下翻一页
                }
                else // 页面和光标都到底，返回开始页
                {
                    prmt->Cursor = 0;
                    prmt->PageNo = 0;
                }
                tft180_full(RGB565_WHITE);
            }
            break;
        }
        case KEY_M: // 确认
        {
            prmt->Index = prmt->Cursor + prmt->PageNo; //计算执行项的索引,索引为光标加上页码
            rValue = 0;

            break;
        }
        case KEY_L: // 左键返回上级菜单
        {
            prmt->ExitMark = 1;
            break;
        }
        case KEY_R: // 右键跳到底部
        {
            prmt->Cursor = prmt->DispNum - 1; // 光标到底
            prmt->PageNo = prmt->MaxPage - 1; // 最后页
            tft180_full(RGB565_WHITE);
            break;
        }
        default:
            break;
    }
    return rValue; // 返回执行索引
}

//显示菜单函数
void Menu_Display(MENU_TABLE *menuTable, uint8_t pageNo, uint8_t dispNum, uint8_t cursor)
{
    uint8_t i;
    Site_t site;
    for (i = 0; i < dispNum; i++)
    {
        site.x = 0;
        site.y = (i+1)*16;
        if (cursor == i)
            /* 反白显示当前光标选中菜单项 */
            m_tft180_show_string((uint16_t)site.x, (uint16_t)site.y, (const int8 *)menuTable[pageNo + i].MenuName, RGB565_WHITE, RGB565_BLUE);
        else
            /* 正常显示其余菜单项 */
            m_tft180_show_string((uint16_t)site.x, (uint16_t)site.y, (const int8 *)menuTable[pageNo + i].MenuName, RGB565_BLUE, RGB565_WHITE);
        /* 若此菜单项有需要调的参数，则显示该参数 */
        if (menuTable[pageNo + i].MenuType == Para_uint || menuTable[pageNo + i].MenuType == Param_Int || menuTable[pageNo + i].MenuType== Enumerate)
        {
            site.x = 84;
            if (menuTable[pageNo + i].MenuType == Para_uint){
                uint32_t num_t = (*(menuTable[pageNo + i].MenuParams.UINT32));
                m_tft180_show_uint(site.x, site.y, num_t,5, RGB565_RED, RGB565_WHITE);
            }
            else if (menuTable[pageNo + i].MenuType == Param_Int){
                int32_t num_t = (*(menuTable[pageNo + i].MenuParams.INT32));
                m_tft180_show_int(site.x, site.y, num_t,6, RGB565_RED, RGB565_WHITE);
            }
            else if (menuTable[pageNo + i].MenuType == Enumerate){
                uint32_t num_t = (*(menuTable[pageNo + i].MenuParams.UINT32));
                m_tft180_show_string(site.x, site.y,(int8*)(menuTable[pageNo + i].ItemHook.EnumName+num_t*(EnumNameLenth+1)),RGB565_RED,RGB565_WHITE);
                //EnumName是一个指向枚举项名称的字符数组的指针。
                //num_t*(EnumNameLenth+1) 是用于计算当前枚举值在枚举项名称数组中的偏移量
            }
        }
    }
}

//处理菜单函数
void Menu_Process(uint8_t *menuName, MENU_PRMT *prmt, MENU_TABLE *table, uint8_t num)
{
    KEY_ID key;
    Site_t site;
    uint8_t page; //显示菜单需要的页数
    if (num - PAGE_DISP_NUM <= 0)
        page = 1;
    else
    {
        page = num - PAGE_DISP_NUM + 1;//多几个就多需要显示几页
        num = PAGE_DISP_NUM;
    }
    // 显示项数和页数设置
    Menu_PrmtInit(prmt, num, page);
    do
    {
        LCD_ShowString((uint16_t)0, (uint16_t)0, (const uint8_t *)menuName, RGB565_RED, RGB565_WHITE,16,0); //显示菜单标题
        // 显示菜单项
        Menu_Display(table, prmt->PageNo, prmt->DispNum, prmt->Cursor);
        key = KeySan(); //获取按键

        if (Menu_Move(prmt, key) == 0) //菜单移动 按下确认键
        {
            // 判断此菜单项有无需要调节的参数 有则进入参数调节 在参数调节里看有无函数同时运行
            if (table[prmt->Index].MenuType == Para_uint || table[prmt->Index].MenuType == Param_Int || table[prmt->Index].MenuType == Enumerate)
            {
                site.x = 84;
                site.y = (1 + prmt->Cursor)*16;
                if (table[prmt->Index].MenuType == Para_uint)
                    m_tft180_show_uint(site.x, site.y, *(table[prmt->Index].MenuParams.UINT32), 5, RGB565_WHITE, RGB565_RED);
                else if (table[prmt->Index].MenuType == Param_Int)
                    m_tft180_show_int (site.x, site.y, *(table[prmt->Index].MenuParams.INT32), 6, RGB565_WHITE, RGB565_RED);
                else if (table[prmt->Index].MenuType == Enumerate)
                    m_tft180_show_string(site.x, site.y,(int8*)(table[prmt->Index].ItemHook.EnumName+(*(table[prmt->Index].MenuParams.INT32))*EnumNameLenth),RGB565_WHITE, RGB565_RED);
                //在参数调节里看有无函数同时运行  可以同时执行 方便舵机调试，电机调试 这个在上面的调节参数函数里已经执行过
                adjustParam(site, &table[prmt->Index], RGB565_WHITE, RGB565_RED);
            }
            // 不是参数调节的话就执行菜单函数
            else if(table[prmt->Index].MenuType == Functions)
            {
                table[prmt->Index].ItemHook.ItemFunc(); // 执行相应项
            }
            // 没有参数调节和函数执行的话 就是子菜单
            else if(table[prmt->Index].MenuType == Sub_Menus){
                //确定有子菜单
                if (table[prmt->Index].MenuParams.SubMenu != Table_Null){
                    tft180_full(RGB565_WHITE);
                    MENU_PRMT Submenu_Prmt;
                    uint8_t SubMenuName[20];
                    SubNameCat(SubMenuName,table[prmt->Index].MenuName);
                    Menu_Process(SubMenuName, &Submenu_Prmt, table[prmt->Index].MenuParams.SubMenu, table[prmt->Index].ItemHook.SubMenuNum);
                }
            }
        }
    } while (prmt->ExitMark == 0 && ExitMenu_flag == 0);
    Write_EEPROM();//将数据写入EEPROM保存
    tft180_full(RGB565_WHITE);
}

//设置主菜单函数
void MainMenu_Set(void)
{
    ExitMenu_flag = 0;
    tft180_full(RGB565_WHITE);
    MENU_PRMT MainMenu_Prmt;
    uint8_t menuNum = MenuNum(MainMenu_Table); // 菜单项数
    Read_EEPROM();//读取数据（第一次设置参数请注释掉此行，写入后重新开启此语句）
    
    Menu_Process((uint8_t *)" -=  Setting  =- ", &MainMenu_Prmt, MainMenu_Table, menuNum);
    tft180_full(RGB565_WHITE);
}

//----------------------------------以下是供上述菜单操作函数使用的功能函数----------------------------//

//按键获取函数
KEY_ID KeySan(void)
{
    while (keymsg.status == KEY_UP && !ExitMenu_flag);
    keymsg.status = KEY_UP;
    return keymsg.key;
}

//子菜单名称美化函数
void SubNameCat(uint8_t* SubMenuName,uint8_t *TableMenuName)
{
    const uint8_t SubTitle1[] = " -=";
    const uint8_t SubTitle2[] = "=- ";
    for (uint8_t i = 0;i < 20;i++){
        SubMenuName[i] = '\0';
    }
    uint8_t TableBody[20];
    uint8_t NameLenth = (uint8_t)strlen((char*)TableMenuName);
    for (uint8_t i = 0;i < NameLenth;i ++){
        TableBody[i] = TableMenuName[i];
        if(TableMenuName[i] == ' ') {
            TableBody[i] = '\0';
            NameLenth = i;
        }
    }
    if (NameLenth > 14) NameLenth = 14;
    strcat((char*)SubMenuName,(char*)SubTitle1);
    for (uint8_t i = 0;i < (14-NameLenth)/2         ;i ++) strcat((char*)SubMenuName," "); //填充空格
    strcat((char*)SubMenuName,(char*)TableBody);
    for (uint8_t i = 0;i < 7-NameLenth + NameLenth/2;i ++) strcat((char*)SubMenuName," "); //填充空格
    strcat((char*)SubMenuName,(char*)SubTitle2);
}

//参数调节函数
void adjustParam(Site_t site, MENU_TABLE *table, uint16_t Color, uint16_t bkColor)
{
    //根据 uint32_t INT32 EnumNameNum三种类型分别调节
    do
    {
        KeySan();
        MenuParam param;
        if (table->MenuType == Para_uint)   param.UINT32 = table->MenuParams.UINT32;
        else                                param.INT32  = table->MenuParams.INT32;
        switch (keymsg.key)
        {
            case KEY_U:
                if (table->MenuType == Para_uint)
                    (*param.UINT32)++;
                else if (table->MenuType == Param_Int)
                    (*param.INT32)++;
                else if (table->MenuType == Enumerate){
                    if (*param.INT32 < EnumNameNum-1)
                    (*param.INT32)++;
                }
                break;

            case KEY_D:
                if (table->MenuType == Para_uint)
                    (*param.UINT32)--;
                else if (table->MenuType == Param_Int)
                    (*param.INT32)--;
                else if (table->MenuType == Enumerate){
                    if (*param.INT32 > 0)
                    (*param.INT32)--;
                }
                break;

            case KEY_L:
                if (table->MenuType == Para_uint)
                    (*param.UINT32)-=10;
                else if (table->MenuType == Param_Int)
                    (*param.INT32)-=10;
                else if (table->MenuType == Enumerate){
                    if (*param.INT32 > 5)
                    (*param.INT32)-=5;
                }
                break;

            case KEY_R:
                if (table->MenuType == Para_uint)
                    (*param.UINT32)+=10;
                else if (table->MenuType == Param_Int)
                    (*param.INT32)+=10;
                else if (table->MenuType == Enumerate){
                    if (*param.INT32 < EnumNameNum-5)
                    (*param.INT32)+=5;
                }
                break;

            default:
                break;
        }
        if (table->MenuType == Para_uint){
            m_tft180_show_uint(site.x, site.y, *param.UINT32, 5,Color, bkColor);
            if (table->ItemHook.ItemFunc != Func_Null) table->ItemHook.ItemFunc();
        }
        else if (table->MenuType == Param_Int){
            m_tft180_show_int(site.x, site.y, *param.INT32, 5,Color, bkColor);
            if (table->ItemHook.ItemFunc != Func_Null) table->ItemHook.ItemFunc();
        }
        else if (table->MenuType == Enumerate){
            m_tft180_show_string(site.x, site.y,(int8*)(table->ItemHook.EnumName+(*param.INT32)*(EnumNameLenth+1)),Color, bkColor);
        }
    } while (keymsg.key != KEY_M);
}
//-------------------------------------------------------------------------------------------------------------------
// 函数简介     清空数据缓冲区
// 参数说明     void
// 返回参数     void
// 使用示例     flash_buffer_clear();
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
void flash_buffer_clear (void)
{
    memset(flash_union_buffer, 0xFF, EEPROM_PAGE_LENGTH);
}


void Write_EEPROM(void)
{
    const uint16 Flash_Save_uintNum = sizeof(EEPROM_DATA) / sizeof(EEPROM_DATA[0]);
    Flash_PageErase(FLASH_SAVE_ADDR);
    flash_buffer_clear();   //清除缓存
		STMFLASH_Write_Word(FLASH_SAVE_ADDR,*EEPROM_DATA,Flash_Save_uintNum);
    flash_buffer_clear();   //清除缓存

}
void Read_EEPROM(void)
{

    const uint16 Flash_Save_uintNum = sizeof(EEPROM_DATA) / sizeof(EEPROM_DATA[0]);
		flash_buffer_clear();   //清除缓存
		STMFLASH_Read_Word(FLASH_SAVE_ADDR,*EEPROM_DATA,Flash_Save_uintNum);
		flash_buffer_clear();   //清除缓存
}

