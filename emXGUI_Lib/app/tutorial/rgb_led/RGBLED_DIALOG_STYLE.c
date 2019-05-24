/**
  ******************************************************************
  * @file    GUI_RGBLED_STYLE.c
  * @author  fire
  * @version V1.0
  * @date    2019-xx-xx
  * @brief   RGB�Ʋ��Դ��ڿؼ��ķ�񣬰���ÿ���ؼ����ػ溯��
  ******************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��  i.MXRT1052������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************
  */

#include "emXGUI.h"
#include "GUI_RGBLED_DIALOG.h"
#include "RGBLED_DIALOG_STYLE.h"

/**
  * @brief  Button�ؼ����ػ���
  * @param  ds:DRAWITEM_HDR�ṹ��
  * @retval NULL
  */
void GUI_RGBLED_ButtonOwnerDraw(DRAWITEM_HDR *ds) 
{
	HWND hwnd;
	HDC hdc;
	RECT rc;
	WCHAR wbuf[128];

	hwnd = ds->hwnd; //button�Ĵ��ھ��.
	hdc = ds->hDC;   //button�Ļ�ͼ�����ľ��.
	rc = ds->rc;     //button�Ļ��ƾ�����.
 
  SetTextColor(hdc, MapARGB(hdc, 255,255, 255, 255));
   //��ť����״̬
  if (ds->State & BST_PUSHED)
	{ 
		SetBrushColor(hdc, MapARGB(hdc, 255,105,105,105));
	}
	else//��ť����״̬
	{ 
		SetBrushColor(hdc, MapARGB(hdc, 255,215,61,50));
	}
  FillCircle(hdc, rc.x+rc.w, rc.y, rc.w);
   /* ʹ�ÿ���ͼ������ */
	SetFont(hdc, ctrlFont64);
	GetWindowText(hwnd, wbuf, 128); //��ð�ť�ؼ�������
  rc.y = -10;
  rc.x = 16;
	DrawText(hdc, wbuf, -1, &rc, NULL);//��������(���ж��뷽ʽ)

  /* �ָ�Ĭ������ */
	SetFont(hdc, defaultFont);
}

/**
  * @brief  LED�ƿ����ػ溯��
  * @param  ds:DRAWITEM_HDR�ṹ��
  * @param  state:�ؼ���״̬��ѡ��/δѡ�У�
  * @retval NULL
  */
void GUI_RGBLED_CheckBoxOwnerDraw(DRAWITEM_HDR *ds, BOOL state)
{
  HDC hdc; //�ؼ�����HDC
  HWND hwnd; //�ؼ���� 
  RECT rc_cli;//�ؼ���λ�ô�С����
  WCHAR wbuf[128];
  hwnd = ds->hwnd;
  hdc = ds->hDC; 
  GetClientRect(hwnd, &rc_cli);
  //��������㣬��ʽΪSURF_ARGB4444
   
	GetWindowText(ds->hwnd,wbuf,128); //��ð�ť�ؼ�������  
	if((ds->State & BST_PUSHED))//��ť�ǰ���״̬
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
    EnableAntiAlias(hdc, TRUE);//�����
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
  SetTextColor(hdc, MapRGB(hdc, 255,255,255));      //��������ɫ
  rc_cli.x = 35;
  rc_cli.y = 55;
  if(state == TRUE)
  { 
    DrawText(hdc, L"op",-1,&rc_cli,NULL);//��������(���ж��뷽ʽ)           
  }
  else
  {     
    DrawText(hdc, L"np",-1,&rc_cli,NULL);//��������(���ж��뷽ʽ)         
  }      
}

/**
  * @brief  �ı���ؼ����ػ���
  * @param  ds:DRAWITEM_HDR�ṹ��
  * @param  hdc_mem:APP������
  * @retval NULL
  */
