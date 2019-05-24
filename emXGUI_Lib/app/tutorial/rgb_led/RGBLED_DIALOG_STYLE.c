/**
  ******************************************************************
  * @file    GUI_RGBLED_STYLE.c
  * @author  fire
  * @version V1.0
  * @date    2019-xx-xx
  * @brief   RGB灯测试窗口控件的风格，包含每个控件的重绘函数
  ******************************************************************
  * @attention
  *
  * 实验平台:野火  i.MXRT1052开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************
  */

#include "emXGUI.h"
#include "GUI_RGBLED_DIALOG.h"
#include "RGBLED_DIALOG_STYLE.h"

/**
  * @brief  Button控件的重绘制
  * @param  ds:DRAWITEM_HDR结构体
  * @retval NULL
  */
void GUI_RGBLED_ButtonOwnerDraw(DRAWITEM_HDR *ds) 
{
	HWND hwnd;
	HDC hdc;
	RECT rc;
	WCHAR wbuf[128];

	hwnd = ds->hwnd; //button的窗口句柄.
	hdc = ds->hDC;   //button的绘图上下文句柄.
	rc = ds->rc;     //button的绘制矩形区.
 
  SetTextColor(hdc, MapARGB(hdc, 255,255, 255, 255));
   //按钮按下状态
  if (ds->State & BST_PUSHED)
	{ 
		SetBrushColor(hdc, MapARGB(hdc, 255,105,105,105));
	}
	else//按钮弹起状态
	{ 
		SetBrushColor(hdc, MapARGB(hdc, 255,215,61,50));
	}
  FillCircle(hdc, rc.x+rc.w, rc.y, rc.w);
   /* 使用控制图标字体 */
	SetFont(hdc, ctrlFont64);
	GetWindowText(hwnd, wbuf, 128); //获得按钮控件的文字
  rc.y = -10;
  rc.x = 16;
	DrawText(hdc, wbuf, -1, &rc, NULL);//绘制文字(居中对齐方式)

  /* 恢复默认字体 */
	SetFont(hdc, defaultFont);
}

/**
  * @brief  LED灯开关重绘函数
  * @param  ds:DRAWITEM_HDR结构体
  * @param  state:控件的状态（选中/未选中）
  * @retval NULL
  */
void GUI_RGBLED_CheckBoxOwnerDraw(DRAWITEM_HDR *ds, BOOL state)
{
  HDC hdc; //控件窗口HDC
  HWND hwnd; //控件句柄 
  RECT rc_cli;//控件的位置大小矩形
  WCHAR wbuf[128];
  hwnd = ds->hwnd;
  hdc = ds->hDC; 
  GetClientRect(hwnd, &rc_cli);
  //创建缓冲层，格式为SURF_ARGB4444
   
	GetWindowText(ds->hwnd,wbuf,128); //获得按钮控件的文字  
	if((ds->State & BST_PUSHED))//按钮是按下状态
	{ 
    EnableAlpha(hdc,TRUE);
    EnableAntiAlias(hdc, TRUE);
    SetAlpha(hdc, 128);
    SetBrushColor(hdc,MapRGB(hdc, 185,218,251));
    FillCircle(hdc, rc_cli.x + rc_cli.w/2, rc_cli.y + rc_cli.w/2, rc_cli.w/2);       
    EnableAlpha(hdc,FALSE);
    SetBrushColor(hdc,MapRGB(hdc,  185,218,251));
    FillCircle(hdc, rc_cli.x + rc_cli.w/2, rc_cli.y + rc_cli.w/2, rc_cli.w/2-5);      
    EnableAntiAlias(hdc, FALSE);
	}
  else
  {
    EnableAntiAlias(hdc, TRUE);//抗锯齿
    if(state == TRUE)
    {
       EnableAlpha(hdc,TRUE);
       SetAlpha(hdc, 128);
       SetBrushColor(hdc,MapRGB(hdc, 50,205,50));
       FillCircle(hdc, rc_cli.x + rc_cli.w/2, rc_cli.y + rc_cli.w/2, rc_cli.w/2); 
       
       EnableAlpha(hdc,FALSE);
       SetBrushColor(hdc,MapRGB(hdc,  50,205,50));
       
       FillCircle(hdc, rc_cli.x + rc_cli.w/2, rc_cli.y + rc_cli.w/2, rc_cli.w/2-5);                 
    }
    else
    {
       EnableAlpha(hdc,TRUE);
       SetAlpha(hdc, 128);
       
       SetBrushColor(hdc,MapRGB(hdc, 220,20,60));
       FillCircle(hdc, rc_cli.x + rc_cli.w/2, rc_cli.y + rc_cli.w/2, rc_cli.w/2); 
       
       EnableAlpha(hdc,FALSE);
       SetBrushColor(hdc,MapRGB(hdc,  220,20,60));
       FillCircle(hdc, rc_cli.x + rc_cli.w/2, rc_cli.y + rc_cli.w/2, rc_cli.w/2-5);          
    }
    EnableAntiAlias(hdc, FALSE);
  }
  SetFont(hdc, ctrlFont32);
  SetTextColor(hdc, MapRGB(hdc, 255,255,255));      //设置文字色
  rc_cli.x = 35;
  rc_cli.y = 55;
  if(state == TRUE)
  { 
    DrawText(hdc, L"op",-1,&rc_cli,NULL);//绘制文字(居中对齐方式)           
  }
  else
  {     
    DrawText(hdc, L"np",-1,&rc_cli,NULL);//绘制文字(居中对齐方式)         
  }      
}

