/**
  ******************************************************************
  * @file    GUI_RGBLED_DIALOG.c
  * @author  fire
  * @version V1.0
  * @date    2019-xx-xx
  * @brief   RGB灯测试窗口的GUI代码
  ******************************************************************
  * @attention
  *
  * 实验平台:野火  i.MXRT1052开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
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
  /***********************硬件相关部分***************************/
  RGBLED_Dialog.m_SetColorValue = SetColorValue;
  RGBLED_Dialog.m_hardware_init = FlexPWM_Init;
  RGBLED_Dialog.m_hardware_deinit = FlexPWM_DeInit;
  
  FlexPWM_Init();
  
  RGBLED_Dialog.m_SetColorValue(RGBLED_Dialog.RGB_Component.R_col,
                                RGBLED_Dialog.RGB_Component.G_col,
                                RGBLED_Dialog.RGB_Component.B_col);
}

/**
  * @brief  设置背景HDC
  * @param  hdc_background:背景层HDC
  * @retval NULL
  */
void Set_BackGroudHDC(HDC hdc_background)
{
  RGBLED_Dialog.hdc_back = hdc_background;
}
/**
  * @brief  获取当前APP的背景层HDC
  * @param  NULL
  * @retval 当前背景层HDC
  */
HDC Get_BackGroudHDC(void)
{
  return RGBLED_Dialog.hdc_back;
}

/**
  * @brief  APP背景加载函数
  * @param  hdc_mem:绘制上下文，用于背景层
  * @param  rc:窗口大小
  * @retval NULL
  */
