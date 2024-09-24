/*      1.�����Ĳ�������               Para_uint, Param_Int
 *          �ڴ����ʱ��ֻ��Ҫ1>�� MenuParams��ʼ��Ϊ{.uint32_t = (uint32_t *)&param}
 *                                 ����{.INT32 = (int32 *)&param}
 *                        2>�� ItemHook ��ʼ��Ϊ {.ItemFunc = Func_Null}
 *                        3>�� MenuType ��ʼ��ΪPara_uint����Param_Int
 *      2.�������ڵ�ͬʱ���к������� ����   Para_uint, Param_Int
 *          ������ֵ�ĵ��ڣ����pwm���
 *                        1>�� MenuParams��ʼ��Ϊ{.uint32_t = (uint32_t *)&param}
 *                                 ����{.INT32 = (int32 *)&param}
 *                        2>�� ItemHook ��ʼ��Ϊ {.ItemFunc = ���õĺ�����}
 *                        3>�� MenuType ��ʼ��ΪPara_uint����Param_Int
 *      3.ö�����͵���ʾ               Enumerate
 *          ���������������runmode runstate�ȿ���ģʽ��Ϣ
 *          ���԰�ö�ٱ�����eeprom Ҫ����ǿ��ת��Ϊ (int32 *)
 *          ����Ҫ��         1>�� MenuParams��ʼ��Ϊ{.uint32_t = (uint32_t *)&param} ���� {.INT32 = (int32 *)&param}
 *                        2>�� ItemHook ��ʼ��Ϊ {.EnumName = ö������������}
 *                        3>�� MenuType ��ʼ��Ϊ Enumerate
 *               �����������д��ģ��char PointTypeName[EnumNameNum][EnumNameLenth+1]
 *               �����ַ��������ڱ�����ʱ�������\0 ����������Ҫ�ֶ���һ�� Ȼ�󳤶ȺͿ���ں궨����д�� �ַ�������Ϊ6 ����Ϊ10 �������޸� �����NULL����ͺ�
 *      4.ֻ�д�������                 Functions
 *                        1>�� MenuParams ��ʼ��Ϊ {.SubMenu = Table_Null}
 *                        2>�� ��ItemHook ��ʼ��Ϊ {.ItemFunc = ���õĺ�����}
 *                        3>�� MenuType ��ʼ��Ϊ Functions
 *      5.�Ӳ˵�                     Sub_Menus
 *                        1>�� MenuParams ��ʼ��Ϊ {.SubMenu = MenuDebug}
 *                        2>�� ItemHook ��ʼ��Ϊ   {.SubMenuNum = MenuNum(MenuDebug)} ������������Ӳ˵�������
 *                        3>�� MenuType ��ʼ��Ϊ Sub_Menus
 */
//ע��1.MainMenu_Set�е�Read_EEPROM���ڵ�һ�����ò�����ע�͵����У�д������¿��������
//   2.�Ӳ˵������ƺ�Ӧ�����пո�
//   3.����ͷ�²�����Ҫ������Ƭ��������Ч
#include "menu.h" //����˵�ʹ�õ�ȫ�ֱ���
#include "tftOut.h"
#include "servo.h"
#include "eeprom.h"
#include "motor.h"
#include "encoderM.h"

#define         MenuNum(Table)      sizeof(Table)/sizeof(Table[0])
volatile uint8_t ExitMenu_flag = 0;//�˳��˵��ı�־λ
#define Type_Null 5
#define Para_uint 0
#define Functions 4
#define Sub_Menus 3
#define Param_Int 1
#define Enumerate 2

extern volatile KEY keymsg;
uint32_t flash_union_buffer[EEPROM_PAGE_LENGTH];                    // FLASH ���������ݻ�����

//����һЩ��ֵ
#define         Debug_Null          NULL
void            Func_Null(void)     {};
MENU_TABLE  Table_Null[] = {{(uint8_t *)"", {.SubMenu = Table_Null}, Type_Null, {.ItemFunc = Func_Null}},};

//����һЩ�˵�������Ҫʹ�õĹ��ܺ���,�������ܼ�ʵ�ִ�
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
/****************************************Basic_Test�˵�******************************************/
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
/****************************************Basic_Test�˵�******************************************/

