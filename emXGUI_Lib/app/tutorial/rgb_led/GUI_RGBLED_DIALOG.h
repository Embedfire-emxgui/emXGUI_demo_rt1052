#ifndef _GUI_RGBLED_DIALOG_H_
#define _GUI_RGBLED_DIALOG_H_

#define MEM_DETECT       (1)



enum GUI_RGBLEDID
{
   /*滑动条ID宏定义*/
   eID_SCROLLBAR_R = 0x1100,
   eID_SCROLLBAR_G  ,
   eID_SCROLLBAR_B ,

   /*文本框ID宏定义*/
   eID_TEXTBOX_Title,
   eID_TEXTBOX_APP,
   eID_TEXTBOX_R,
  
   eID_TEXTBOX_R_LED,   
   eID_TEXTBOX_R_NUM,
   eID_TEXTBOX_G,	 
  
   eID_TEXTBOX_G_LED,   
   eID_TEXTBOX_G_NUM,  
   eID_TEXTBOX_B,
  
   eID_TEXTBOX_B_LED,   
   eID_TEXTBOX_B_NUM,  
   /*按钮ID宏定义*/
   eID_EXIT,  
   eID_CHECKBOX_SW, 
};

enum RGBLED_Message
{
  Upadate_Mess =  WM_USER+1,
  LEDOff_Mess,
  LEDOn_Mess,
};

typedef struct
{
  uint8_t R_col;
  uint8_t G_col;
  uint8_t B_col;
}RGB_Component_Typedef;

typedef struct
{
  HWND hwnd;
  HDC hdc_back;//背景层
  
#if MEM_DETECT 
  UINT cur_size;
#endif  
  
  
  BOOL state;//当前led灯的状态
  
  SCROLLINFO sif_R;//红色分量值
  SCROLLINFO sif_G;//红色分量值
  SCROLLINFO sif_B;//红色分量值 
  
  RGB_Component_Typedef RGB_Component;
  
  void(*m_button_draw)(DRAWITEM_HDR *);
  void(*m_checkbox_draw)(DRAWITEM_HDR *, BOOL);
  void(*m_textbox_draw)(DRAWITEM_HDR *);
  void(*m_scrollbar_draw)(DRAWITEM_HDR *, RGB_Component_Typedef);
  void(*m_hardware_init)(void);
  void(*m_hardware_deinit)(void);
  void(*m_SetColorValue)(uint8_t,uint8_t,uint8_t);
  
}RGBLED_DIALOG_Typedef;

void Set_BackGroudHDC(HDC hdc_background);
HDC Get_BackGroudHDC(void);

#endif

