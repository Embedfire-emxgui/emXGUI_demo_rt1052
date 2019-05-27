#ifndef _GUI_RGBLED_DIALOG_H_
#define _GUI_RGBLED_DIALOG_H_

#define MEM_DETECT       (1)



enum GUI_RGBLEDID
{
   /*������ID�궨��*/
   eID_SCROLLBAR_R = 0x1100,
   eID_SCROLLBAR_G  ,
   eID_SCROLLBAR_B ,

   /*�ı���ID�궨��*/
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
   /*��ťID�궨��*/
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
  HDC hdc_back;//������
  
#if MEM_DETECT 
  UINT cur_size;
#endif  
  
  
  BOOL state;//��ǰled�Ƶ�״̬
  
  SCROLLINFO sif_R;//��ɫ����ֵ
  SCROLLINFO sif_G;//��ɫ����ֵ
  SCROLLINFO sif_B;//��ɫ����ֵ 
  
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

