/**
  ******************************************************************
  * @file    GUI_RGBLED_DIALOG.c
  * @author  fire
  * @version V1.0
  * @date    2019-xx-xx
  * @brief   RGB�Ʋ��Դ��ڵ�GUI����
  ******************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��  i.MXRT1052������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************
  */
#include "x_libc.h"
#include "emXGUI.h"
#include "GUI_RGBLED_DIALOG.h"
#include "RGBLED_DIALOG_STYLE.h"
#include "emXGUI_JPEG.h"

__align(4) RGBLED_DIALOG_Typedef RGBLED_Dialog = 
{
  .state = TRUE,
  .hdc_back = NULL,
  .m_button_draw = GUI_RGBLED_ButtonOwnerDraw,
  .RGB_Component = {255, 165, 208},
  .m_checkbox_draw = GUI_RGBLED_CheckBoxOwnerDraw,
  .m_textbox_draw = GUI_RGBLED_TextBoxOwnerDraw,
  .m_scrollbar_draw = GUI_RGBLED_ScrollbarOwnerDraw_V,
  .m_SetColorValue = SetColorValue,
  .m_hardware_init = FlexPWM_Init,
  .m_hardware_deinit = FlexPWM_DeInit,
};

static void RGBLED_Dialog_Init(void)
{
  RGBLED_Dialog.state = TRUE;
  
  RGBLED_Dialog.RGB_Component.R_col = 255;
  RGBLED_Dialog.RGB_Component.G_col = 165;
  RGBLED_Dialog.RGB_Component.B_col = 208;
  RGBLED_Dialog.m_button_draw = GUI_RGBLED_ButtonOwnerDraw;
  RGBLED_Dialog.m_checkbox_draw = GUI_RGBLED_CheckBoxOwnerDraw;
  RGBLED_Dialog.m_textbox_draw = GUI_RGBLED_TextBoxOwnerDraw;
  RGBLED_Dialog.m_scrollbar_draw = GUI_RGBLED_ScrollbarOwnerDraw_V; 
  /***********************Ӳ����ز���***************************/
  RGBLED_Dialog.m_SetColorValue = SetColorValue;
  RGBLED_Dialog.m_hardware_init = FlexPWM_Init;
  RGBLED_Dialog.m_hardware_deinit = FlexPWM_DeInit;
  
  FlexPWM_Init();
  
  RGBLED_Dialog.m_SetColorValue(RGBLED_Dialog.RGB_Component.R_col,
                                RGBLED_Dialog.RGB_Component.G_col,
                                RGBLED_Dialog.RGB_Component.B_col);
}

/**
  * @brief  ���ñ���HDC
  * @param  hdc_background:������HDC
  * @retval NULL
  */
void Set_BackGroudHDC(HDC hdc_background)
{
  RGBLED_Dialog.hdc_back = hdc_background;
}
/**
  * @brief  ��ȡ��ǰAPP�ı�����HDC
  * @param  NULL
  * @retval ��ǰ������HDC
  */
HDC Get_BackGroudHDC(void)
{
  return RGBLED_Dialog.hdc_back;
}

/**
  * @brief  APP�������غ���
  * @param  hdc_mem:���������ģ����ڱ�����
  * @param  rc:���ڴ�С
  * @retval NULL
  */
void Load_BackGround(HDC hdc_mem, RECT rc)
{
  BOOL res;
  u8 *jpeg_buf;
  u32 jpeg_size;
  JPG_DEC *dec;  
  //���ر���ͼƬ
  res = RES_Load_Content(GUI_RGB_BACKGROUNG_PIC, (char**)&jpeg_buf, &jpeg_size);      
  //����ȡ�ļ�ʧ�ܣ�����ƺ�ɫ����
  if(res)
  {
    /* ����ͼƬ���ݴ���JPG_DEC��� */
    dec = JPG_Open(jpeg_buf, jpeg_size);

    /* �������ڴ���� */
    JPG_Draw(hdc_mem, 0, 0, dec);

    /* �ر�JPG_DEC��� */
    JPG_Close(dec);
  }
  else
  {
    
    SetBrushColor(hdc_mem, MapRGB(hdc_mem, 250, 250, 250));
    FillRect(hdc_mem, &rc);
    SetTextColor(hdc_mem, MapRGB(hdc_mem, 0, 0, 0));
    DrawText(hdc_mem, L"�Ҳ�������ͼƬ", -1, &rc, DT_TOP | DT_LEFT);
    
  }
  /* �ͷ�ͼƬ���ݿռ� */
  RES_Release_Content((char **)&jpeg_buf);   
}