void GUI_RGBLED_TextBoxOwnerDraw(DRAWITEM_HDR *ds)
{
   HDC hdc; //�ؼ�����HDC
   HWND hwnd; //�ؼ���� 
   RECT rc_cli, rc_tmp;//�ؼ���λ�ô�С����
   WCHAR wbuf[128];
   hwnd = ds->hwnd;
	 hdc = ds->hDC; 
    
   //��ȡ�ؼ���λ�ô�С��Ϣ
   GetClientRect(hwnd, &rc_cli);
   GetClientRect(hwnd, &rc_tmp);//�õ��ؼ���λ��
   //GetClientRect(hwnd, &rc_cli);//�õ��ؼ���λ��
   WindowToScreen(hwnd, (POINT *)&rc_tmp, 1);//����ת��
   HDC hdc_mem = Get_BackGroudHDC(); 
   BitBlt(hdc, rc_cli.x, rc_cli.y, rc_cli.w, rc_cli.h, hdc_mem, rc_tmp.x, rc_tmp.y, SRCCOPY);  
  
   
   //��������㣬��ʽΪSURF_ARGB4444
	 GetWindowText(ds->hwnd,wbuf,128); //��ð�ť�ؼ�������  
   //EnableAlpha(hdc,TRUE);

   //���ð�������ɫ
   SetTextColor(hdc, MapRGB(hdc,250,250,250));
   //NEXT����BACK����LIST������ʱ���ı���ɫ

 
   DrawText(hdc, wbuf,-1,&rc_cli,DT_VCENTER|DT_CENTER);//��������(���ж��뷽ʽ)
   
}

/*    
 * @brief  ���ƹ�����
 * @param  hwnd:   �������ľ��ֵ
 * @param  hdc:    ��ͼ������
 * @param  Page_c: ������Page������ɫ
 * @param  fore_c���������������ɫ
 * @retval NONE
*/
static void GUI_RGBLED_drawscrollbar_V(HWND hwnd, HDC hdc, COLOR_RGB32 Page_c, COLOR_RGB32 fore_c)
{
  RECT rc, rc_tmp;
  RECT rc_scrollbar;

  GetClientRect(hwnd, &rc_tmp);//�õ��ؼ���λ��
  GetClientRect(hwnd, &rc);//�õ��ؼ���λ��
  WindowToScreen(hwnd, (POINT *)&rc_tmp, 1);//����ת��
 
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

	/* ���� */
	SendMessage(hwnd, SBM_GETTRACKRECT, 0, (LPARAM)&rc);

	SetBrushColor(hdc, MapRGB(hdc, 169, 169, 169));
//	rc.x += (rc.w >> 2) >> 1;
//	rc.w -= rc.w >> 2;
	/* �߿� */
	FillCircle(hdc, rc.x + rc.h / 2+1, rc.y + rc.h / 2, rc.h / 2);
  InflateRect(&rc, -2, -2);

	SetBrushColor(hdc, MapRGB888(hdc, fore_c));
	FillCircle(hdc, rc.x + rc.h / 2+1, rc.y + rc.h / 2, rc.h / 2);
  EnableAntiAlias(hdc, FALSE);
  
}
/*
 * @brief  �Զ���ص�����
 * @param  ds:	�Զ�����ƽṹ��
 * @R_col,G_col,B_col: ����������ɫ����
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

	//���ư�ɫ���͵Ĺ�����
	GUI_RGBLED_drawscrollbar_V(hwnd, hdc_mem1, RGB888(250, 250, 250), RGB888(255, 255, 255));
	//������ɫ���͵Ĺ�����
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
	//���ƻ���
	if (ds->State & SST_THUMBTRACK)//����
	{
		BitBlt(hdc, rc.x, 0, rc.w, rc_cli.h, hdc_mem, rc.x, 0, SRCCOPY);
	}
	else//δѡ��
	{
		BitBlt(hdc, rc.x, 0, rc.w, rc_cli.h, hdc_mem1, rc.x, 0, SRCCOPY);
	}	
	//��
	BitBlt(hdc, rc_cli.x, rc_cli.y, rc_cli.w, rc.y, hdc_mem, 0, 0, SRCCOPY);
	//��
	BitBlt(hdc, rc_cli.x, rc.y+rc.h, rc_cli.w , rc_cli.h-(rc.y+rc.h), hdc_mem1, 0, rc.y + rc.h, SRCCOPY);

	//�ͷ��ڴ�MemoryDC
	DeleteDC(hdc_mem1);
	DeleteDC(hdc_mem);
}

