/**
  *********************************************************************
  * @file    gui_font_port.c
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   字体管理接口
  *********************************************************************
  * @attention
  * 官网    :www.emXGUI.com
  *
  **********************************************************************
  */ 

#include	"emXGUI.h"
#include  "gui_drv_cfg.h"

#include "gui_mem_port.h"
#include "gui_font_port.h"
#include "gui_resource_port.h"

#include "GUI_AppDef.h"

/*===================================================================================*/
/*===================================================================================*/


#include <GUI_Font_XFT.h>

/**
  * @note  若要把字体加入到内部FLASH，
  * 把相应的字体数组文件添加到工程，然后在此处加入声明
  * 并使用XFT_CreateFont创建字体
  */
extern const char GB2312_16_1BPP[];
extern const char GB2312_16_2BPP[];
extern const char GB2312_20_2BPP[];

extern const char GB2312_16_4BPP[];
extern const char GB2312_20_4BPP[];

extern const char ASCII_16_4BPP[];
extern const char ASCII_20_4BPP[];
extern const char ASCII_24_4BPP[];
extern const char ASCII_32_4BPP[];


/* 默认字体 */
HFONT defaultFont =NULL;
HFONT logoFont50 =NULL;
HFONT logoFont100 =NULL;
HFONT logoFont252 =NULL;

HFONT ctrlFont32 =NULL;
HFONT ctrlFont48 =NULL;
HFONT ctrlFont64 =NULL;
HFONT ctrlFont72 =NULL;
HFONT ctrlFont100 =NULL;
/* 默认英文字体 */
HFONT defaultFontEn = NULL;

/* 用于标记是否有资源文件无法找到 */
BOOL res_not_found_flag = FALSE;

/** 
  * @brief  字体参数
  *  为适应不同分辨率的屏幕，使用不同的默认字体 
  *  不需要使用不同字体参数时，直接设置gui_drv_cfg.h文件即可
  *  这些参数用在gui_drv_cfg.h文件的宏GUI_DEFAULT_FONT_EN和GUI_DEFAULT_EXTERN_FONT
*/  
const FONT_PARAM_TypeDef gui_font_param[GUI_LCD_TYPE_NUM] = {
  /* 5寸屏 */
  {   
    .default_en = ASCII_24_4BPP,                /* 默认英文字体，内部flash */
    .default_extern_cn = "GB2312_24_4BPP.xft", /* 默认中文字体，外部 */
    .default_extern_logo50 = "LOGO_50_4BPP.xft",
    .default_extern_logo100 = "APP_ICON_100_100_4BPP.xft",
    .default_extern_logo252 = "APP_ICON_252_252_4BPP.xft",
    
    
    .default_extern_ctrl32 = "CONTROL_ICON_32_32_4BPP.xft",
    .default_extern_ctrl48 = "CONTROL_ICON_48_48_4BPP.xft", 
    .default_extern_ctrl64 = "CONTROL_ICON_64_64_4BPP.xft",    
    .default_extern_ctrl72 = "CONTROL_ICON_72_72_4BPP.xft",
    .default_extern_ctrl100 = "CONTROL_ICON_100_100_4BPP.xft",
  },
  /* 7寸屏 */
  {   
    .default_en = ASCII_24_4BPP,                /* 默认英文字体，内部flash */
    .default_extern_cn = "GB2312_24_4BPP.xft", /* 默认中文字体，外部 */
  },
  /* 4.3寸屏 */
  {   
    .default_en = ASCII_16_4BPP,                /* 默认英文字体，内部flash */
    .default_extern_cn = "GB2312_20_4BPP.xft", /* 默认中文字体，外部 */
  },
};

#if (GUI_FONT_LOAD_TO_RAM_EN)
  u8 *default_font_buf;
  u8 *logo_font_buf_50;
  u8 *logo_font_buf_100;
  u8 *logo_font_buf_252;
  u8 *ctrl_font_buf_32;
  u8 *ctrl_font_buf_48;
  u8 *ctrl_font_buf_64;
  u8 *ctrl_font_buf_72;
  u8 *ctrl_font_buf_100;
