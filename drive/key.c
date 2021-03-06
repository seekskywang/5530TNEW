/******************** (C) COPYRIGHT 2015 AVER********************
 * 文件名  ：KEY.C
 * 作者    ：李振
 * 描述    ：3*2矩阵键盘扫描
 * 内容    ：扫描完成后出键值
 * 硬件连接：一路3*2矩阵键盘  一路编码器独立按键
 * 修改日期：2015-08-18
********************************************************************/
#include "my_register.h"
#include "beep.h"
#include "flash.h"
#include "gpio.h"
#include "key.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "ssd1963.h"
#include  "gui.h"
#include "MainTask.h"
#include "internalflash.h"

extern WM_HWIN CreateR(void);
extern WM_HWIN CreateWindow2(void);
extern WM_HWIN CreateWindow(void);
extern WM_HWIN CreateG(void);
extern WM_HWIN CreateSET(void);
extern WM_HWIN Createcal(void);
void setmode_r(void);
extern WM_HWIN hWinWind;
extern WM_HWIN hWinR;
extern WM_HWIN load_wind;//璐熻浇鐣岄潰鍙ユ焺
extern WM_HWIN hWinG;
extern WM_HWIN hWinset;
extern WM_HWIN hWincdc;
extern WM_HWIN hWinsysinfo;
extern vu8 para_set2;
extern vu16 s;
extern vu8 cdc_sw;
extern vu8 oct_sw;
extern vu8 timer_sw;
extern vu8 mode_sw;
extern vu8 pow_sw;
extern vu8 load_sw;
extern vu8 admin;
extern vu8 pause_flag;
extern vu16 restart_time;
extern vu8 paused;
extern vu8 finish;
extern vu8 short_finish;
extern float gate_v;
extern vu8 manual;
extern vu8 oc_mode;
extern vu8 cal;
extern char inputv[10];
extern vu8 con_flag;
extern vu8 test_finish;
extern float v;
vu8 t_MODE;
vu8 pass = 0;
vu8 LOAD_t;
vu8 track = face_r;
vu8 clear_flag1 = 0;
vu8 clear_flag2 = 0;
vu8 clear_flag3 = 0;
vu16 menu_time;
vu16 load_time;
vu8 r_test = 0;
float clear_v = 0;
float clear_pv = 0;
float clear_lv = 0;
vu8 clear_r = 0;
vu8 r_raly=0;
vu8 lock;
vu8 c_rec;
vu8 calstep;
vu8 oldcal;
extern vu8 test_start;
extern vu8 step;
extern vu16 r;
extern vu8 staticcdc;

//=================================================================
extern struct bitDefine
{
    unsigned bit0: 1;
    unsigned bit1: 1;
    unsigned bit2: 1;
    unsigned bit3: 1;
    unsigned bit4: 1;
    unsigned bit5: 1;
    unsigned bit6: 1;
    unsigned bit7: 1;
} flagA, flagB,flagC,flagD,flagE,flagF,flagG;
//====================================================================



#define MAX_VOL  3200		
#define MAX_CUR  5100	
vu8 t_KeyScan;
vu8 NewKey;
vu8 t_wei;
vu8 t_beep;
vu8 t_Str;
vu8 t_lock;
vu8 t_onoff;
vu8 t_doub;
vu8 t_key_step;//閿洏鎵弿璁℃暟鍣?
vu16 Key1,Key2,Key3,Key4,Key5,Key6,Key7,Key8;
vu32 Key9;
vu16 KeyCounter;
vu16 MAX_limit;//自动测试上限值
vu16 MIN_limit;//自动测试下限值
vu16 Test_Time;
vu16 Test_Time1;
vu16 Time_sw;//定时
vu16 Test_Daley;//自动测试判断等待时间，此时间用户可设置
vu16 Test_C_Door;//自动测试门槛电流
vu8 OFFSET_Zore;//零点值
vu32 Key_Scan(void);
void Lift_Move(void);
void Right_Move(void);
void Setvalue_Add(void);
void IO_OFF(void);
extern u8 calmode;
/* 閿�煎畾涔?*/
#define        KEY_1                		 0X4E     //
#define        KEY_2                		 0X56     //
#define        KEY_3                		 0X5E     //
#define        KEY_4                		 0X57      //
#define        KEY_5                		 0X5F      //
#define        KEY_6                		 0X76      //
#define        KEY_7                		 0X67     //
#define        KEY_8                		 0X6F      //
#define        KEY_9                		 0X77     //
#define        KEY_0                		 0X5D      //
#define        KEY_dian              		 0X65     //
#define        KEY_ESC              		 0X6D     //

#define        ENTER                     0X4D    //
#define        PUSH_Lift                 0X45     //
#define        PUSH_Right                0X55      //
#define        PUSH_Up                   0X46      //
#define        PUSH_Down                 0X74      //
#define        PUSH_DISP                 0X47      //
#define        PUSH_SETUP                0X4F     //

#define        KEY_Lock                  0X6C     //
#define        KEY_BIAS                  0X6E      //
#define        KEY_Reset                 0X66      //
#define        KEY_TRIG                  0X75      //

//#define        KEY_Powe                  0X6C     //
#define        KEY_Face1                 0X64      //
#define        KEY_Face2                 0X5c      //
#define        KEY_Face3                 0X54      //
#define        KEY_Face4                 0X4c      //
#define        KEY_Face5                 0X44      //

/* 瀹氫箟鎸夐敭鍔ㄤ綔 */
#define        KEY_NULL_VALUE             0X00    //无动作
#define        KEY_PRESS                  0X10    //单击
#define        KEY_LONG                   0X20    //长按
#define        KEY_CONTINUE               0X40    //连击
#define        KEY_UP                     0X80    //按键抬起
/*瀹氫箟鎸夐敭澶勭悊姝ラ*/
#define        KEY_INIT_STATE               0      //初态
#define        KEY_WOBBLE_STATE             1      //消抖
#define        KEY_PRESS_STATE              2      //触发单键
#define        KEY_LONG_STATE               3      //长按触发
#define        KEY_CONTINUE_STATE           4      //连续触发
#define        KEY_RELEASE_STATE            5      //恢复初态
/* 闀挎寜鏃堕棿 */
#define         KEY_LONG_PERIOD             50                /* 长按时间1S */
#define        KEY_CONTINUE_PERIOD          10                /* 双击时间500ms */