static LRESULT GUI_LED_PROC(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  static HDC hdc_mem = NULL;
  switch(msg)
  {
    case WM_CREATE:
    {
      WCHAR wbuf[128];
      RECT rc;
      //��ȡ���ڴ�С
      GetClientRect(hwnd, &rc);
      CreateWindow(BUTTON, L"O",WS_TRANSPARENT|WS_OWNERDRAW|WS_VISIBLE,
                   (EXIT_POS_X - EXIT_POS_W), EXIT_POS_Y, EXIT_POS_W, EXIT_POS_H, 
                   hwnd, eID_EXIT, NULL, NULL);     

      CreateWindow(BUTTON,L"��/�ص�",WS_TRANSPARENT|WS_OWNERDRAW|WS_VISIBLE,
                   SW_POS_X, SW_POS_Y, SW_POS_W, SW_POS_H,
                   hwnd,eID_CHECKBOX_SW,NULL,NULL);
      //���
      CreateWindow(TEXTBOX, L"k", WS_TRANSPARENT|WS_VISIBLE, 
                   RLED_POS_X, RLED_POS_Y, RLED_POS_W, RLED_POS_H, 
                  hwnd, eID_TEXTBOX_R_LED, NULL, NULL);
      SetWindowFont(GetDlgItem(hwnd, eID_TEXTBOX_R_LED), ctrlFont72);
      SendMessage(GetDlgItem(hwnd, eID_TEXTBOX_R_LED),TBM_SET_TEXTFLAG,0,
                    DT_SINGLELINE|DT_CENTER|DT_VCENTER); 
      //�̵�            
      CreateWindow(TEXTBOX, L"k", WS_TRANSPARENT|WS_VISIBLE, 
                   GLED_POS_X, GLED_POS_Y, GLED_POS_W, GLED_POS_H,
                  hwnd, eID_TEXTBOX_G_LED, NULL, NULL);
      SetWindowFont(GetDlgItem(hwnd, eID_TEXTBOX_G_LED), ctrlFont72);
      SendMessage(GetDlgItem(hwnd, eID_TEXTBOX_G_LED),TBM_SET_TEXTFLAG,0,
                    DT_SINGLELINE|DT_CENTER|DT_VCENTER); 
      //����              
      CreateWindow(TEXTBOX, L"k", WS_TRANSPARENT|WS_VISIBLE, 
                   BLED_POS_X, BLED_POS_Y, BLED_POS_W, BLED_POS_H, 
                   hwnd, eID_TEXTBOX_B_LED, NULL, NULL);
      SetWindowFont(GetDlgItem(hwnd, eID_TEXTBOX_B_LED), ctrlFont72);         
      SendMessage(GetDlgItem(hwnd, eID_TEXTBOX_B_LED),TBM_SET_TEXTFLAG,0,
                    DT_SINGLELINE|DT_CENTER|DT_VCENTER); 

      RGBLED_Dialog.sif_R.cbSize = sizeof(RGBLED_Dialog.sif_R);
      RGBLED_Dialog.sif_R.fMask = SIF_ALL;
      RGBLED_Dialog.sif_R.nMin = 0;
      RGBLED_Dialog.sif_R.nMax = 255;
      RGBLED_Dialog.sif_R.nValue = RGBLED_Dialog.RGB_Component.R_col;
      RGBLED_Dialog.sif_R.TrackSize = 35;
      RGBLED_Dialog.sif_R.ArrowSize = 0;

      /*����������--R*/
      CreateWindow(SCROLLBAR, L"SCROLLBAR_R", SBS_VERT|WS_OWNERDRAW |WS_TRANSPARENT| WS_VISIBLE, 
                   RSBAR_POS_X, RSBAR_POS_Y, RSBAR_POS_W, RSBAR_POS_H,
                  hwnd, eID_SCROLLBAR_R, NULL, NULL);
      SendMessage(GetDlgItem(hwnd, eID_SCROLLBAR_R), SBM_SETSCROLLINFO, TRUE, (LPARAM)&RGBLED_Dialog.sif_R);

      RGBLED_Dialog.sif_G.cbSize = sizeof(RGBLED_Dialog.sif_G);
      RGBLED_Dialog.sif_G.fMask = SIF_ALL;
      RGBLED_Dialog.sif_G.nMin = 0;
      RGBLED_Dialog.sif_G.nMax = 255;
      RGBLED_Dialog.sif_G.nValue = RGBLED_Dialog.RGB_Component.G_col;
      RGBLED_Dialog.sif_G.TrackSize = 35;
      RGBLED_Dialog.sif_G.ArrowSize = 0;
      /*����������--G*/
      CreateWindow(SCROLLBAR, L"SCROLLBAR_G", SBS_VERT|WS_OWNERDRAW |WS_TRANSPARENT| WS_VISIBLE, 
                   GSBAR_POS_X,GSBAR_POS_Y,GSBAR_POS_W,GSBAR_POS_H,
                   hwnd, eID_SCROLLBAR_G, NULL, NULL);
      SendMessage(GetDlgItem(hwnd, eID_SCROLLBAR_G), SBM_SETSCROLLINFO, TRUE, (LPARAM)&RGBLED_Dialog.sif_G);

      RGBLED_Dialog.sif_B.cbSize = sizeof(RGBLED_Dialog.sif_B);
      RGBLED_Dialog.sif_B.fMask = SIF_ALL;
      RGBLED_Dialog.sif_B.nMin = 0;
      RGBLED_Dialog.sif_B.nMax = 255;
      RGBLED_Dialog.sif_B.nValue = RGBLED_Dialog.RGB_Component.B_col;
      RGBLED_Dialog.sif_B.TrackSize = 35;
      RGBLED_Dialog.sif_B.ArrowSize = 0;
      /*����������--B*/
      CreateWindow(SCROLLBAR, L"SCROLLBAR_B", SBS_VERT|WS_OWNERDRAW | WS_TRANSPARENT|WS_VISIBLE, 
                    BSBAR_POS_X,BSBAR_POS_Y,BSBAR_POS_W,BSBAR_POS_H,
                    hwnd, eID_SCROLLBAR_B, NULL, NULL);
      SendMessage(GetDlgItem(hwnd, eID_SCROLLBAR_B), SBM_SETSCROLLINFO, TRUE, (LPARAM)&RGBLED_Dialog.sif_B);

      /*�����ı���--���*/
      CreateWindow(BUTTON, L"qp", WS_TRANSPARENT|WS_VISIBLE|WS_OWNERDRAW, 
                    RTEXT_POS_X,RTEXT_POS_Y,RTEXT_POS_W,RTEXT_POS_H,
                  hwnd, eID_TEXTBOX_R, NULL, NULL);
      SendMessage(GetDlgItem(hwnd, eID_TEXTBOX_R),TBM_SET_TEXTFLAG,0,
                    DT_SINGLELINE|DT_CENTER|DT_VCENTER|DT_BKGND); 
      SetWindowFont(GetDlgItem(hwnd, eID_TEXTBOX_R), ctrlFont32);                        
                        
      //�����ı���--�̵�         
      CreateWindow(BUTTON, L"rp", WS_TRANSPARENT|WS_OWNERDRAW|WS_VISIBLE, 
                    GTEXT_POS_X, GTEXT_POS_Y, GTEXT_POS_W, GTEXT_POS_H,
                  hwnd, eID_TEXTBOX_G, NULL, NULL);
      SendMessage(GetDlgItem(hwnd, eID_TEXTBOX_G),TBM_SET_TEXTFLAG,0,
                    DT_SINGLELINE|DT_CENTER|DT_VCENTER|DT_BKGND);   
      SetWindowFont(GetDlgItem(hwnd, eID_TEXTBOX_G), ctrlFont32);  
      //�����ı���--����
      CreateWindow(BUTTON, L"sp", WS_TRANSPARENT|WS_OWNERDRAW|WS_VISIBLE, 
                   BTEXT_POS_X, BTEXT_POS_Y, BTEXT_POS_W, BTEXT_POS_H,                    
                  hwnd, eID_TEXTBOX_B, NULL, NULL);  
      SetWindowFont(GetDlgItem(hwnd, eID_TEXTBOX_B), ctrlFont32);
      //R�ķ���ֵ 
      x_wsprintf(wbuf, L"%d", RGBLED_Dialog.RGB_Component.R_col);
      CreateWindow(BUTTON, wbuf, WS_TRANSPARENT|WS_VISIBLE|WS_OWNERDRAW|WS_DISABLED, 
                   RNUMS_POS_X,RNUMS_POS_Y,RNUMS_POS_W,RNUMS_POS_H,
                  hwnd, eID_TEXTBOX_R_NUM, NULL, NULL); 
      SetWindowFont(GetDlgItem(hwnd, eID_TEXTBOX_R_NUM), ctrlFont32);               
      //G�ķ���ֵ 
      x_wsprintf(wbuf, L"%d", RGBLED_Dialog.RGB_Component.G_col);
      CreateWindow(BUTTON, wbuf, WS_TRANSPARENT|WS_VISIBLE|WS_OWNERDRAW|WS_DISABLED, 
                   GNUMS_POS_X,GNUMS_POS_Y,GNUMS_POS_W,GNUMS_POS_H,        
                  hwnd, eID_TEXTBOX_G_NUM, NULL, NULL);

      SetWindowFont(GetDlgItem(hwnd, eID_TEXTBOX_G_NUM), ctrlFont32);       
      //B�ķ���ֵ         
      x_wsprintf(wbuf, L"%d", RGBLED_Dialog.RGB_Component.B_col);         
      CreateWindow(BUTTON, wbuf, WS_TRANSPARENT|WS_VISIBLE|WS_OWNERDRAW|WS_DISABLED, 
                  BNUMS_POS_X,BNUMS_POS_Y,BNUMS_POS_W,BNUMS_POS_H,
                  hwnd, eID_TEXTBOX_B_NUM, NULL, NULL);
      SetWindowFont(GetDlgItem(hwnd, eID_TEXTBOX_B_NUM), ctrlFont32);   

      CreateWindow(TEXTBOX, L"c", WS_TRANSPARENT|WS_VISIBLE, 
                   BAPP_POS_X, BAPP_POS_Y, BAPP_POS_W, BAPP_POS_H,
                  hwnd, eID_TEXTBOX_APP, NULL, NULL);
      SetWindowFont(GetDlgItem(hwnd, eID_TEXTBOX_APP), logoFont252);
      SendMessage(GetDlgItem(hwnd, eID_TEXTBOX_APP),TBM_SET_TEXTFLAG,0,
                    DT_SINGLELINE|DT_CENTER|DT_VCENTER);

      hdc_mem = CreateMemoryDC(SURF_SCREEN, rc.w, rc.h);
      //���ر���
      Load_BackGround(hdc_mem, rc);
      Set_BackGroudHDC(hdc_mem);
      
      break;
    }
    
    case WM_DRAWITEM:
    {
       DRAWITEM_HDR *ds;
       ds = (DRAWITEM_HDR*)lParam;
       switch(ds->ID)
       {
          case eID_EXIT:
          {              
            RGBLED_Dialog.m_button_draw(ds);
            return TRUE;              
          }
          case eID_CHECKBOX_SW:
          {
            RGBLED_Dialog.m_checkbox_draw(ds, RGBLED_Dialog.state);
            return TRUE;
          }
          case eID_SCROLLBAR_R:
          case eID_SCROLLBAR_G:
          case eID_SCROLLBAR_B:
          {
            RGBLED_Dialog.m_scrollbar_draw(ds, RGBLED_Dialog.RGB_Component);
            return TRUE;
          }
          case eID_TEXTBOX_R:
          case eID_TEXTBOX_G:
          case eID_TEXTBOX_B:
          case eID_TEXTBOX_R_NUM:
          case eID_TEXTBOX_G_NUM:
          case eID_TEXTBOX_B_NUM:      
          {
            RGBLED_Dialog.m_textbox_draw(ds);
            return TRUE;
          }
       }
       break;
    }     
    case WM_NOTIFY: 
    {
      NMHDR *nr;
      u16 ctr_id; 
      u16 code,  id;
      id  =LOWORD(wParam);//��ȡ��Ϣ��ID��
      code=HIWORD(wParam);//��ȡ��Ϣ������
      ctr_id = LOWORD(wParam); //wParam��16λ�Ƿ��͸���Ϣ�Ŀؼ�ID.
      nr = (NMHDR*)lParam; //lParam����������NMHDR�ṹ�忪ͷ.
         
      if(id == eID_EXIT && code == BN_CLICKED)
      {
        PostCloseMessage(hwnd);
      }
      if(ctr_id == eID_SCROLLBAR_R)
      {
        NM_SCROLLBAR *sb_nr;		
        sb_nr = (NM_SCROLLBAR*)nr; //Scrollbar��֪ͨ��Ϣʵ��Ϊ NM_SCROLLBAR��չ�ṹ,���渽���˸������Ϣ.
        switch (nr->code)
        {
           case SBN_THUMBTRACK: //R�����ƶ�
           {
              RGBLED_Dialog.RGB_Component.R_col = sb_nr->nTrackValue; //��û��鵱ǰλ��ֵ
              SendMessage(nr->hwndFrom, SBM_SETVALUE, TRUE, RGBLED_Dialog.RGB_Component.R_col); //����λ��ֵ
      //                  x_wsprintf(wbuf, L"%d", leddlg_S.col_R);
      //                  SetWindowText(GetDlgItem(hwnd, ID_TEXTBOX_R_NUM), wbuf);
      //                  RedrawWindow(GetDlgItem(hwnd, ID_TEXTBOX_R_LED), NULL, RDW_ALLCHILDREN|RDW_INVALIDATE);
           
              SendMessage(hwnd, Upadate_Mess, ctr_id, RGBLED_Dialog.state);
           }
           break;
        }
      }

      if(ctr_id == eID_SCROLLBAR_G)
      {
        NM_SCROLLBAR *sb_nr;
        sb_nr = (NM_SCROLLBAR*)nr; //Scrollbar��֪ͨ��Ϣʵ��Ϊ NM_SCROLLBAR��չ�ṹ,���渽���˸������Ϣ.
        switch (nr->code)
        {
           case SBN_THUMBTRACK: //G�����ƶ�
           {
              RGBLED_Dialog.RGB_Component.G_col = sb_nr->nTrackValue; //��û��鵱ǰλ��ֵ
              SendMessage(nr->hwndFrom, SBM_SETVALUE, TRUE, RGBLED_Dialog.RGB_Component.G_col); //����λ��ֵ
      //                  x_wsprintf(wbuf, L"%d", leddlg_S.col_G);
      //                  SetWindowText(GetDlgItem(hwnd, ID_TEXTBOX_G_NUM), wbuf);
      //                  RedrawWindow(GetDlgItem(hwnd, ID_TEXTBOX_G_LED), NULL, RDW_ALLCHILDREN|RDW_INVALIDATE);
              SendMessage(hwnd, Upadate_Mess, ctr_id, RGBLED_Dialog.state);
           }
           break;
        }
      }

      if(ctr_id == eID_SCROLLBAR_B)
      {
        NM_SCROLLBAR *sb_nr;
        sb_nr = (NM_SCROLLBAR*)nr; //Scrollbar��֪ͨ��Ϣʵ��Ϊ NM_SCROLLBAR��չ�ṹ,���渽���˸������Ϣ.
        switch (nr->code)
        {
           case SBN_THUMBTRACK: //B�����ƶ�
           {
              RGBLED_Dialog.RGB_Component.B_col = sb_nr->nTrackValue; //���B���鵱ǰλ��ֵ
              SendMessage(nr->hwndFrom, SBM_SETVALUE, TRUE, RGBLED_Dialog.RGB_Component.B_col); //����B�����λ��

              //��������
              SendMessage(hwnd, Upadate_Mess, ctr_id, RGBLED_Dialog.state);
           }
           break;
        }
        
      }
      if(id == eID_CHECKBOX_SW)
      {
        if(code == BN_CLICKED)
        {
          RGBLED_Dialog.state = !RGBLED_Dialog.state;
          if(RGBLED_Dialog.state==FALSE)
            SendMessage(hwnd, LEDOff_Mess, NULL, NULL);
          else
            SendMessage(hwnd, LEDOn_Mess, NULL, NULL);
        }
      }
      break;
    }         
    case WM_ERASEBKGND:
    {
      HDC hdc =(HDC)wParam;
      RECT rc =*(RECT*)lParam;
      if(RGBLED_Dialog.state)
        BitBlt(hdc, rc.x, rc.y, rc.w, rc.h, hdc_mem, rc.x, rc.y, SRCCOPY);
      else
      {
        SetBrushColor(hdc, MapRGB(hdc, 0, 0, 0));
        FillRect(hdc, &rc); 
      }
      return TRUE;

    }    
    case WM_CTLCOLOR:
    {
       u16 id;
       id =LOWORD(wParam);         
       CTLCOLOR *cr;
       cr =(CTLCOLOR*)lParam;      
       switch(id)
       {
          case eID_TEXTBOX_APP:
          {
             //cr->BackColor = ID_TEXTBOX_APP_BackColor; 
             //cr->BorderColor = ID_TEXTBOX_APP_BackColor;
             cr->TextColor = RGB888(RGBLED_Dialog.RGB_Component.R_col, 
                                    RGBLED_Dialog.RGB_Component.G_col, 
                                    RGBLED_Dialog.RGB_Component.B_col);
             break;
          }
          case eID_TEXTBOX_R_LED:
          {
             cr->TextColor = RGB888(RGBLED_Dialog.RGB_Component.R_col, 0, 0);
             //cr->BackColor = ID_TEXTBOX_R_LED_BackColor;     
             break;
          }
          case eID_TEXTBOX_G_LED:
          {
             cr->TextColor = RGB888(0, RGBLED_Dialog.RGB_Component.G_col, 0);
//             cr->BackColor = ID_TEXTBOX_G_LED_BackColor;     
             break;
          }
          case eID_TEXTBOX_B_LED:
          {
             cr->TextColor = RGB888(0, 0, RGBLED_Dialog.RGB_Component.B_col);
//             cr->BackColor = ID_TEXTBOX_B_LED_BackColor;     
             break;
          }              
          default:
             return FALSE;
       }    
       return TRUE;      
    } 
    case Upadate_Mess:
    {
      WCHAR wbuf[128];
      u16 ctr_id = LOWORD(wParam);
      //BOOL cur_state = HIWORD(wParam);//��ǰ״̬
      switch(ctr_id)
      {
        case eID_SCROLLBAR_R:
        {
          x_wsprintf(wbuf, L"%d", RGBLED_Dialog.RGB_Component.R_col);
          SetWindowText(GetDlgItem(hwnd, eID_TEXTBOX_R_NUM), wbuf);
          InvalidateRect(GetDlgItem(hwnd, eID_TEXTBOX_R_LED), NULL, TRUE);           
          break;
        }
        case eID_SCROLLBAR_G:
        {
          x_wsprintf(wbuf, L"%d", RGBLED_Dialog.RGB_Component.G_col);
          SetWindowText(GetDlgItem(hwnd, eID_TEXTBOX_G_NUM), wbuf);
          InvalidateRect(GetDlgItem(hwnd, eID_TEXTBOX_G_LED), NULL, TRUE);           
          break;
        }
        case eID_SCROLLBAR_B:
        {
          x_wsprintf(wbuf, L"%d", RGBLED_Dialog.RGB_Component.B_col);
          SetWindowText(GetDlgItem(hwnd, eID_TEXTBOX_B_NUM), wbuf);
          InvalidateRect(GetDlgItem(hwnd, eID_TEXTBOX_B_LED), NULL, TRUE);          
          
          break;
        }        
      }
      InvalidateRect(GetDlgItem(hwnd, eID_TEXTBOX_APP), NULL, TRUE);

      {
        RGBLED_Dialog.m_SetColorValue(RGBLED_Dialog.RGB_Component.R_col,
                                      RGBLED_Dialog.RGB_Component.G_col,
                                      RGBLED_Dialog.RGB_Component.B_col);        
      }
        
      break;
    } 
    case LEDOff_Mess:
    {
      ShowWindow(GetDlgItem(hwnd, eID_TEXTBOX_B_NUM), SW_HIDE); 
      ShowWindow(GetDlgItem(hwnd, eID_TEXTBOX_R_NUM), SW_HIDE);
      ShowWindow(GetDlgItem(hwnd, eID_TEXTBOX_G_NUM), SW_HIDE);

      ShowWindow(GetDlgItem(hwnd, eID_SCROLLBAR_R), SW_HIDE);
      ShowWindow(GetDlgItem(hwnd, eID_SCROLLBAR_G), SW_HIDE);
      ShowWindow(GetDlgItem(hwnd, eID_SCROLLBAR_B), SW_HIDE);


      ShowWindow(GetDlgItem(hwnd, eID_TEXTBOX_Title), SW_HIDE);                   
      ShowWindow(GetDlgItem(hwnd, eID_TEXTBOX_APP), SW_HIDE);

      ShowWindow(GetDlgItem(hwnd, eID_TEXTBOX_R_LED), SW_HIDE); 
      ShowWindow(GetDlgItem(hwnd, eID_TEXTBOX_G_LED), SW_HIDE);
      ShowWindow(GetDlgItem(hwnd, eID_TEXTBOX_B_LED), SW_HIDE);

      ShowWindow(GetDlgItem(hwnd, eID_TEXTBOX_B), SW_HIDE);
      ShowWindow(GetDlgItem(hwnd, eID_TEXTBOX_G), SW_HIDE);
      ShowWindow(GetDlgItem(hwnd, eID_TEXTBOX_R), SW_HIDE); 
      RedrawWindow(hwnd, NULL, RDW_ALLCHILDREN|RDW_INVALIDATE|RDW_ERASE); 
      if(RGBLED_Dialog.state == FALSE)
      {
        RGBLED_Dialog.m_SetColorValue(0, 0, 0);
      }    
      break;
    }
    case LEDOn_Mess:
    {
      ShowWindow(GetDlgItem(hwnd, eID_TEXTBOX_B_NUM), SW_SHOW); 
      ShowWindow(GetDlgItem(hwnd, eID_TEXTBOX_R_NUM), SW_SHOW);
      ShowWindow(GetDlgItem(hwnd, eID_TEXTBOX_G_NUM), SW_SHOW);

      ShowWindow(GetDlgItem(hwnd, eID_SCROLLBAR_R), SW_SHOW);
      ShowWindow(GetDlgItem(hwnd, eID_SCROLLBAR_G), SW_SHOW);
      ShowWindow(GetDlgItem(hwnd, eID_SCROLLBAR_B), SW_SHOW);


      ShowWindow(GetDlgItem(hwnd, eID_TEXTBOX_Title), SW_SHOW);                   
      ShowWindow(GetDlgItem(hwnd, eID_TEXTBOX_APP), SW_SHOW);

      ShowWindow(GetDlgItem(hwnd, eID_TEXTBOX_R_LED), SW_SHOW); 
      ShowWindow(GetDlgItem(hwnd, eID_TEXTBOX_G_LED), SW_SHOW);
      ShowWindow(GetDlgItem(hwnd, eID_TEXTBOX_B_LED), SW_SHOW);

      ShowWindow(GetDlgItem(hwnd, eID_TEXTBOX_B), SW_SHOW);
      ShowWindow(GetDlgItem(hwnd, eID_TEXTBOX_G), SW_SHOW);
      ShowWindow(GetDlgItem(hwnd, eID_TEXTBOX_R), SW_SHOW);   
      RedrawWindow(hwnd, NULL, RDW_ALLCHILDREN|RDW_INVALIDATE|RDW_ERASE); 

      RGBLED_Dialog.m_SetColorValue(RGBLED_Dialog.RGB_Component.R_col,
                                    RGBLED_Dialog.RGB_Component.G_col,
                                    RGBLED_Dialog.RGB_Component.B_col);
      break;
    }
    case WM_DESTROY:
    {    
      
      RGBLED_Dialog.m_SetColorValue(0, 0, 0);
      DeleteDC(hdc_mem);
      return PostQuitMessage(hwnd);	
    }     
    default:
      return	DefWindowProc(hwnd, msg, wParam, lParam); 
  }
  
  return WM_NULL;
}