#endif

/*===================================================================================*/
#if (GUI_EXTERN_FONT_EN)

#if (!GUI_RES_DEV_EN)
  #error Use extern must enable macro 'GUI_RES_DEV_EN' first!
#endif

/**
  * @brief  从流媒体加载内容的回调函数
  * @param  buf[out] 存储读取到的数据缓冲区
  * @param  offset 要读取的位置
  * @param  size 要读取的数据大小
  * @param  lParam 调用函数时的自定义参数（用户参数）
  * @retval 读取到的数据大小
  */
static int font_read_data_exFlash(void *buf,int offset,int size,LONG lParam)
{
  /* 本例子中offset是具体字符数据在字体文件中的偏移 
   * lParam 是字体文件在FLASH中的基地址
  */
	offset += lParam;
  
  /* 读取具体的字模数据内容 */
	RES_DevRead(buf,offset,size);
	return size;
}

/**
  * @brief  初始化外部FLASH字体(流设备方式)
  * @param  res_name 字体资源名字
  * @retval 返回默认字体的句柄
  */
HFONT GUI_Init_Extern_Font_Stream(const char* res_name)
{
  /* 使用流设备加载字体，按需要读取 */
  int font_base;
  HFONT hFont = NULL;
  CatalogTypeDef dir;

#if (GUI_APP_BOOT_INTERFACE_EN)
  {
    /* 启动界面的进度条 */
    if(Boot_progbar != NULL)
    {  
      int count = 0;

      /* 更新启动界面的进度条 */
      count = SendMessage(Boot_progbar,PBM_GET_VALUE,TRUE,NULL); 
      count++;
      SendMessage(Boot_progbar,PBM_SET_VALUE,TRUE,count); 
    }
  }    
#endif

  font_base =RES_GetInfo_AbsAddr(res_name, &dir);
  if(font_base > 0)
  {
    hFont =XFT_CreateFontEx(font_read_data_exFlash,font_base);
  }
  else
  {
    res_not_found_flag = TRUE;
    GUI_ERROR("Can not find RES:%s",res_name);
  }
  
  if(hFont==NULL)
  {
    res_not_found_flag = TRUE;    
    GUI_ERROR("%s font create failed",res_name);
  }
  return hFont;
}



/**
  * @brief  初始化外部FLASH字体（整体加载到SDRAM）
  * @param  res_name[in] 字体资源名字
  * @param  buf[out]：字体资源复制到的缓冲区，
  *         可以通过它free掉占用的空间，但注意释放后使用字体的话会出错
  * @retval 返回默认字体的句柄
  */
HFONT GUI_Init_Extern_Font_2RAM(const char* res_name,u8** buf)
{
    /* 整个字体文件加载至RAM */
    int font_base; 
    HFONT hFont = NULL;  
    CatalogTypeDef dir;
#if (GUI_APP_BOOT_INTERFACE_EN)
  {
    /* 启动界面的进度条 */
    if(Boot_progbar != NULL)
    {  
      int count = 0;

      /* 更新启动界面的进度条 */
      count = SendMessage(Boot_progbar,PBM_GET_VALUE,TRUE,NULL); 
      count++;
      SendMessage(Boot_progbar,PBM_SET_VALUE,TRUE,count); 
    }  
  }
#endif
  
    /* RES_GetInfo读取到的dir.offset是资源的绝对地址 */
    font_base =RES_GetInfo_AbsAddr(res_name, &dir);

    if(font_base > 0)
    {
    	*buf =(u8*)GUI_VMEM_Alloc(dir.size);
      if(*buf!=NULL)
      {
        RES_DevRead((u8*)*buf, font_base, dir.size);

        hFont = XFT_CreateFont(*buf);
      }
    }
    else
    {
      res_not_found_flag = TRUE;
      GUI_ERROR("Can not find RES:%s",res_name);
    }
    
    if(hFont==NULL)
    {
      res_not_found_flag = TRUE;    
      GUI_ERROR("%s font create failed",res_name);
    }
    
   return hFont;
}


