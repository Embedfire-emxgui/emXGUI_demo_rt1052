#ifndef _RGBLED_DIALOG_STYLE_H_
#define _RGBLED_DIALOG_STYLE_H_

#include "emXGUI.h"
#include "RGBLED_DIALOG_BACKEND.h"
//APP背景（800*480）
#define GUI_RGB_BACKGROUNG_PIC      "desktop.jpg"


//退出按钮的位置大小
#define EXIT_POS_X         GUI_XSIZE
#define EXIT_POS_Y         0
#define EXIT_POS_W         70
#define EXIT_POS_H         70
//LED灯开关的位置大小:125 125 160 160
#define SW_POS_X           (GUI_XSIZE-125)
#define SW_POS_Y           (GUI_YSIZE-125)
#define SW_POS_W           (GUI_YSIZE/3)
#define SW_POS_H           (GUI_YSIZE/3)
//红灯位置大小
#define RLED_POS_X         (93)
#define RLED_POS_Y         (80)
#define RLED_POS_W         (72)
#define RLED_POS_H         (72)
//绿灯位置大小
#define GLED_POS_X         (222)
#define GLED_POS_Y         (80)
#define GLED_POS_W         (72)
#define GLED_POS_H         (72)
//蓝灯位置大小
#define BLED_POS_X         (350)
#define BLED_POS_Y         (80)
#define BLED_POS_W         (72)
#define BLED_POS_H         (72)

#define SBAR_Y             (155)
#define SBAR_W             (35)
#define SBAR_H             (200)

//红灯滑动条位置大小
#define RSBAR_POS_X         (113)
#define RSBAR_POS_Y         (SBAR_Y)
#define RSBAR_POS_W         (SBAR_W)
#define RSBAR_POS_H         (SBAR_H)
//绿灯滑动条位置大小
#define GSBAR_POS_X         (242)
#define GSBAR_POS_Y         (SBAR_Y)
#define GSBAR_POS_W         (SBAR_W)
#define GSBAR_POS_H         (SBAR_H)
//蓝灯滑动条位置大小
#define BSBAR_POS_X         (370)
#define BSBAR_POS_Y         (SBAR_Y)
#define BSBAR_POS_W         (SBAR_W)
#define BSBAR_POS_H         (SBAR_H)
/*****************************************/
#define TEXT_POS_Y          (420)
#define TEXT_POS_W          (72)
#define TEXT_POS_H          (50)
//“红灯”位置大小
#define RTEXT_POS_X         (96)
#define RTEXT_POS_Y         (TEXT_POS_Y)
#define RTEXT_POS_W         (TEXT_POS_W)
#define RTEXT_POS_H         (TEXT_POS_H)
//“绿灯”位置大小
#define GTEXT_POS_X         (226)
#define GTEXT_POS_Y         (TEXT_POS_Y)
#define GTEXT_POS_W         (TEXT_POS_W)
#define GTEXT_POS_H         (TEXT_POS_H)
//“蓝灯”位置大小
#define BTEXT_POS_X         (356)
#define BTEXT_POS_Y         (TEXT_POS_Y)
#define BTEXT_POS_W         (TEXT_POS_W)
#define BTEXT_POS_H         (TEXT_POS_H)

/**********************************************/
#define NUMS_POS_Y          (355)
#define NUMS_POS_W          (72)
#define NUMS_POS_H          (72)

//红色分量值位置大小
#define RNUMS_POS_X         (96)
#define RNUMS_POS_Y         (NUMS_POS_Y)
#define RNUMS_POS_W         (NUMS_POS_W)
#define RNUMS_POS_H         (NUMS_POS_H)
//绿灯滑动条位置大小
#define GNUMS_POS_X         (226)
#define GNUMS_POS_Y         (NUMS_POS_Y)
#define GNUMS_POS_W         (NUMS_POS_W)
#define GNUMS_POS_H         (NUMS_POS_H)
//蓝灯滑动条位置大小
#define BNUMS_POS_X         (356)
#define BNUMS_POS_Y         (NUMS_POS_Y)
#define BNUMS_POS_W         (NUMS_POS_W)
#define BNUMS_POS_H         (NUMS_POS_H)

/**********************************************/
#define BAPP_POS_X          (425)
#define BAPP_POS_Y          (80)
#define BAPP_POS_W          (275)
#define BAPP_POS_H          (275)



extern void GUI_RGBLED_ButtonOwnerDraw(DRAWITEM_HDR *ds);
extern void GUI_RGBLED_TextBoxOwnerDraw(DRAWITEM_HDR *ds);
extern void GUI_RGBLED_CheckBoxOwnerDraw(DRAWITEM_HDR *ds, BOOL state);
extern void GUI_RGBLED_ScrollbarOwnerDraw_V(DRAWITEM_HDR *ds, RGB_Component_Typedef RGB_Component);
#endif