/************************************************************************************************************************/
vu32 Key_Scan(void)
{
	static vu8 KeyState = KEY_INIT_STATE;
	static vu8 IsKeyRelease = 1;

	static vu8 KeyValueTemp = 0;
	vu8 KeyValue ;
	KeyValue= KEY_NULL_VALUE;
	
	if((NewKey>0x37)&&(IsKeyRelease==1))
	{
		if(KeyState == KEY_INIT_STATE)                    /*无按键按下*/
		{
			KeyState = KEY_WOBBLE_STATE;
			return KEY_NULL_VALUE;
		}
		else if(KeyState == KEY_WOBBLE_STATE)                /* 消抖 */
		{
			KeyState = KEY_PRESS_STATE;
			return KEY_NULL_VALUE;
		}
		else if(KeyState == KEY_PRESS_STATE)                /* 有按键按下，返回按键值 */
		{
			KeyValue = NewKey;
			KeyValueTemp = KeyValue;
			KeyState = KEY_CONTINUE_STATE;
			return KeyValue;
		}
		else if(KeyState == KEY_CONTINUE_STATE)           /*双击按键*/
		{
			KeyCounter ++;
			if(KeyCounter == KEY_CONTINUE_PERIOD)
			{
				 KeyCounter = 0;
				 KeyState = KEY_LONG_STATE;
			}
		}
		else if(KeyState == KEY_LONG_STATE)                        /* 长按按键 */
		{
			KeyCounter ++;
			if(KeyCounter == KEY_LONG_PERIOD)
			{
				KeyCounter = 0;
				Flag_Long=1; //长按触发标志位
			}
		}
	}
	else if(NewKey<=0x37)
	{
		KeyState = KEY_INIT_STATE;        /* 误触发，返回到初始状态 */
		IsKeyRelease = 1;
		Flag_Long=0;
	}
	return KEY_NULL_VALUE;
}
/***************************************************************************************************************************/
void Key_Funtion(void)
{
	vu32 KeyValue = 0;
	static vu8 t_KEYON;
	static vu8 a,b;
/****************************************************鐭寜鍔熻兘************************************************************/
	if(Flag_Key_Scan==1)
	{
		Flag_Key_Scan = 0;
		KeyValue = Key_Scan();
        if(lock == 0)
        {
            switch(KeyValue)
            {
                case KEY_1 ://
                {
                    switch(page_sw){
                        case face_menu: 
                        {
                            INPUT_POW("1");
                            KeyCounter = 0;
                            BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                            break;
                        }
                        case face_set:
                        {
                             INPUT_NUM("1");
                             KeyCounter = 0;
                             BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                             break;
                        }
                        case face_r:
                        {
                            if(para_set2 == set_2_on)
                            {
                                INPUT_C("1");
                                KeyCounter = 0;
                                BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                                break;
                            }
                        }   
                        case face_load:
                        {
                            INPUT_LOAD("1");
                            KeyCounter = 0;
                            BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                            break;
                        }
                        case face_cdc:
                        {
                            INPUT_CDC("1");
                            KeyCounter = 0;
                            BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                            break;
                        }
                        case face_sys_info:
                        {
                            if(admin == 1)
                            {
                                INPUT_INFO(1);    
                            }else if(cal == 1){
                                Rlow_cal(1);
                            }else{
                                pass += 1;
                            }
                            KeyCounter = 0;
                            BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                            break;
                        }
                        case face_cal:
                        {
                            if(calmode == mode_load)
                            {
                                LoadVCal(1);
                            }else if(calmode == mode_pow){
                                IO_OFF();
                                SET_Voltage = 100;
                                SET_Current = 1000;  
                                GPIO_ResetBits(GPIOC,GPIO_Pin_13);//鎵撳紑鐢垫簮杈撳嚭缁х數鍣�
                                GPIO_SetBits(GPIOC,GPIO_Pin_1);//鎵撳紑鐢垫簮杈撳嚭
                                calstep = 1;
                            }else if(calmode == mode_r ){
                                Rlow_cal(1);
                            }else if(calmode == mode_loadc){
                                IO_OFF();
                                SET_Current_Laod = 1000;
                                GPIO_ResetBits(GPIOC,GPIO_Pin_10);//CC
                                flag_Load_CC = 1;
                                GPIO_ResetBits(GPIOA,GPIO_Pin_15);//鐢靛瓙璐熻浇On
                                calstep = 1;
                            }else if(calmode == mode_powc){
                                IO_OFF();
                                SET_Voltage = 1000;
                                SET_Current = 1000;  
                                GPIO_ResetBits(GPIOC,GPIO_Pin_13);//鎵撳紑鐢垫簮杈撳嚭缁х數鍣�
                                GPIO_SetBits(GPIOC,GPIO_Pin_1);//鎵撳紑鐢垫簮杈撳嚭
                                calstep = 1;
                            }else if(calmode == input){
                                inputcal("1");
                            }
                            KeyCounter = 0;
                            BEEP_Tiggr();//瑙﹀彂铚傞福鍣�
                        }break;
                    }
                    KeyCounter = 0;
                    BEEP_Tiggr();//
                }
                break;
                
                case KEY_2 ://
                {
                    switch(page_sw){
                        case face_menu:  
                        {
                            INPUT_POW("2");
                            KeyCounter = 0;
                            BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                        }
                        case face_set:
                        {
                            INPUT_NUM("2");
                            KeyCounter = 0;
                            BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                            break;
                        }
                        case face_r:
                        {
                            if(para_set2 == set_2_on)
                            {
                                INPUT_C("2");
                                KeyCounter = 0;
                                BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                                break;
                            }
                        }
                        case face_load:
                        {
                            INPUT_LOAD("2");
                            KeyCounter = 0;
                            BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                            break;
                        }
                        case face_cdc:
                        {
                            INPUT_CDC("2");
                            KeyCounter = 0;
                            BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                            break;
                        }
                        case face_sys_info:
                        {
                            if(admin == 1)
                            {
                                INPUT_INFO(2);    
                            }else if(cal == 1){
                                Rlow_cal(2);
                            }else{
                                pass += 2;
                            }
                            KeyCounter = 0;
                            BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                            break;
                        }
                        case face_cal:
                        {
                            if(calmode == mode_load)
                            {
                                LoadVCal(2);
                            }else if(calmode == mode_pow){
                                IO_OFF();
                                SET_Voltage = 1000;
                                SET_Current = 1000;  
                                GPIO_ResetBits(GPIOC,GPIO_Pin_13);//鎵撳紑鐢垫簮杈撳嚭缁х數鍣�
                                GPIO_SetBits(GPIOC,GPIO_Pin_1);//鎵撳紑鐢垫簮杈撳嚭
                                calstep = 2;
                            }else if(calmode == mode_r ){
                                Rlow_cal(2);
                            }else if(calmode == mode_loadc){
                                IO_OFF();
                                SET_Current_Laod = 5000;
                                GPIO_ResetBits(GPIOC,GPIO_Pin_10);//CC
                                flag_Load_CC = 1;
                                GPIO_ResetBits(GPIOA,GPIO_Pin_15);//鐢靛瓙璐熻浇On
                                calstep = 2;
                            }else if(calmode == mode_powc){
                                IO_OFF();
                                SET_Voltage = 1000;
                                SET_Current = 5000;  
                                GPIO_ResetBits(GPIOC,GPIO_Pin_13);//鎵撳紑鐢垫簮杈撳嚭缁х數鍣�
                                GPIO_SetBits(GPIOC,GPIO_Pin_1);//鎵撳紑鐢垫簮杈撳嚭
                                calstep = 2;
                            }else if(calmode == input){
                                inputcal("2");
                            }
                            KeyCounter = 0;
                            BEEP_Tiggr();//瑙﹀彂铚傞福鍣�
                        }break;
       
                    }
                    KeyCounter = 0;
                    BEEP_Tiggr();//
                }
                break;
                
                case KEY_3 ://
                {
                    switch(page_sw){
                        case face_menu:  
                        {
                            INPUT_POW("3");
                            KeyCounter = 0;
                            BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                        }
                        case face_set:
                        {
                            INPUT_NUM("3");
                            KeyCounter = 0;
                            BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                            break;
                        }
                        case face_r:
                        {
                            if(para_set2 == set_2_on)
                            {
                                INPUT_C("3");
                                KeyCounter = 0;
                                BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                                break;
                            }
                        }
                        case face_load:
                        {
                            INPUT_LOAD("3");
                            KeyCounter = 0;
                            BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                            break;
                        }
                        case face_cdc:
                        {
                            INPUT_CDC("3");
                            KeyCounter = 0;
                            BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                            break;
                        }
                        case face_sys_info:
                        {
                            if(admin == 1)
                            {
                                INPUT_INFO(3);    
                            }else if(cal == 1){
                                Rlow_cal(3);
                            }else{
                                pass += 3;
                            }
                            KeyCounter = 0;
                            BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                            break;
                        }
                        case face_cal:
                        {
                            if(calmode == mode_load)
                            {
                                LoadVCal(3);
                            }else if(calmode == mode_pow){
                                IO_OFF();
                                SET_Voltage = 1000;
                                SET_Current = 1000;  
                                GPIO_ResetBits(GPIOC,GPIO_Pin_13);//鎵撳紑鐢垫簮杈撳嚭缁х數鍣�
                                GPIO_SetBits(GPIOC,GPIO_Pin_1);//鎵撳紑鐢垫簮杈撳嚭
                                calstep = 3;
                            }else if(calmode == mode_r ){
                                Rlow_cal(3);
                            }else if(calmode == input){
                                inputcal("3");
                            }
                            KeyCounter = 0;
                            BEEP_Tiggr();//瑙﹀彂铚傞福鍣�
                        }break;
                    }
                }
                break;
                
                case KEY_4 ://
                {
                    switch(page_sw){
                        case face_menu:  
                        {
                            INPUT_POW("4");
                            KeyCounter = 0;
                            BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                        }
                        case face_set:
                        {
                            INPUT_NUM("4");
                            KeyCounter = 0;
                            BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                            break;
                        }
                        case face_r:
                        {
                            if(para_set2 == set_2_on)
                            {
                                INPUT_C("4");
                                KeyCounter = 0;
                                BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                                break;
                            }
                        }
                        case face_load:
                        {
                            INPUT_LOAD("4");
                            KeyCounter = 0;
                            BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                            break;
                        }
                        case face_cdc:
                        {
                            INPUT_CDC("4");
                            KeyCounter = 0;
                            BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                            break;
                        }
                        case face_sys_info:
                        {
                            if(admin == 1)
                            {
                                INPUT_INFO(4);    
                            }else if(cal == 1){
                                Rlow_cal(4);
                            }else{
                                pass += 4;
                            }
                            KeyCounter = 0;
                            BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                            break;
                        }
                        case face_cal:
                        {
                            if(calmode == mode_load)
                            {
                                LoadVCal(4);
                            }else if(calmode == mode_pow){
                                IO_OFF();
                                SET_Voltage = 3000;
                                SET_Current = 1000;  
                                GPIO_ResetBits(GPIOC,GPIO_Pin_13);//鎵撳紑鐢垫簮杈撳嚭缁х數鍣�
                                GPIO_SetBits(GPIOC,GPIO_Pin_1);//鎵撳紑鐢垫簮杈撳嚭
                                calstep = 4;
                            }else if(calmode == mode_r ){
                                Rlow_cal(4);
                            }else if(calmode == input){
                                inputcal("4");
                            }
                            KeyCounter = 0;
                            BEEP_Tiggr();//瑙﹀彂铚傞福鍣�
                        }break;
                    }
                }
                break;
                
                case KEY_5 ://
                {
                    switch(page_sw){
                        case face_menu:  
                        {
                            INPUT_POW("5");
                            KeyCounter = 0;
                            BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                        }
                        case face_set:
                        {
                            INPUT_NUM("5");
                            KeyCounter = 0;
                            BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                            break;
                        }
                        case face_r:
                        {
                            if(para_set2 == set_2_on)
                            {
                                INPUT_C("5");
                                KeyCounter = 0;
                                BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                                break;
                            }
                        }
                        case face_load:
                        {
                            INPUT_LOAD("5");
                            KeyCounter = 0;
                            BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                            break;
                        }
                        case face_cdc:
                        {
                            INPUT_CDC("5");
                            KeyCounter = 0;
                            BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                            break;
                        }
                        case face_sys_info:
                        {
                            if(admin == 1)
                            {
                                INPUT_INFO(5);    
                            }else if(cal == 1){
                                Rlow_cal(5);
                            }else{
                                pass += 5;
                            }
                            KeyCounter = 0;
                            BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                            break;
                        }
                        case face_cal:
                        {
                            if(calmode == mode_load)
                            {
                                LoadVCal(5);
                            }else if(calmode == mode_pow){
                                IO_OFF();
                                SET_Voltage = 3000;
                                SET_Current = 1000;  
                                GPIO_ResetBits(GPIOC,GPIO_Pin_13);//鎵撳紑鐢垫簮杈撳嚭缁х數鍣�
                                GPIO_SetBits(GPIOC,GPIO_Pin_1);//鎵撳紑鐢垫簮杈撳嚭
                                calstep = 5;
                            }else if(calmode == mode_r ){
                                Rlow_cal(5);
                            }else if(calmode == input){
                                inputcal("5");
                            }
                            KeyCounter = 0;
                            BEEP_Tiggr();//瑙﹀彂铚傞福鍣�
                        }break;
                    }
                }
                break;	
                
                case KEY_6 ://
                {
                    switch(page_sw){
                        case face_menu:  
                        {
                            INPUT_POW("6");
                            KeyCounter = 0;
                            BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                        }
                        case face_set:
                        {
                            INPUT_NUM("6");
                            KeyCounter = 0;
                            BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                            break;
                        }
                        case face_r:
                        {
                            if(para_set2 == set_2_on)
                            {
                                INPUT_C("6");
                                KeyCounter = 0;
                                BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                                break;
                            }
                        }
                        case face_load:
                        {
                            INPUT_LOAD("6");
                            KeyCounter = 0;
                            BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                            break;
                        }
                        case face_cdc:
                        {
                            INPUT_CDC("6");
                            KeyCounter = 0;
                            BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                            break;
                        } 
                        case face_sys_info:
                        {
                            if(admin == 1)
                            {
                                INPUT_INFO(6);    
                            }else if(cal == 1){
                                Rlow_cal(6);
                            }
                            KeyCounter = 0;
                            BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                            break;
                        }
                        case face_cal:
                        {
                            if(calmode == mode_load)
                            {
                                LoadVCal(6);
                            }else if(calmode == mode_pow){
                                IO_OFF();
                                SET_Voltage = 6000;
                                SET_Current = 1000;  
                                GPIO_ResetBits(GPIOC,GPIO_Pin_13);//鎵撳紑鐢垫簮杈撳嚭缁х數鍣�
                                GPIO_SetBits(GPIOC,GPIO_Pin_1);//鎵撳紑鐢垫簮杈撳嚭
                                calstep = 6;
                            }else if(calmode == mode_r ){
                                Rlow_cal(6);
                            }else if(calmode == input){
                                inputcal("6");
                            }
                            KeyCounter = 0;
                            BEEP_Tiggr();//瑙﹀彂铚傞福鍣�
                        }break;
                    }
                }
                break;
                
                case KEY_7 ://
                {
                    switch(page_sw){
                        case face_menu:  
                        {
                            INPUT_POW("7");
                            KeyCounter = 0;
                            BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                        }
                        case face_set:
                        {
                            INPUT_NUM("7");
                            KeyCounter = 0;
                            BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                            break;
                        }
                        case face_r:
                        {
                            if(para_set2 == set_2_on)
                            {
                                INPUT_C("7");
                                KeyCounter = 0;
                                BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                                break;
                            }
                        }
                        case face_load:
                        {
                            INPUT_LOAD("7");
                            KeyCounter = 0;
                            BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                            break;
                        }
                        case face_cdc:
                        {
                            INPUT_CDC("7");
                            KeyCounter = 0;
                            BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                            break;
                        }
                        case face_sys_info:
                        {
                            if(admin == 1)
                            {
                                INPUT_INFO(7);    
                            }
                            KeyCounter = 0;
                            BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                            break;
                        }
                        case face_cal:
                        {
                            if(calmode == mode_load)
                            {
                                LoadVCal(7);
                            }else if(calmode == mode_pow){
                                
                            }else if(calmode == mode_r ){
                                Rlow_cal(7);
                            }else if(calmode == input){
                                inputcal("7");
                            }
                            KeyCounter = 0;
                            BEEP_Tiggr();//瑙﹀彂铚傞福鍣�
                        }break;
                    }
                }
                break;
                
                case KEY_8 ://
                {
                    switch(page_sw){
                        case face_menu:  
                        {
                            INPUT_POW("8");
                            KeyCounter = 0;
                            BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                        }
                        case face_set:
                        {
                            INPUT_NUM("8");
                            KeyCounter = 0;
                            BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                            break;
                        }
                        case face_r:
                        {
                            if(para_set2 == set_2_on)
                            {
                                INPUT_C("8");
                                KeyCounter = 0;
                                BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                                break;
                            }
                        }
                        case face_load:
                        {
                            INPUT_LOAD("8");
                            KeyCounter = 0;
                            BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                            break;
                        }
                        case face_cdc:
                        {
                            INPUT_CDC("8");
                            KeyCounter = 0;
                            BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                            break;
                        }
                        case face_sys_info:
                        {
                            if(admin == 1)
                            {
                                INPUT_INFO(8);    
                            }
                            KeyCounter = 0;
                            BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                            break;
                        }
                         case face_cal:
                        {
                            if(calmode == mode_load)
                            {
                                LoadVCal(8);
                            }else if(calmode == mode_pow){
                                
                            }else if(calmode == mode_r ){
                                Rlow_cal(8);
                            }else if(calmode == input){
                                inputcal("8");
                            }
                            KeyCounter = 0;
                            BEEP_Tiggr();//瑙﹀彂铚傞福鍣�
                        }break;
                    }
                }
                break;
                
                case KEY_9 ://
                {
                    switch(page_sw){
                        case face_menu:  
                        {
                            INPUT_POW("9");
                            KeyCounter = 0;
                            BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                        }
                        case face_set:
                        {
                            INPUT_NUM("9");
                            KeyCounter = 0;
                            BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                            break;
                        }
                        case face_r:
                        {
                            if(para_set2 == set_2_on)
                            {
                                INPUT_C("9");
                                KeyCounter = 0;
                                BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                                break;
                            }
                        }
                        case face_load:
                        {
                            INPUT_LOAD("9");
                            KeyCounter = 0;
                            BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                            break;
                        }
                        case face_cdc:
                        {
                            INPUT_CDC("9");
                            KeyCounter = 0;
                            BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                            break;
                        }
                        case face_sys_info:
                        {
                            if(admin == 1)
                            {
                                INPUT_INFO(9);    
                            }
                            KeyCounter = 0;
                            BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                            break;
                        }
                         case face_cal:
                        {
                            if(calmode == mode_load)
                            {
                                LoadVCal(9);
                            }else if(calmode == mode_pow){
                                
                            }else if(calmode == mode_r ){
                                Rlow_cal(9);
                            }else if(calmode == input){
                                inputcal("9");
                            }
                            KeyCounter = 0;
                            BEEP_Tiggr();//瑙﹀彂铚傞福鍣�
                        }break;
                    }
                }
                break;
                
                case KEY_0 ://
                {
                    switch(page_sw){
                        case face_menu:  
                        {
                            INPUT_POW("0");
                            KeyCounter = 0;
                            BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                        }
                        case face_set:
                        {
                            INPUT_NUM("0");
                            KeyCounter = 0;
                            BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                            break;
                        }
                        case face_r:
                        {
                            if(para_set2 == set_2_on)
                            {
                                INPUT_C("0");
                                KeyCounter = 0;
                                BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                                break;
                            }
                        }
                        case face_load:
                        {
                            INPUT_LOAD("0");
                            KeyCounter = 0;
                            BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                            break;
                        }
                        case face_cdc:
                        {
                            INPUT_CDC("0");
                            KeyCounter = 0;
                            BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                            break;
                        }
                        case face_sys_info:
                        {
                            if(admin == 1)
                            {
                                INPUT_INFO(0);    
                            }else{
                                pass += 1;
                            }
                            KeyCounter = 0;
                            BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                            break;
                        }
                        case face_cal:
                        {
                            if(calmode == mode_load)
                            {
                                LoadVCal(0);
                            }else if(calmode == mode_pow){
                                
                            }else if(calmode == mode_r ){
                                Rlow_cal(0);
                            }else if(calmode == input){
                                inputcal("0");
                            }
                            KeyCounter = 0;
                            BEEP_Tiggr();//瑙﹀彂铚傞福鍣�
                        }break;
                    }
                }
                break;
                
                case KEY_dian ://
                {
                    switch(page_sw){
                        case face_menu:
                        {
                            INPUT_POW(".");
                            KeyCounter = 0;
                            BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                            break;
                        }
                        case face_set:
                        {
                            INPUT_NUM(".");
                            KeyCounter = 0;
                            BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                            break;
                        }
                        case face_load:
                        {
                            INPUT_LOAD(".");
                            KeyCounter = 0;
                            BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                            break;
                        }
                        case face_r:
                        {
                            if(para_set2 == set_2_on)
                            {
                                INPUT_C(".");
                                KeyCounter = 0;
                                BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                                break;
                            }
                        }
                        case face_cdc:
                        {
                            INPUT_CDC(".");
                            KeyCounter = 0;
                            BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                            break;
                        }                     
                    }
                }
                break;
                
                case KEY_ESC :
                {
					Flash_Read16BitDatas(FLASH_USER_START_ADDR,20,InFlashSave);
//                  sLCD_WR_REG(0x01);
    //                main();
    //				timer_sw = 0;
                    KeyCounter = 0;
                   BEEP_Tiggr();//瑙﹀彂铚傞福鍣
    //                switch(page_sw){
    //                    case face_r:
    //                    {
    //                         if(oc_mode == 0)
    //                         {
    //                             oc_mode = 1;
    //                         }else{
    //                             oc_mode = 0;
    //                         }
    //                    }break;
    //                    default:sLCD_WR_REG(0x01);
    //                    break;
    //                }
                }
                break;
                
                case PUSH_Lift :
                {
                    vu8 l;
                    switch(page_sw){
                        case face_set:  //绯荤粺璁剧疆
                        {
                            SET_OP_LEFT();  //璁剧疆閫夐」鍒囨崲
                            KeyCounter = 0;
                            BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                            break;
                        }
                        case face_sys_info:
                        {
                            if(admin == 1)
                            {
                                SYS_INFO_LEFT();  //璁剧疆閫夐」鍒囨崲
                            } 
                            KeyCounter = 0;
                            BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                            break;
                        }
                        case face_cdc:  //绯荤粺璁剧疆
                        {
                            if(l != 0)
                            {
                                CDC_OP_LEFT();  //璁剧疆閫夐」鍒囨崲
                            }
                            KeyCounter = 0;
                            BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                            break;
                        }
                    }
                }
                break;
                case PUSH_Right :
                {
                    vu8 l;
                    switch(page_sw){
                        case face_set:  //绯荤粺璁剧疆
                        {
                            SET_OP_RIGHT();  //璁剧疆閫夐」鍒囨崲
                            KeyCounter = 0;
                            BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                            break;
                        }
                        case face_sys_info:
                        {
                            if(admin == 1)
                            {
                                SYS_INFO_RIGHT();  //璁剧疆閫夐」鍒囨崲
                            } 
                            KeyCounter = 0;
                            BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                            break;
                        }
                        case face_cdc:  //绯荤粺璁剧疆
                        {
                            if(l != 0)
                            {
                                CDC_OP_RIGHT();  //璁剧疆閫夐」鍒囨崲
                            }
                            KeyCounter = 0;
                            BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                            break;
                        }
                        
                    }
                }
                break;
                case PUSH_Up :
                {
                    switch(page_sw){
                        case face_menu:
                        {
                            KeyCounter = 0;
                            BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                            MENU_OP_UP();
                            break;
                        }                    
                        case face_load:
                        {
                            KeyCounter = 0;
                            BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                            LOAD_OP_UP();
                            break;
                        }
                        case face_set:
                        {
                            SET_OP_UP();  //璁剧疆閫夐」鍒囨崲
                            KeyCounter = 0;
                            BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                            break;
                        }
                        case face_r:
                        {
                            if(para_set2 == set_2_on/* && oc_mode ==0*/)
                            {
                                OC_OP_UP();
                                KeyCounter = 0;
                                BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                                break;
                            }

                        }
                        case face_cdc:
                        {
                            CDC_OP_UP();
                            KeyCounter = 0;
                            BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                            break;
                        }
                        case face_sys_info:
                        {
                            if(admin == 1)
                            {
                                SYS_INFO_UP();  //璁剧疆閫夐」鍒囨崲
                            }                        
                            KeyCounter = 0;
                            BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                            break;
                        }

                    }
                }
                break;
                case PUSH_Down :
                {
                    u8 i;
                    switch(page_sw){
                        case face_menu:
                        {
                            KeyCounter = 0;
                            BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                            MENU_OP_DOWN();
                            break;
                        }  
                        case face_load:
                        {
                            KeyCounter = 0;
                            BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                            LOAD_OP_DOWN();
                            break;
                        }
                        case face_set:  //绯荤粺璁剧疆
                        {
                            SET_OP_DOWN();  //璁剧疆閫夐」鍒囨崲
                            KeyCounter = 0;
                            BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                            break;
                        }
                        case face_r:
                        {
                            if(para_set2 == set_2_on/* && oc_mode ==0*/)
                            {
                                OC_OP_DOWN();
                                KeyCounter = 0;
                                BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                                break;
                            }
                        }
                        case face_cdc:
                        {
                            CDC_OP_DOWN();
                            KeyCounter = 0;
                            BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                            break;
                        }
                        case face_sys_info:
                        {
                            if(admin == 1)
                            {
                                SYS_INFO_DOWN();  //璁剧疆閫夐」鍒囨崲
                            } 
                            KeyCounter = 0;
                            BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                            break;
                        }
                        case face_cal:
                        {
                            if(calmode != input)
                            {
                                oldcal = calmode;
                                calmode = input;
                                for(i=0;i<10;i++)
                                {
                                    inputv[i] = '\0';
                                }
                            }
                            KeyCounter = 0;
                            BEEP_Tiggr();//瑙﹀彂铚傞福鍣�
                        }break;
                    }
                }
                break;
                
                case ENTER ://
                {
                    switch(page_sw){
                        case face_menu:
                        {
                            MENU_SET();
                            KeyCounter = 0;
                            BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                            break;
                        }  
                        case face_set:
                        {
                            PARA_SET();  //鍙傛暟璁剧疆
                            KeyCounter = 0;
                            BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                            break;                        
                        }
                        case face_r:
                        {
                            if(para_set2 == set_2_on)
                            {
                                OC_SET();
                                KeyCounter = 0;
                                BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                                break;
                            }
                        }
                        case face_load:
                        {
                            LOAD_SET();
                            KeyCounter = 0;
                            BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                            break;
                        }
                        case face_cdc:
                        {
                            CDC_SET();
                            KeyCounter = 0;
                            BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                            break;
                        }
                        case face_sys_info:
                        {
                            CFM_PASS();
                            KeyCounter = 0;
                            BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                            break;
                        }
                        case face_cal:
                        {
                            if(calmode == mode_r)
                            {
                                if(r_raly == 1)
                                {
                                    GPIO_ResetBits(GPIOB,GPIO_Pin_13);//R_RALY浣庢。浣� 
                                    r_raly = 0;
                                }else{
                                     GPIO_SetBits(GPIOB,GPIO_Pin_13);//R_RALY楂樻。浣�
                                    r_raly = 1;
                                }
                            }else if(calmode == input){
                                confirmcal();
                            }
                            KeyCounter = 0;
                            BEEP_Tiggr();//瑙﹀彂铚傞福鍣�
                        }break;
                    }
                }
                break;
                case PUSH_DISP :
                {
                    switch(track)
                    {
                        case face_r:
                        {
                            WM_DeleteWindow(hWinR);
                            WM_DeleteWindow(hWinWind);
                            WM_DeleteWindow(hWinG);
                            WM_DeleteWindow(load_wind);
                            WM_DeleteWindow(hWinsysinfo);
                            WM_DeleteWindow(hWincdc);
                            WM_DeleteWindow(hWinset);                       
                            CreateR();
                            KeyCounter = 0;
                            BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                            break;
                        }
                        case face_load:
                        {
                            WM_DeleteWindow(hWinR);
                            WM_DeleteWindow(hWinWind);
                            WM_DeleteWindow(hWinG);
                            WM_DeleteWindow(load_wind);
                            WM_DeleteWindow(hWinsysinfo);
                            WM_DeleteWindow(hWincdc);
                            WM_DeleteWindow(hWinset);
                            CreateWindow2();
                            KeyCounter = 0;
                            BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                            break;
                        }
                        case face_menu:
                        {
                            WM_DeleteWindow(hWinR);
                            WM_DeleteWindow(hWinWind);
                            WM_DeleteWindow(hWinG);
                            WM_DeleteWindow(load_wind);
                            WM_DeleteWindow(hWinsysinfo);
                            WM_DeleteWindow(hWincdc);
                            WM_DeleteWindow(hWinset); 
                            CreateWindow();
                            KeyCounter = 0;
                            BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                            break;
                        }
                        case face_graph:
                        {
                            WM_DeleteWindow(hWinR);
                            WM_DeleteWindow(hWinWind);
                            WM_DeleteWindow(hWinG);
                            WM_DeleteWindow(load_wind);
                            WM_DeleteWindow(hWinsysinfo);
                            WM_DeleteWindow(hWincdc);
                            WM_DeleteWindow(hWinset); 
                            CreateG();
                            KeyCounter = 0;
                            BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                            break;
                        }
                        case face_cdc:
                        {
                            WM_DeleteWindow(hWinR);
                            WM_DeleteWindow(hWinWind);
                            WM_DeleteWindow(hWinG);
                            WM_DeleteWindow(load_wind);
                            WM_DeleteWindow(hWinsysinfo);
                            WM_DeleteWindow(hWincdc);
                            WM_DeleteWindow(hWinset);
                            CreateCDC();
                            KeyCounter = 0;
                            BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                            break;
                        }
                    }
                }
                break;
                case PUSH_SETUP :
                {
    // 				KeyCounter = 0;
    // 				BEEP_Tiggr();//
    // 				t_MODE++;
    // 				if(t_MODE>1)
    // 				{
    // 					t_MODE=0;
    // 				}
    // 				if(t_MODE==0)
    // 				{
    // 					flag_Load_CC=1;//CC模式
    // 					GPIO_ResetBits(GPIOC,GPIO_Pin_12);//CC
    // 					
    // 				}
    // 				else if(t_MODE==1)
    // 				{
    // 					flag_Load_CC=0;//CV模式
    // 					GPIO_SetBits(GPIOC,GPIO_Pin_12);//CV
    // 				}
                    WM_DeleteWindow(hWinR);
                    WM_DeleteWindow(hWinWind);
                    WM_DeleteWindow(hWinG);
                    WM_DeleteWindow(load_wind);
                    WM_DeleteWindow(hWinsysinfo);
                    WM_DeleteWindow(hWincdc);
                    WM_DeleteWindow(hWinset); 
                    CreateSET();
                    KeyCounter = 0;
                    BEEP_Tiggr();//瑙﹀彂铚傞福鍣
                }
                break;
                case KEY_TRIG://电子负载/电源ON/OFF
                {
                    switch(page_sw)
                    {
                        case face_r:
                        {
                             if(test_start == 0 && DISS_Voltage > gate_v)
                             {								 
                                test_start = 1;
								r = R_VLUE;
								v = DISS_Voltage;
								if(staticcdc == 1)
								{
									step = 1;
								}else{
									step = 4;
									SET_Current_Laod = set_init_c;
									GPIO_ResetBits(GPIOA,GPIO_Pin_15);
								}
                             }else if(test_start == 1){
                                test_start = 0;
								step = 0;
								con_flag = 0;
                             }
                             KeyCounter = 0;
                             BEEP_Tiggr();//触发蜂鸣器
                             break;
                        }
                        case face_load:
                        {
    //                         t_onoff++;
    //                         if(t_onoff>1)
    //                         {
    //                             t_onoff=0;
    //                         }
                            if(load_sw==load_on)
                            {
                                Flag_Swtich_ON=0;
                                GPIO_SetBits(GPIOA,GPIO_Pin_15);//电子负载OFF
                                c_rec = 0;
                                mode_sw = 0;
                                load_sw = load_off;
                            }
                            else if(load_sw==load_off)
                            {
                                Flag_Swtich_ON=1;
                                GPIO_ResetBits(GPIOA,GPIO_Pin_15);//电子负载On
                                if(flag_Load_CC == 0)
                                {
                                    c_rec = 1;
                                }
                                mode_sw = mode_load;
                                load_sw = load_on;
                            }
                            KeyCounter = 0;
                            BEEP_Tiggr();//
                        }break;
                        case face_menu:
                        {
    //                         static vu8 POW_t;
    //                         POW_t++;
    //                         if(POW_t>1)
    //                         {
    //                             POW_t=0;
    //                         }
                            if(pow_sw==pow_on)
                            {
                                GPIO_ResetBits(GPIOC,GPIO_Pin_1);//关闭电源输出
                                GPIO_SetBits(GPIOC,GPIO_Pin_13);//关闭电源输出继电器
                                mode_sw = 0;
                                pow_sw = pow_off;
                            }
                            else if(pow_sw==pow_off)
                            {
                               GPIO_ResetBits(GPIOC,GPIO_Pin_13);//打开电源输出继电器
                                GPIO_SetBits(GPIOC,GPIO_Pin_1);//打开电源输出                           
                                mode_sw = mode_pow;
                                pow_sw = pow_on;
                            }
                            KeyCounter = 0;
                            BEEP_Tiggr();//
                        }break;
                         case face_cdc:
                        {
                            if(cdc_sw == cdc_off)
                            {
                               SET_Voltage = opv1;
                               SET_Current = opc1;
                               cutoff_flag = 0;
    //                           Mode_SW_CONT(0x03);
                               
                               charge_step = 1;
                               GPIO_ResetBits(GPIOC,GPIO_Pin_13);//打开电源输出继电器
                               GPIO_SetBits(GPIOC,GPIO_Pin_1);//打开电源输出
                               mode_sw = mode_pow;
                               cdc_sw = cdc_on;
                            }else{
                               
                               GPIO_ResetBits(GPIOC,GPIO_Pin_1);//关闭电源输出
                               GPIO_SetBits(GPIOC,GPIO_Pin_13);//关闭电源输出继电器
                               GPIO_SetBits(GPIOA,GPIO_Pin_15);//电子负载OFF
                               cdc_sw = cdc_off;
                               paused = 0;
                               mode_sw = 0;
                            }
                            KeyCounter = 0;
                            BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                           
                        }break;
                        case face_cal:
                        {
                            if(calmode == mode_pow)
                            {
                                if(calstep == 1)
                                {
                                    PowVCal(1);
                                }else if(calstep == 2){
                                    PowVCal(2);
                                }else if(calstep == 3){
                                    PowVCal(3);
                                }else if(calstep == 4){
                                    PowVCal(4);
                                }else if(calstep == 5){
                                    PowVCal(5);
                                }else if(calstep == 6){
                                    PowVCal(6);
                                }
                            }else if(calmode == mode_loadc){
                                if(calstep == 1)
                                {
                                    LoadCCal(1);
                                }else if(calstep == 2){
                                    LoadCCal(2);
                                }
                            }else if(calmode == mode_powc){
                                if(calstep == 1)
                                {
                                    PowCCal(1);
                                }else if(calstep == 2){
                                    PowCCal(2);
                                }
                            }
                            KeyCounter = 0;
                            BEEP_Tiggr();//
                        }break;
                    }
                    
                }
                break;
                case KEY_Reset://电阻高低档位切换
                {
                    static vu8 at = 0;
                    switch(page_sw)
                    {
                        case face_r:
                        {
                            if(at == 0)
                            {
                                GPIO_SetBits(GPIOC,GPIO_Pin_13);//关闭电源输出继电器
                                GPIO_ResetBits(GPIOB,GPIO_Pin_13);//R_RALY低档位                           
                                at++;
                                r_raly=0;
                                manual = 1;//手动换档
                            }else if(at == 1){
                                GPIO_SetBits(GPIOC,GPIO_Pin_13);//关闭电源输出继电器
                                GPIO_SetBits(GPIOB,GPIO_Pin_13);//R_RALY高档位
                                r_raly=1;
                                at++;
                                manual = 1;//手动换档
                            }else if(at == 2){
                                at = 0;
                                manual =0;
                            }
                        }
                        case face_cal:
                        {
                            if(calmode == mode_load)
                            {
                                calmode = mode_loadc;
                                IO_OFF();
                            }else if(calmode == mode_loadc){ 
                                calmode = mode_pow;
                                IO_OFF();
                            }else if(calmode == mode_pow){ 
                                calmode = mode_powc;
                                IO_OFF();
                            }else if(calmode == mode_powc){ 
                                calmode = mode_r;
                                IO_OFF();
                            }else if(calmode == mode_r){ 
                                calmode = mode_load;
                                IO_OFF();
                            }
                        }break;
                    }
                    KeyCounter = 0;
                    BEEP_Tiggr();//
                }
                break;
    // 			case KEY_Powe :
    // 			{
    // 				
    // 				KeyCounter = 0;
    // 				BEEP_Tiggr();//
    // 			}
    // 			break;
                case KEY_Lock :
                {
                    lock = 1;
                    KeyCounter = 0;
                    BEEP_Tiggr();//
                }break;
                case KEY_Face1 :
                {
                    WM_DeleteWindow(hWinR);
                    WM_DeleteWindow(hWinWind);
                    WM_DeleteWindow(hWinG);
                    WM_DeleteWindow(load_wind);
                    WM_DeleteWindow(hWinsysinfo);
                    WM_DeleteWindow(hWincdc);
                    WM_DeleteWindow(hWinset);
                    GPIO_SetBits(GPIOA,GPIO_Pin_15);//电子负载OFF
                    GPIO_SetBits(GPIOC,GPIO_Pin_13);//关闭电源输出继电器
                    GPIO_ResetBits(GPIOC,GPIO_Pin_1);//关闭电源输出
                    CreateR();
                    KeyCounter = 0;
                    BEEP_Tiggr();//
                }
                break;
                case KEY_Face2 :
                {
                    WM_DeleteWindow(hWinR);
                    WM_DeleteWindow(hWinWind);
                    WM_DeleteWindow(hWinG);
                    WM_DeleteWindow(load_wind);
                    WM_DeleteWindow(hWinsysinfo);
                    WM_DeleteWindow(hWincdc);
                    WM_DeleteWindow(hWinset);
                    CreateWindow2();
                    t_onoff = 0;
                    GPIO_SetBits(GPIOC,GPIO_Pin_13);//关闭电源输出继电器
                    GPIO_ResetBits(GPIOC,GPIO_Pin_1);//关闭电源输出
                    KeyCounter = 0;
                    BEEP_Tiggr();//
                }
                break;
                case KEY_Face3 :
                {
                    WM_DeleteWindow(hWinR);
                    WM_DeleteWindow(hWinWind);
                    WM_DeleteWindow(hWinG);
                    WM_DeleteWindow(load_wind);
                    WM_DeleteWindow(hWinsysinfo);
                    WM_DeleteWindow(hWincdc);
                    WM_DeleteWindow(hWinset);
                    CreateWindow();
                    GPIO_SetBits(GPIOA,GPIO_Pin_15);//电子负载OFF
                    KeyCounter = 0;
                    BEEP_Tiggr();//
                }
                break;
                case KEY_Face4 :
                {
                    WM_DeleteWindow(hWinR);
                    WM_DeleteWindow(hWinWind);
                    WM_DeleteWindow(hWinG);
                    WM_DeleteWindow(load_wind);
                    WM_DeleteWindow(hWinsysinfo);
                    WM_DeleteWindow(hWincdc);
                    WM_DeleteWindow(hWinset);
                    CreateCDC(); 
                    KeyCounter = 0;
                    BEEP_Tiggr();//
                }
                break;
                case KEY_Face5 :
                {
                    WM_DeleteWindow(hWinR);
                    WM_DeleteWindow(hWinWind);
                    WM_DeleteWindow(hWinG);
                    WM_DeleteWindow(load_wind);
                    WM_DeleteWindow(hWinsysinfo);
                    WM_DeleteWindow(hWincdc);
                    WM_DeleteWindow(hWinset);
                    CreateG();
                    KeyCounter = 0;
                    BEEP_Tiggr();//
                }
                break;
                case KEY_BIAS :
                {
                    WM_DeleteWindow(hWinR);
                    WM_DeleteWindow(hWinWind);
                    WM_DeleteWindow(hWinG);
                    WM_DeleteWindow(load_wind);
                    WM_DeleteWindow(hWinsysinfo);
                    WM_DeleteWindow(hWincdc);
                    WM_DeleteWindow(hWinset);
                    Createsysinfo();
                    KeyCounter = 0;
                    BEEP_Tiggr();//瑙﹀彂铚傞福鍣?
                }
                break;

                        
                
                default:
                break;
            }
        }else{
            switch(KeyValue)
            {
                case KEY_Lock :
                {
                    lock = 0;
                    usartocflag = 0;
                    KeyCounter = 0;
                    BEEP_Tiggr();//
                }break;
            }
        } 
	}
}


