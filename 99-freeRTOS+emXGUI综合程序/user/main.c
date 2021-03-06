/**
  ******************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   ELCDIF—液晶显示（显示英文）
  ******************************************************************
  * @attention
  *
  * 实验平台:野火  i.MXRT1052开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************
  */
#include <stdio.h>

#include "fsl_debug_console.h"
#include "fsl_elcdif.h"
#include "host_gamepad.h"
#include "usb_host_app.h"

#include "board.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "./systick/bsp_systick.h"
#include "./pit/bsp_pit.h"
#include "./delay/core_delay.h"   
#include "./led/bsp_led.h" 
#include "./key/bsp_key.h" 
#include "./lcd/bsp_lcd.h" 
#include "./touch/bsp_touch_gtxx.h"
#include "./touch/bsp_i2c_touch.h"
#include "./touch/palette.h"
#include "./bsp/sd_fatfs_test/bsp_sd_fatfs_test.h"
/* FreeRTOS头文件 */
#include "FreeRTOS.h"
#include "task.h"

/******************************* 全局变量声明 ************************************/
/*
 * 当我们在写应用程序的时候，可能需要用到一些全局变量。
 */


/*
*************************************************************************
*                             函数声明
*************************************************************************
*/
static void GUI_Thread_Entry(void* pvParameters);/* Test_Task任务实现 */

static void BSP_Init(void);/* 用于初始化板载相关资源 */
static void CPU_Task(void);
/*****************************************************************
  * @brief  主函数
  * @param  无
  * @retval 无
  * @note   第一步：开发板硬件初始化 
            第二步：创建APP应用任务
            第三步：启动FreeRTOS，开始多任务调度
  ****************************************************************/
static void sdad(void *param)
{
    while (1)
    {
      CPU_Task();
       //printf("%d", eTaskGetState(xTaskGetHandle("GUI_APP")));
       GUI_msleep(1000); 
    }
}

//static void USB_HostApplicationMouseTask(void *param)
//{
//    while (1)
//    {
//        USB_HostHidGamepad1Task(param);
//    }
//}
#include "fsl_sai_edma.h"
/*****************************************************************
  * @brief  主函数
  * @param  无
  * @retval 无
  * @note   第一步：开发板硬件初始化 
            第二步：创建APP应用任务
            第三步：启动FreeRTOS，开始多任务调度
  ****************************************************************/
int main(void)
{	
  BaseType_t xReturn = pdPASS;/* 定义一个创建信息返回值，默认为pdPASS */

  /* 开发板硬件初始化 */
  BSP_Init();  
//  SoundTest();
   /* 创建AppTaskCreate任务 */
  xReturn = xTaskCreate((TaskFunction_t )GUI_Thread_Entry,  /* 任务入口函数 */
                        (const char*    )"gui",/* 任务名字 */
                        (uint16_t       )8*1024,  /* 任务栈大小 */
                        (void*          )NULL,/* 任务入口函数参数 */
                        (UBaseType_t    )6, /* 任务的优先级 */
                        (TaskHandle_t*  )NULL);/* 任务控制块指针 */ 
                        
//                        
//    if (xTaskCreate(sdad, "usdahost task", 2000L / sizeof(portSTACK_TYPE), NULL, 4, NULL) != pdPASS)
//    {
//        usb_echo("create host task error\r\n");
//    }
//    if (xTaskCreate(USB_HostApplicationMouseTask, "mouse task", 2000L / sizeof(portSTACK_TYPE), &g_HostHidGamepad1, 3,
//                    NULL) != pdPASS)
//    {
//        usb_echo("create mouse task error\r\n");
//    }                        
  /* 启动任务调度 */           
  if(1)//(pdPASS == xReturn)
    vTaskStartScheduler();   /* 启动任务，开启调度 */
  else
    return -1;  
  
  while(1);   /* 正常不会执行到这里 */    
}


extern void GUI_Startup(void);
extern uint8_t Gamepad_state;
int y;
/**********************************************************************
  * @ 函数名  ： gui_thread_entry
  * @ 功能说明： gui_thread_entry任务主体
  * @ 参数    ：   
  * @ 返回值  ： 无
  ********************************************************************/
static void GUI_Thread_Entry(void* parameter)
{	
  
  printf("野火emXGUI演示例程\n\n");
//  USB_HostApplicationInit();
//  USB_HostTaskFn(g_HostHandle); 
//  USB_HostHidGamepad1Task(&g_HostHidGamepad1);  
  /* 执行本函数不会返回 */
	GUI_Startup();
  
  while (1)
  {
//    LED1_ON;
    printf("Test_Task Running,LED1_ON\r\n");
    vTaskDelay(500);   /* 延时500个tick */
    
//    LED1_OFF;     
    printf("Test_Task Running,LED1_OFF\r\n");
    vTaskDelay(500);   /* 延时500个tick */
  }
}
FIL file_object;   //定义文件描述符，
FATFS g_fileSystem; /* File system object */
/***********************************************************************
  * @ 函数名  ： BSP_Init
  * @ 功能说明： 板级外设初始化，所有板子上的初始化均可放在这个函数里面
  * @ 参数    ：   
  * @ 返回值  ： 无
  *********************************************************************/