void	GUI_LED_DIALOG(void *param)
{
	WNDCLASS	wcex;
	MSG msg;


	wcex.Tag = WNDCLASS_TAG;

	wcex.Style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = GUI_LED_PROC; //������������Ϣ����Ļص�����.
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = NULL;//hInst;
	wcex.hIcon = NULL;//LoadIcon(hInstance, (LPCTSTR)IDI_WIN32_APP_TEST);
	wcex.hCursor = NULL;//LoadCursor(NULL, IDC_ARROW);

  
  RGBLED_Dialog_Init();
	//����������
	RGBLED_Dialog.hwnd = CreateWindowEx(WS_EX_NOFOCUS|WS_EX_FRAMEBUFFER,
                        &wcex,
                        L"GUI_LED_DIALOG",
                        WS_VISIBLE,
                        0, 0, GUI_XSIZE, GUI_YSIZE,
                        NULL, NULL, NULL, NULL);
   //��ʾ������
	ShowWindow(RGBLED_Dialog.hwnd, SW_SHOW);
	//��ʼ������Ϣѭ��(���ڹرղ�����ʱ,GetMessage������FALSE,�˳�����Ϣѭ��)��
	while (GetMessage(&msg, RGBLED_Dialog.hwnd))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}  
}

void GUI_LED_DIALOG_Create(void *param)
{
  ////Ӳ����ʼ��
  GUI_LED_DIALOG(NULL);//GUI���ڳ�ʼ��
  
}

void GUI_LED_DIALOG_Delete(void)
{
  
  RGBLED_Dialog.m_hardware_deinit();
  memset(&RGBLED_Dialog, 0, sizeof(RGBLED_Dialog));
  
}

#if 1
void GUI_RGBLED_DIALOGTest(void *param)
{
  static int thread = 0;
  int app = 0;
  
  if(thread == 0)
  {
#if MEM_DETECT      
      RGBLED_Dialog.cur_size = Get_VMEM_CurSize();
#endif
    
     GUI_Thread_Create(GUI_RGBLED_DIALOGTest,"GUI_PicViewer",8*1024,NULL,5,5);
     thread = 1;
     return;
  }
  if(thread == 1)
  {
		if(app==0)
		{
			app=1;
     
			GUI_LED_DIALOG_Create(NULL);
#if MEM_DETECT      
      if(Get_VMEM_CurSize() != RGBLED_Dialog.cur_size)
      {
        GUI_DEBUG("�ڴ�δ�ͷ�");
      }
#endif      
			app=0;
			thread=0;
      GUI_LED_DIALOG_Delete();
      GUI_Thread_Delete(GUI_GetCurThreadHandle());
		}    
  }
}
#endif


