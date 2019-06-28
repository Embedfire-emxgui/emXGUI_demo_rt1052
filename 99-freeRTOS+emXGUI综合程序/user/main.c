/**
  ******************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   ELCDIF��Һ����ʾ����ʾӢ�ģ�
  ******************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��  i.MXRT1052������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :http://firestm32.taobao.com
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
/* FreeRTOSͷ�ļ� */
#include "FreeRTOS.h"
#include "task.h"

/******************************* ȫ�ֱ������� ************************************/
/*
 * ��������дӦ�ó����ʱ�򣬿�����Ҫ�õ�һЩȫ�ֱ�����
 */


/*
*************************************************************************
*                             ��������
*************************************************************************
*/
static void GUI_Thread_Entry(void* pvParameters);/* Test_Task����ʵ�� */

static void BSP_Init(void);/* ���ڳ�ʼ�����������Դ */
static void CPU_Task(void);
/*****************************************************************
  * @brief  ������
  * @param  ��
  * @retval ��
  * @note   ��һ����������Ӳ����ʼ�� 
            �ڶ���������APPӦ������
            ������������FreeRTOS����ʼ���������
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
  * @brief  ������
  * @param  ��
  * @retval ��
  * @note   ��һ����������Ӳ����ʼ�� 
            �ڶ���������APPӦ������
            ������������FreeRTOS����ʼ���������
  ****************************************************************/
int main(void)
{	
  BaseType_t xReturn = pdPASS;/* ����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS */

  /* ������Ӳ����ʼ�� */
  BSP_Init();  
//  SoundTest();
   /* ����AppTaskCreate���� */
  xReturn = xTaskCreate((TaskFunction_t )GUI_Thread_Entry,  /* ������ں��� */
                        (const char*    )"gui",/* �������� */
                        (uint16_t       )8*1024,  /* ����ջ��С */
                        (void*          )NULL,/* ������ں������� */
                        (UBaseType_t    )6, /* ��������ȼ� */
                        (TaskHandle_t*  )NULL);/* ������ƿ�ָ�� */ 
                        
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
  /* ����������� */           
  if(1)//(pdPASS == xReturn)
    vTaskStartScheduler();   /* �������񣬿������� */
  else
    return -1;  
  
  while(1);   /* ��������ִ�е����� */    
}


extern void GUI_Startup(void);
extern uint8_t Gamepad_state;
int y;
/**********************************************************************
  * @ ������  �� gui_thread_entry
  * @ ����˵���� gui_thread_entry��������
  * @ ����    ��   
  * @ ����ֵ  �� ��
  ********************************************************************/
static void GUI_Thread_Entry(void* parameter)
{	
  
  printf("Ұ��emXGUI��ʾ����\n\n");
//  USB_HostApplicationInit();
//  USB_HostTaskFn(g_HostHandle); 
//  USB_HostHidGamepad1Task(&g_HostHidGamepad1);  
  /* ִ�б��������᷵�� */
	GUI_Startup();
  
  while (1)
  {
    LED1_ON;
    printf("Test_Task Running,LED1_ON\r\n");
    vTaskDelay(500);   /* ��ʱ500��tick */
    
    LED1_OFF;     
    printf("Test_Task Running,LED1_OFF\r\n");
    vTaskDelay(500);   /* ��ʱ500��tick */
  }
}
FIL file_object;   //�����ļ���������
FATFS g_fileSystem; /* File system object */
/***********************************************************************
  * @ ������  �� BSP_Init
  * @ ����˵���� �弶�����ʼ�������а����ϵĳ�ʼ�����ɷ��������������
  * @ ����    ��   
  * @ ����ֵ  �� ��
  *********************************************************************/