static void BSP_Init(void)
{	
#if (!CPU_TS_INIT_IN_DELAY_FUNCTION)      
      //使用时间戳延时函数前必须先使能计数器
      CPU_TS_TmrInit();
#endif
	  /* 初始化内存保护单元 */
    BOARD_ConfigMPU();
		/* 初始化开发板引脚 */
    BOARD_InitPins();
		/* 初始化开发板时钟 */
    BOARD_BootClockRUN();
		/* 初始化调试串口 */
		BOARD_InitDebugConsole();
  
    /*RT1052不支持无子优先级的中断分组，按照port.c的770行代码相关的注释，
      调用NVIC_SetPriorityGrouping(0)设置中断优先级分组*/
    NVIC_SetPriorityGrouping(0); 
  
		/* 打印系统时钟 */
		PRINTF("\r\n");
		PRINTF("*****欢迎使用 野火i.MX RT1052 开发板*****\r\n");
		PRINTF("CPU:             %d Hz\r\n", CLOCK_GetFreq(kCLOCK_CpuClk));
		PRINTF("AHB:             %d Hz\r\n", CLOCK_GetFreq(kCLOCK_AhbClk));
		PRINTF("SEMC:            %d Hz\r\n", CLOCK_GetFreq(kCLOCK_SemcClk));
		PRINTF("SYSPLL:          %d Hz\r\n", CLOCK_GetFreq(kCLOCK_SysPllClk));
		PRINTF("SYSPLLPFD0:      %d Hz\r\n", CLOCK_GetFreq(kCLOCK_SysPllPfd0Clk));
		PRINTF("SYSPLLPFD1:      %d Hz\r\n", CLOCK_GetFreq(kCLOCK_SysPllPfd1Clk));
		PRINTF("SYSPLLPFD2:      %d Hz\r\n", CLOCK_GetFreq(kCLOCK_SysPllPfd2Clk));
		PRINTF("SYSPLLPFD3:      %d Hz\r\n", CLOCK_GetFreq(kCLOCK_SysPllPfd3Clk));	

  	PRINTF("*****液晶显示英文*****\r\n");

    /* 初始化SysTick */
    SysTick_Config(SystemCoreClock / configTICK_RATE_HZ);
//    hard_init();
//    
////    SoundTest();
    /* 初始化LED */
//    LED_GPIO_Config();
    
    Key_GPIO_Config();
//    
    USB_HostApplicationInit();
    /* 触摸初始化 */
//    GTP_Init_Panel();

    /* 初始化LCD */
//    LCD_Init(LCD_INTERRUPT_ENABLE);    
    /*挂载文件系统*/
//    f_mount_test(&g_fileSystem);
//    f_touch_test("/dir_1/he.txt"); 
//    /*打开文件*/
//    f_open_test("/dir_1/he.txt",&file_object);
    /*初始化PIT定时器*/
    PIT_TIMER_Init();
    
    /*开启定时器*/
    PIT_StartTimer(PIT, PIT_CHANNEL_X);   
     /*关闭文件*/
//    f_close_test(&file_object);    
//    RGB_LED_COLOR_BLUE;    
//    CORE_BOARD_LED_ON;
    
    /*调用画板函数*/
//    Palette_Init();
}
static void CPU_Task(void)
{
	uint8_t CPU_RunInfo[400]; //淇濆瓨浠诲姟杩愯鏃堕棿淇℃伅

	memset(CPU_RunInfo,0,400); //淇℃伅缂撳啿鍖烘竻闆?

  vTaskList((char *)&CPU_RunInfo); //获取任务运行时间信息

//   printf("---------------------------------------------\r\n");
//   printf("任务名 任务状态 优先级 剩余栈 任务序号\r\n");
//   printf("%s", CPU_RunInfo);
//   printf("---------------------------------------------\r\n");

//	vTaskGetRunTimeStats((char *)&CPU_RunInfo);

//	printf("任务名 运行计数 使用率\r\n");
//	printf("%s", CPU_RunInfo);
//	printf("---------------------------------------------\r\n\n");

}
void vApplicationIdleHook( void )
{
  //printf("%d", eTaskGetState(xTaskGetHandle("GUI_APP")));
  //CPU_Task();
}
/****************************END OF FILE**********************/