void setmode_r(void)
{
    GPIO_ResetBits(GPIOC,GPIO_Pin_1);//关闭电源输出
    GPIO_SetBits(GPIOC,GPIO_Pin_13);//关闭电源输出继电器  
    GPIO_SetBits(GPIOA,GPIO_Pin_15);//电子负载OFF  
    pow_sw = pow_off;
    load_sw = load_off;
    mode_sw = mode_r;
}
const GUI_POINT aPoints[] = {
{ 0, 0},
{ 13, 0},
{ 13, 9},
{ 0, 9}
};

void DrawLock(void)
{
    if(lock == 1)
    {
        GUI_SetColor(GUI_RED);
        GUI_SetPenSize(2);
        GUI_DrawArc( 390,12,6,3,0, 180 );
        GUI_DrawPolygon(aPoints,4,383,12);
    }else{
        
    }
}

void DrawLockc(void)
{
    if(lock == 1)
    {
        GUI_SetColor(GUI_RED);
        GUI_SetPenSize(2);
        GUI_DrawArc( 405+60,12,6,3,0, 180 );
        GUI_DrawPolygon(aPoints,4,398+60,12);
    }else{
        
    }
}
void IO_OFF(void)
{
    GPIO_ResetBits(GPIOC,GPIO_Pin_1);//关闭电源输出
    GPIO_SetBits(GPIOC,GPIO_Pin_13);//关闭电源输出继电器  
    GPIO_SetBits(GPIOA,GPIO_Pin_15);//电子负载OFF  
    pow_sw = pow_off;
    load_sw = load_off;
}