/**
  * @brief  加载外部字体
  * @retval 返回重新创建的defaultFont
  */
HFONT GUI_Init_Extern_Font(void)
{
   /* 整个字体文件加载至RAM */
#if (GUI_FONT_LOAD_TO_RAM_EN  )
  {  
    defaultFont = GUI_Init_Extern_Font_2RAM(GUI_DEFAULT_EXTERN_FONT,&default_font_buf);
    logoFont50 =  GUI_Init_Extern_Font_2RAM(GUI_LOGO_FONT_50,&logo_font_buf_50);
    logoFont100 =  GUI_Init_Extern_Font_2RAM(GUI_LOGO_FONT_100,&logo_font_buf_100);   
    logoFont252 =  GUI_Init_Extern_Font_2RAM(GUI_LOGO_FONT_252,&logo_font_buf_100);
    
    
    ctrlFont32 =  GUI_Init_Extern_Font_2RAM(GUI_CTRL_FONT_32,&ctrl_font_buf_32);
    ctrlFont48 =  GUI_Init_Extern_Font_2RAM(GUI_CTRL_FONT_48,&ctrl_font_buf_48);
    
    ctrlFont64 =  GUI_Init_Extern_Font_2RAM(GUI_CTRL_FONT_64,&ctrl_font_buf_64);
    
    ctrlFont72 =  GUI_Init_Extern_Font_2RAM(GUI_CTRL_FONT_72,&ctrl_font_buf_72); 
    ctrlFont100 =  GUI_Init_Extern_Font_2RAM(GUI_CTRL_FONT_100,&ctrl_font_buf_100);    
  }
  
#else
   /* 使用流设备加载字体，按需要读取 */
  {
    defaultFont =GUI_Init_Extern_Font_Stream(GUI_DEFAULT_EXTERN_FONT);  
  }
#endif 
 
  return defaultFont;
}

#endif


/**
  * @brief  GUI默认字体初始化
  * @param  无
  * @retval 返回默认字体的句柄
  */
HFONT GUI_Default_FontInit(void)
{
    /* 若前面的字体加载失败，使用内部FLASH中的数据（工程中的C语言数组）
    *  添加字体数据时，把数组文件添加到工程，在本文件头添加相应字体数组的声明，
    *  然后调用XFT_CreateFont函数创建字体即可
    */
  
    /* 默认英文字体 */ 
    defaultFontEn = XFT_CreateFont(ASCII_16_4BPP);        

    /* 如果使用了启动界面，在启动界面再加载外部字体 */
#if (GUI_EXTERN_FONT_EN && (!GUI_APP_BOOT_INTERFACE_EN))
 
    /* 从外部资源设备加载字体 */
    defaultFont = GUI_Init_Extern_Font();  
  
#elif (GUI_INER_CN_FONT_EN && (!GUI_APP_BOOT_INTERFACE_EN))
    /* 中文字库存储占用空间非常大，不推荐放在内部FLASH */
    /* 从内部flash加载默认中文字体，不推荐*/
    defaultFont = XFT_CreateFont(GUI_DEFAULT_FONT_CN);    
#endif
 
/* 不使用外部字体时，默认把Load_state至1 */  
#if ((!GUI_EXTERN_FONT_EN) || (!GUI_APP_BOOT_INTERFACE_EN)) 
    Load_state = TRUE;
#endif  
  
    /* 中文字体创建失败时使用英文字体作为默认字体 */
    if(defaultFont==NULL)
    { 
      defaultFont = defaultFontEn;  
    }
    
	return defaultFont;
}

/********************************END OF FILE****************************/