/**
  * @brief  文本框控件的重绘制
  * @param  ds:DRAWITEM_HDR结构体
  * @param  hdc_mem:APP背景层
  * @retval NULL
  */
void GUI_RGBLED_TextBoxOwnerDraw(DRAWITEM_HDR *ds)
{
   HDC hdc; //控件窗口HDC
   HWND hwnd; //控件句柄 
   RECT rc_cli, rc_tmp;//控件的位置大小矩形
   WCHAR wbuf[128];
   hwnd = ds->hwnd;
	 hdc = ds->hDC; 
    
   //获取控件的位置大小信息
   GetClientRect(hwnd, &rc_cli);
   GetClientRect(hwnd, &rc_tmp);//得到控件的位置
   //GetClientRect(hwnd, &rc_cli);//得到控件的位置
   WindowToScreen(hwnd, (POINT *)&rc_tmp, 1);//坐标转换
   HDC hdc_mem = Get_BackGroudHDC(); 
   BitBlt(hdc, rc_cli.x, rc_cli.y, rc_cli.w, rc_cli.h, hdc_mem, rc_tmp.x, rc_tmp.y, SRCCOPY);  
  
   
   //创建缓冲层，格式为SURF_ARGB4444
	 GetWindowText(ds->hwnd,wbuf,128); //获得按钮控件的文字  
   //EnableAlpha(hdc,TRUE);

   //设置按键的颜色
   SetTextColor(hdc, MapRGB(hdc,250,250,250));
   //NEXT键、BACK键和LIST键按下时，改变颜色

 
   DrawText(hdc, wbuf,-1,&rc_cli,DT_VCENTER|DT_CENTER);//绘制文字(居中对齐方式)
   
}