static void BSP_Init(void)
{	
#if (!CPU_TS_INIT_IN_DELAY_FUNCTION)      
      //ʹ��ʱ�����ʱ����ǰ������ʹ�ܼ�����
      CPU_TS_TmrInit();
#endif
	  /* ��ʼ���ڴ汣����Ԫ */
    BOARD_ConfigMPU();
		/* ��ʼ������������ */
    BOARD_InitPins();
		/* ��ʼ��������ʱ�� */
    BOARD_BootClockRUN();
		/* ��ʼ�����Դ��� */
		BOARD_InitDebugConsole();
  
    /*RT1052��֧���������ȼ����жϷ��飬����port.c��770�д�����ص�ע�ͣ�
      ����NVIC_SetPriorityGrouping(0)�����ж����ȼ�����*/
    NVIC_SetPriorityGrouping(0); 
  
		/* ��ӡϵͳʱ�� */
		PRINTF("\r\n");
		PRINTF("*****��ӭʹ�� Ұ��i.MX RT1052 ������*****\r\n");
		PRINTF("CPU:             %d Hz\r\n", CLOCK_GetFreq(kCLOCK_CpuClk));
		PRINTF("AHB:             %d Hz\r\n", CLOCK_GetFreq(kCLOCK_AhbClk));
		PRINTF("SEMC:            %d Hz\r\n", CLOCK_GetFreq(kCLOCK_SemcClk));
		PRINTF("SYSPLL:          %d Hz\r\n", CLOCK_GetFreq(kCLOCK_SysPllClk));
		PRINTF("SYSPLLPFD0:      %d Hz\r\n", CLOCK_GetFreq(kCLOCK_SysPllPfd0Clk));
		PRINTF("SYSPLLPFD1:      %d Hz\r\n", CLOCK_GetFreq(kCLOCK_SysPllPfd1Clk));
		PRINTF("SYSPLLPFD2:      %d Hz\r\n", CLOCK_GetFreq(kCLOCK_SysPllPfd2Clk));
		PRINTF("SYSPLLPFD3:      %d Hz\r\n", CLOCK_GetFreq(kCLOCK_SysPllPfd3Clk));	

  	PRINTF("*****Һ����ʾӢ��*****\r\n");

    /* ��ʼ��SysTick */
    SysTick_Config(SystemCoreClock / configTICK_RATE_HZ);
//    hard_init();
//    
////    SoundTest();
    /* ��ʼ��LED */
//    LED_GPIO_Config();
    
    Key_GPIO_Config();
//    
    USB_HostApplicationInit();
    /* ������ʼ�� */
//    GTP_Init_Panel();

    /* ��ʼ��LCD */
//    LCD_Init(LCD_INTERRUPT_ENABLE);    
    /*�����ļ�ϵͳ*/
//    f_mount_test(&g_fileSystem);
//    f_touch_test("/dir_1/he.txt"); 
//    /*���ļ�*/
//    f_open_test("/dir_1/he.txt",&file_object);
    /*��ʼ��PIT��ʱ��*/
    PIT_TIMER_Init();
    
    /*������ʱ��*/
    PIT_StartTimer(PIT, PIT_CHANNEL_X);   
     /*�ر��ļ�*/
//    f_close_test(&file_object);    
//    RGB_LED_COLOR_BLUE;    
//    CORE_BOARD_LED_ON;
    AudioTest();
    /*���û��庯��*/
//    Palette_Init();
}
static void CPU_Task(void)
{
	uint8_t CPU_RunInfo[400]; //保存任务运行时间信息

	memset(CPU_RunInfo,0,400); //信息缓冲区清�?

  vTaskList((char *)&CPU_RunInfo); //��ȡ��������ʱ����Ϣ

//   printf("---------------------------------------------\r\n");
//   printf("������ ����״̬ ���ȼ� ʣ��ջ �������\r\n");
//   printf("%s", CPU_RunInfo);
//   printf("---------------------------------------------\r\n");

//	vTaskGetRunTimeStats((char *)&CPU_RunInfo);

//	printf("������ ���м��� ʹ����\r\n");
//	printf("%s", CPU_RunInfo);
//	printf("---------------------------------------------\r\n\n");

}
void vApplicationIdleHook( void )
{
  //printf("%d", eTaskGetState(xTaskGetHandle("GUI_APP")));
  //CPU_Task();
}
/****************************END OF FILE**********************/