/****************************************���˵�������******************************************/
MENU_TABLE MainMenu_Table[] =
{
    {(uint8_t *)"1.Basic_Test     ",{.SubMenu = Basic_Test},  Sub_Menus ,   {.SubMenuNum = MenuNum(Basic_Test)}},

};
/****************************************���˵�������******************************************/

//�˵���ʼ������
void Menu_PrmtInit(MENU_PRMT *prmt, uint8_t num, uint8_t page)
{
    prmt->ExitMark = 0; //����˳��˵���־
    prmt->Cursor = 0;    //�������
    prmt->PageNo = 0;    //ҳ����
    prmt->Index = 0;     //��������
    prmt->DispNum = num; //ҳ�����ʾ��Ŀ��
    prmt->MaxPage = page; //���ҳ��
}

//�˵�����ƶ�����
uint8_t Menu_Move(MENU_PRMT *prmt, KEY_ID key)
{
    uint8_t rValue = 1;//����ʱ������ȷ�ϼ�
    switch (key)
    {
        case KEY_U: // ����
        {
            if (prmt->Cursor != 0) // ��겻�ڶ���
            {
                prmt->Cursor--; // �������
            }
            else // ����ڶ���
            {
                if (prmt->PageNo != 0) // ҳ��û�е���С
                {
                    prmt->PageNo--; // ���Ϸ�
                }
                else
                {
                    prmt->Cursor = prmt->DispNum - 1; // ��굽��
                    prmt->PageNo = prmt->MaxPage - 1; // ���ҳ
                }
                tft180_full(RGB565_WHITE);
            }
            break;
        }

        case KEY_D: // ����
        {
            if (prmt->Cursor < prmt->DispNum - 1) // ���û�е��ף��ƶ����
            {
                prmt->Cursor++; // ��������ƶ�
            }
            else // ��굽��
            {
                if (prmt->PageNo < prmt->MaxPage - 1) // ҳ��û�е��ף�ҳ���ƶ�
                {
                    prmt->PageNo++; // �·�һҳ
                }
                else // ҳ��͹�궼���ף����ؿ�ʼҳ
                {
                    prmt->Cursor = 0;
                    prmt->PageNo = 0;
                }
                tft180_full(RGB565_WHITE);
            }
            break;
        }
        case KEY_M: // ȷ��
        {
            prmt->Index = prmt->Cursor + prmt->PageNo; //����ִ���������,����Ϊ������ҳ��
            rValue = 0;

            break;
        }
        case KEY_L: // ��������ϼ��˵�
        {
            prmt->ExitMark = 1;
            break;
        }
        case KEY_R: // �Ҽ������ײ�
        {
            prmt->Cursor = prmt->DispNum - 1; // ��굽��
            prmt->PageNo = prmt->MaxPage - 1; // ���ҳ
            tft180_full(RGB565_WHITE);
            break;
        }
        default:
            break;
    }
    return rValue; // ����ִ������
}

//��ʾ�˵�����
void Menu_Display(MENU_TABLE *menuTable, uint8_t pageNo, uint8_t dispNum, uint8_t cursor)
{
    uint8_t i;
    Site_t site;
    for (i = 0; i < dispNum; i++)
    {
        site.x = 0;
        site.y = (i+1)*16;
        if (cursor == i)
            /* ������ʾ��ǰ���ѡ�в˵��� */
            m_tft180_show_string((uint16_t)site.x, (uint16_t)site.y, (const int8 *)menuTable[pageNo + i].MenuName, RGB565_WHITE, RGB565_BLUE);
        else
            /* ������ʾ����˵��� */
            m_tft180_show_string((uint16_t)site.x, (uint16_t)site.y, (const int8 *)menuTable[pageNo + i].MenuName, RGB565_BLUE, RGB565_WHITE);
        /* ���˲˵�������Ҫ���Ĳ���������ʾ�ò��� */
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
                //EnumName��һ��ָ��ö�������Ƶ��ַ������ָ�롣
                //num_t*(EnumNameLenth+1) �����ڼ��㵱ǰö��ֵ��ö�������������е�ƫ����
            }
        }
    }
}