/*    
 * @brief  绘制滚动条
 * @param  hwnd:   滚动条的句柄值
 * @param  hdc:    绘图上下文
 * @param  Page_c: 滚动条Page处的颜色
 * @param  fore_c：滚动条滑块的颜色
 * @retval NONE
*/
static void GUI_RGBLED_drawscrollbar_V(HWND hwnd, HDC hdc, COLOR_RGB32 Page_c, COLOR_RGB32 fore_c)
{
  RECT rc, rc_tmp;
  RECT rc_scrollbar;

  GetClientRect(hwnd, &rc_tmp);//得到控件的位置
  GetClientRect(hwnd, &rc);//得到控件的位置
  WindowToScreen(hwnd, (POINT *)&rc_tmp, 1);//坐标转换
 
//  SetBrushColor(hdc_mem, MapRGB(hdc, 0, 0, 0));
//  FillRect(hdc_mem, &rc_tmp);
  BitBlt(hdc, rc.x, rc.y, rc.w, rc.h, Get_BackGroudHDC(), rc_tmp.x, rc_tmp.y, SRCCOPY); 
  rc_scrollbar.x = rc.w/2;
  rc_scrollbar.y = rc.y;
  rc_scrollbar.w = 2;
  rc_scrollbar.h = rc.h;
  EnableAntiAlias(hdc, TRUE);
  SetBrushColor(hdc, MapRGB888(hdc, Page_c));
  FillRect(hdc, &rc_scrollbar);

	/* 滑块 */
	SendMessage(hwnd, SBM_GETTRACKRECT, 0, (LPARAM)&rc);

	SetBrushColor(hdc, MapRGB(hdc, 169, 169, 169));
//	rc.x += (rc.w >> 2) >> 1;
//	rc.w -= rc.w >> 2;
	/* 边框 */
	FillCircle(hdc, rc.x + rc.h / 2+1, rc.y + rc.h / 2, rc.h / 2);
  InflateRect(&rc, -2, -2);

	SetBrushColor(hdc, MapRGB888(hdc, fore_c));
	FillCircle(hdc, rc.x + rc.h / 2+1, rc.y + rc.h / 2, rc.h / 2);
  EnableAntiAlias(hdc, FALSE);
  
}
/*
 * @brief  自定义回调函数
 * @param  ds:	自定义绘制结构体
 * @R_col,G_col,B_col: 滑动条的颜色分量
 * @retval NONE
*/
void GUI_RGBLED_ScrollbarOwnerDraw_V(DRAWITEM_HDR *ds, RGB_Component_Typedef RGB_Component)
{
	HWND hwnd;
	HDC hdc;
	HDC hdc_mem;
	HDC hdc_mem1;
	RECT rc;
	RECT rc_cli;
	hwnd = ds->hwnd;
	hdc = ds->hDC;
	GetClientRect(hwnd, &rc_cli);

	hdc_mem = CreateMemoryDC(SURF_SCREEN, rc_cli.w, rc_cli.h);
	hdc_mem1 = CreateMemoryDC(SURF_SCREEN, rc_cli.w, rc_cli.h);

	//绘制白色类型的滚动条
	GUI_RGBLED_drawscrollbar_V(hwnd, hdc_mem1, RGB888(250, 250, 250), RGB888(255, 255, 255));
	//绘制绿色类型的滚动条
	switch (ds->ID)
	{
		case eID_SCROLLBAR_R:
		{
			GUI_RGBLED_drawscrollbar_V(hwnd, hdc_mem, RGB888(RGB_Component.R_col, 0, 0), RGB888(RGB_Component.R_col, 0, 0));
			break;
		}
		case eID_SCROLLBAR_G:
		{
			GUI_RGBLED_drawscrollbar_V(hwnd, hdc_mem, RGB888(0, RGB_Component.G_col, 0), RGB888(0, RGB_Component.G_col, 0));
			break;
		}
		case eID_SCROLLBAR_B:
		{
			GUI_RGBLED_drawscrollbar_V(hwnd, hdc_mem, RGB888(0, 0, RGB_Component.B_col), RGB888(0, 0, RGB_Component.B_col));
			break;
		}
	}
   
   
  SendMessage(hwnd, SBM_GETTRACKRECT, 0, (LPARAM)&rc);
	//绘制滑块
	if (ds->State & SST_THUMBTRACK)//按下
	{
		BitBlt(hdc, rc.x, 0, rc.w, rc_cli.h, hdc_mem, rc.x, 0, SRCCOPY);
	}
	else//未选中
	{
		BitBlt(hdc, rc.x, 0, rc.w, rc_cli.h, hdc_mem1, rc.x, 0, SRCCOPY);
	}	
	//上
	BitBlt(hdc, rc_cli.x, rc_cli.y, rc_cli.w, rc.y, hdc_mem, 0, 0, SRCCOPY);
	//下
	BitBlt(hdc, rc_cli.x, rc.y+rc.h, rc_cli.w , rc_cli.h-(rc.y+rc.h), hdc_mem1, 0, rc.y + rc.h, SRCCOPY);

	//释放内存MemoryDC
	DeleteDC(hdc_mem1);
	DeleteDC(hdc_mem);
}