void Load_BackGround(HDC hdc_mem, RECT rc)
{
  BOOL res;
  u8 *jpeg_buf;
  u32 jpeg_size;
  JPG_DEC *dec;  
  //加载背景图片
  res = RES_Load_Content(GUI_RGB_BACKGROUNG_PIC, (char**)&jpeg_buf, &jpeg_size);      
  //若读取文件失败，则绘制黑色桌面
  if(res)
  {
    /* 根据图片数据创建JPG_DEC句柄 */
    dec = JPG_Open(jpeg_buf, jpeg_size);

    /* 绘制至内存对象 */
    JPG_Draw(hdc_mem, 0, 0, dec);

    /* 关闭JPG_DEC句柄 */
    JPG_Close(dec);
  }
  else
  {
    
    SetBrushColor(hdc_mem, MapRGB(hdc_mem, 250, 250, 250));
    FillRect(hdc_mem, &rc);
    SetTextColor(hdc_mem, MapRGB(hdc_mem, 0, 0, 0));
    DrawText(hdc_mem, L"找不到背景图片", -1, &rc, DT_TOP | DT_LEFT);
    
  }
  /* 释放图片内容空间 */
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
      //获取窗口大小
      GetClientRect(hwnd, &rc);
      CreateWindow(BUTTON, L"O",WS_TRANSPARENT|WS_OWNERDRAW|WS_VISIBLE,
                   (EXIT_POS_X - EXIT_POS_W), EXIT_POS_Y, EXIT_POS_W, EXIT_POS_H, 
                   hwnd, eID_EXIT, NULL, NULL);     

      CreateWindow(BUTTON,L"开/关灯",WS_TRANSPARENT|WS_OWNERDRAW|WS_VISIBLE,
                   SW_POS_X, SW_POS_Y, SW_POS_W, SW_POS_H,
                   hwnd,eID_CHECKBOX_SW,NULL,NULL);
      //红灯
      CreateWindow(TEXTBOX, L"k", WS_TRANSPARENT|WS_VISIBLE, 
                   RLED_POS_X, RLED_POS_Y, RLED_POS_W, RLED_POS_H, 
                  hwnd, eID_TEXTBOX_R_LED, NULL, NULL);
      SetWindowFont(GetDlgItem(hwnd, eID_TEXTBOX_R_LED), ctrlFont72);
      SendMessage(GetDlgItem(hwnd, eID_TEXTBOX_R_LED),TBM_SET_TEXTFLAG,0,
                    DT_SINGLELINE|DT_CENTER|DT_VCENTER); 
      //绿灯            
      CreateWindow(TEXTBOX, L"k", WS_TRANSPARENT|WS_VISIBLE, 
                   GLED_POS_X, GLED_POS_Y, GLED_POS_W, GLED_POS_H,
                  hwnd, eID_TEXTBOX_G_LED, NULL, NULL);
      SetWindowFont(GetDlgItem(hwnd, eID_TEXTBOX_G_LED), ctrlFont72);
      SendMessage(GetDlgItem(hwnd, eID_TEXTBOX_G_LED),TBM_SET_TEXTFLAG,0,
                    DT_SINGLELINE|DT_CENTER|DT_VCENTER); 
      //蓝灯              
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

      /*创建滑动条--R*/
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
      /*创建滑动条--G*/
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
      /*创建滑动条--B*/
      CreateWindow(SCROLLBAR, L"SCROLLBAR_B", SBS_VERT|WS_OWNERDRAW | WS_TRANSPARENT|WS_VISIBLE, 
                    BSBAR_POS_X,BSBAR_POS_Y,BSBAR_POS_W,BSBAR_POS_H,
                    hwnd, eID_SCROLLBAR_B, NULL, NULL);
      SendMessage(GetDlgItem(hwnd, eID_SCROLLBAR_B), SBM_SETSCROLLINFO, TRUE, (LPARAM)&RGBLED_Dialog.sif_B);

      /*创建文本框--红灯*/
      CreateWindow(BUTTON, L"qp", WS_TRANSPARENT|WS_VISIBLE|WS_OWNERDRAW, 
                    RTEXT_POS_X,RTEXT_POS_Y,RTEXT_POS_W,RTEXT_POS_H,
                  hwnd, eID_TEXTBOX_R, NULL, NULL);
      SendMessage(GetDlgItem(hwnd, eID_TEXTBOX_R),TBM_SET_TEXTFLAG,0,
                    DT_SINGLELINE|DT_CENTER|DT_VCENTER|DT_BKGND); 
      SetWindowFont(GetDlgItem(hwnd, eID_TEXTBOX_R), ctrlFont32);                        
                        
      //创建文本框--绿灯         
      CreateWindow(BUTTON, L"rp", WS_TRANSPARENT|WS_OWNERDRAW|WS_VISIBLE, 
                    GTEXT_POS_X, GTEXT_POS_Y, GTEXT_POS_W, GTEXT_POS_H,
                  hwnd, eID_TEXTBOX_G, NULL, NULL);
      SendMessage(GetDlgItem(hwnd, eID_TEXTBOX_G),TBM_SET_TEXTFLAG,0,
                    DT_SINGLELINE|DT_CENTER|DT_VCENTER|DT_BKGND);   
      SetWindowFont(GetDlgItem(hwnd, eID_TEXTBOX_G), ctrlFont32);  
      //创建文本框--蓝灯
      CreateWindow(BUTTON, L"sp", WS_TRANSPARENT|WS_OWNERDRAW|WS_VISIBLE, 
                   BTEXT_POS_X, BTEXT_POS_Y, BTEXT_POS_W, BTEXT_POS_H,                    
                  hwnd, eID_TEXTBOX_B, NULL, NULL);  
      SetWindowFont(GetDlgItem(hwnd, eID_TEXTBOX_B), ctrlFont32);
      //R的分量值 
      x_wsprintf(wbuf, L"%d", RGBLED_Dialog.RGB_Component.R_col);
      CreateWindow(BUTTON, wbuf, WS_TRANSPARENT|WS_VISIBLE|WS_OWNERDRAW|WS_DISABLED, 
                   RNUMS_POS_X,RNUMS_POS_Y,RNUMS_POS_W,RNUMS_POS_H,
                  hwnd, eID_TEXTBOX_R_NUM, NULL, NULL); 
      SetWindowFont(GetDlgItem(hwnd, eID_TEXTBOX_R_NUM), ctrlFont32);               
      //G的分量值 
      x_wsprintf(wbuf, L"%d", RGBLED_Dialog.RGB_Component.G_col);
      CreateWindow(BUTTON, wbuf, WS_TRANSPARENT|WS_VISIBLE|WS_OWNERDRAW|WS_DISABLED, 
                   GNUMS_POS_X,GNUMS_POS_Y,GNUMS_POS_W,GNUMS_POS_H,        
                  hwnd, eID_TEXTBOX_G_NUM, NULL, NULL);

      SetWindowFont(GetDlgItem(hwnd, eID_TEXTBOX_G_NUM), ctrlFont32);       
      //B的分量值         
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
      //加载背景
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
      id  =LOWORD(wParam);//获取消息的ID码
      code=HIWORD(wParam);//获取消息的类型
      ctr_id = LOWORD(wParam); //wParam低16位是发送该消息的控件ID.
      nr = (NMHDR*)lParam; //lParam参数，是以NMHDR结构体开头.
         
      if(id == eID_EXIT && code == BN_CLICKED)
      {
        PostCloseMessage(hwnd);
      }
      if(ctr_id == eID_SCROLLBAR_R)
      {
        NM_SCROLLBAR *sb_nr;		
        sb_nr = (NM_SCROLLBAR*)nr; //Scrollbar的通知消息实际为 NM_SCROLLBAR扩展结构,里面附带了更多的信息.
        switch (nr->code)
        {
           case SBN_THUMBTRACK: //R滑块移动
           {
              RGBLED_Dialog.RGB_Component.R_col = sb_nr->nTrackValue; //获得滑块当前位置值
              SendMessage(nr->hwndFrom, SBM_SETVALUE, TRUE, RGBLED_Dialog.RGB_Component.R_col); //设置位置值
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
        sb_nr = (NM_SCROLLBAR*)nr; //Scrollbar的通知消息实际为 NM_SCROLLBAR扩展结构,里面附带了更多的信息.
        switch (nr->code)
        {
           case SBN_THUMBTRACK: //G滑块移动
           {
              RGBLED_Dialog.RGB_Component.G_col = sb_nr->nTrackValue; //获得滑块当前位置值
              SendMessage(nr->hwndFrom, SBM_SETVALUE, TRUE, RGBLED_Dialog.RGB_Component.G_col); //设置位置值
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
        sb_nr = (NM_SCROLLBAR*)nr; //Scrollbar的通知消息实际为 NM_SCROLLBAR扩展结构,里面附带了更多的信息.
        switch (nr->code)
        {
           case SBN_THUMBTRACK: //B滑块移动
           {
              RGBLED_Dialog.RGB_Component.B_col = sb_nr->nTrackValue; //获得B滑块当前位置值
              SendMessage(nr->hwndFrom, SBM_SETVALUE, TRUE, RGBLED_Dialog.RGB_Component.B_col); //设置B滑块的位置

              //更新内容
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
      //BOOL cur_state = HIWORD(wParam);//当前状态
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
	wcex.lpfnWndProc = GUI_LED_PROC; //设置主窗口消息处理的回调函数.
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = NULL;//hInst;
	wcex.hIcon = NULL;//LoadIcon(hInstance, (LPCTSTR)IDI_WIN32_APP_TEST);
	wcex.hCursor = NULL;//LoadCursor(NULL, IDC_ARROW);

  
  RGBLED_Dialog_Init();
	//创建主窗口
	RGBLED_Dialog.hwnd = CreateWindowEx(WS_EX_NOFOCUS|WS_EX_FRAMEBUFFER,
                        &wcex,
                        L"GUI_LED_DIALOG",
                        WS_VISIBLE,
                        0, 0, GUI_XSIZE, GUI_YSIZE,
                        NULL, NULL, NULL, NULL);
   //显示主窗口
	ShowWindow(RGBLED_Dialog.hwnd, SW_SHOW);
	//开始窗口消息循环(窗口关闭并销毁时,GetMessage将返回FALSE,退出本消息循环)。
	while (GetMessage(&msg, RGBLED_Dialog.hwnd))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}  
}

void GUI_LED_DIALOG_Create(void *param)
{
  ////硬件初始化
  GUI_LED_DIALOG(NULL);//GUI窗口初始化
  
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
        GUI_DEBUG("内存未释放");
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