//����˵�����
void Menu_Process(uint8_t *menuName, MENU_PRMT *prmt, MENU_TABLE *table, uint8_t num)
{
    KEY_ID key;
    Site_t site;
    uint8_t page; //��ʾ�˵���Ҫ��ҳ��
    if (num - PAGE_DISP_NUM <= 0)
        page = 1;
    else
    {
        page = num - PAGE_DISP_NUM + 1;//�༸���Ͷ���Ҫ��ʾ��ҳ
        num = PAGE_DISP_NUM;
    }
    // ��ʾ������ҳ������
    Menu_PrmtInit(prmt, num, page);
    do
    {
        LCD_ShowString((uint16_t)0, (uint16_t)0, (const uint8_t *)menuName, RGB565_RED, RGB565_WHITE,16,0); //��ʾ�˵�����
        // ��ʾ�˵���
        Menu_Display(table, prmt->PageNo, prmt->DispNum, prmt->Cursor);
        key = KeySan(); //��ȡ����

        if (Menu_Move(prmt, key) == 0) //�˵��ƶ� ����ȷ�ϼ�
        {
            // �жϴ˲˵���������Ҫ���ڵĲ��� �������������� �ڲ��������￴���޺���ͬʱ����
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
                //�ڲ��������￴���޺���ͬʱ����  ����ͬʱִ�� ���������ԣ�������� ���������ĵ��ڲ����������Ѿ�ִ�й�
                adjustParam(site, &table[prmt->Index], RGB565_WHITE, RGB565_RED);
            }
            // ���ǲ������ڵĻ���ִ�в˵�����
            else if(table[prmt->Index].MenuType == Functions)
            {
                table[prmt->Index].ItemHook.ItemFunc(); // ִ����Ӧ��
            }
            // û�в������ںͺ���ִ�еĻ� �����Ӳ˵�
            else if(table[prmt->Index].MenuType == Sub_Menus){
                //ȷ�����Ӳ˵�
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
    Write_EEPROM();//������д��EEPROM����
    tft180_full(RGB565_WHITE);
}

//�������˵�����
void MainMenu_Set(void)
{
    ExitMenu_flag = 0;
    tft180_full(RGB565_WHITE);
    MENU_PRMT MainMenu_Prmt;
    uint8_t menuNum = MenuNum(MainMenu_Table); // �˵�����
    Read_EEPROM();//��ȡ���ݣ���һ�����ò�����ע�͵����У�д������¿�������䣩
    
    Menu_Process((uint8_t *)" -=  Setting  =- ", &MainMenu_Prmt, MainMenu_Table, menuNum);
    tft180_full(RGB565_WHITE);
}

//----------------------------------�����ǹ������˵���������ʹ�õĹ��ܺ���----------------------------//

//������ȡ����
KEY_ID KeySan(void)
{
    while (keymsg.status == KEY_UP && !ExitMenu_flag);
    keymsg.status = KEY_UP;
    return keymsg.key;
}

//�Ӳ˵�������������
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
    for (uint8_t i = 0;i < (14-NameLenth)/2         ;i ++) strcat((char*)SubMenuName," "); //���ո�
    strcat((char*)SubMenuName,(char*)TableBody);
    for (uint8_t i = 0;i < 7-NameLenth + NameLenth/2;i ++) strcat((char*)SubMenuName," "); //���ո�
    strcat((char*)SubMenuName,(char*)SubTitle2);
}

//�������ں���
void adjustParam(Site_t site, MENU_TABLE *table, uint16_t Color, uint16_t bkColor)
{
    //���� uint32_t INT32 EnumNameNum�������ͷֱ����
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
// �������     ������ݻ�����
// ����˵��     void
// ���ز���     void
// ʹ��ʾ��     flash_buffer_clear();
// ��ע��Ϣ
//-------------------------------------------------------------------------------------------------------------------
void flash_buffer_clear (void)
{
    memset(flash_union_buffer, 0xFF, EEPROM_PAGE_LENGTH);
}


void Write_EEPROM(void)
{
    const uint16 Flash_Save_uintNum = sizeof(EEPROM_DATA) / sizeof(EEPROM_DATA[0]);
    Flash_PageErase(FLASH_SAVE_ADDR);
    flash_buffer_clear();   //�������
		STMFLASH_Write_Word(FLASH_SAVE_ADDR,*EEPROM_DATA,Flash_Save_uintNum);
    flash_buffer_clear();   //�������

}
void Read_EEPROM(void)
{

    const uint16 Flash_Save_uintNum = sizeof(EEPROM_DATA) / sizeof(EEPROM_DATA[0]);
		flash_buffer_clear();   //�������
		STMFLASH_Read_Word(FLASH_SAVE_ADDR,*EEPROM_DATA,Flash_Save_uintNum);
		flash_buffer_clear();   //�������
}

