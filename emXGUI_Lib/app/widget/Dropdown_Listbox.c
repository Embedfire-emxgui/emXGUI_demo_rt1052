#include "emXGUI.h"
#include "Dropdown_Listbox.h" 
typedef enum 
{
  eID_BUTTON1 = 0x1000,
  eID_LISTBOX1,
  eID_SCROLLBAR2
}eID;

//static void Darw_Arrow()
//{
//  POINT pt[3]={10, 20, 50, 60, 30, 40};  

//}

static void SCROLLBAR2_OwnerDraw(DRAWITEM_HDR *ds)
{
  HWND hwnd;
  HDC hdc;
  RECT rc;
  WCHAR wbuf[128];
  SCROLLBAR_RECT rcSL;
  hwnd = ds->hwnd; //button的窗口句柄.
  hdc = ds->hDC;   //button的绘图上下文句柄.
  rc = ds->rc;     //button的绘制矩形区.  
  SetBrushColor(hdc, MapRGB(hdc, 232,235,243));
  FillRoundRect(hdc, &rc, MIN(rc.w, rc.h)/2); //用矩形填充背景
  
  SendMessage(hwnd,SBM_GETRECT,0,(LPARAM)&rcSL);
  
  
  SetBrushColor(hdc, MapRGB(hdc, 119,136,153));
  FillRoundRect(hdc, &rcSL.V.Track, MIN(rcSL.V.Track.w, rcSL.V.Track.h)/2); //用矩形填充背景
  
  
}
static void BUTTON1_OwnerDraw(DRAWITEM_HDR *ds) //绘制一个按钮外观
{
  HWND hwnd;
  HDC hdc;
  RECT rc;
  WCHAR wbuf[128];

  hwnd = ds->hwnd; //button的窗口句柄.
  hdc = ds->hDC;   //button的绘图上下文句柄.
  rc = ds->rc;     //button的绘制矩形区.
  
  if(ds->State & BST_PUSHED)  
    GradientFillRect(hdc, &rc, MapRGB(hdc, 75, 82, 89), MapRGB(hdc, 43, 46, 47), TRUE); 
//  SetBrushColor(hdc, MapRGB(hdc, 105,105,105));
  else
    GradientFillRect(hdc, &rc, MapRGB(hdc, 75, 81, 88), MapRGB(hdc, 66, 71, 78), TRUE); 
  
//  FillRoundRect(hdc, &rc, 5);
  SetTextColor(hdc, MapRGB(hdc, 255,255,255));
  GetWindowText(ds->hwnd, wbuf, 128); //获得按钮控件的文字
//  OffsetRect(&rc, -10, 0);
  InflateRectEx(&rc, 0, 0, -rc.w*1/4, 0);
  DrawText(hdc, wbuf, -1, &rc, DT_VCENTER);//绘制文字(居中对齐方式) 
  
  //箭头
  rc =ds->rc;
  InflateRectEx(&rc, -rc.w*3/4, 0, 0, 0);

  SetPenColor(hdc, MapRGB(hdc, 255,255,255));
//  DrawText(hdc, L"\\/", -1, &rc, DT_VCENTER | DT_CENTER);
//  DrawRect(hdc, &rc);
//  GUI_DEBUG("%d", rc.w);
//  VLine(hdc, rc.x+rc.w/3-8,   rc.y, rc.y+rc.h);
//  VLine(hdc, rc.x+rc.w*2/3+8, rc.y, rc.y+rc.h);
//  VLine(hdc, rc.x+rc.w/2, rc.y, rc.y+rc.h);
//  
//  HLine(hdc, rc.x, rc.y+10, rc.x+rc.w);
//  HLine(hdc, rc.x, rc.y+20, rc.x+rc.w);
//  HLine(hdc, rc.x, rc.y+30, rc.x+rc.w);
  
  EnableAntiAlias(hdc, TRUE); 
  Line(hdc, rc.x+rc.w/3-8, rc.y+10, rc.x+rc.w/2, rc.y+30);
  Line(hdc, rc.x+rc.w*2/3+8, rc.y+10, rc.x+rc.w/2, rc.y+30);
  EnableAntiAlias(hdc, FALSE);
}

static LRESULT Dropdown_Listbox_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  static bool state = FALSE;//默认不展开 
  static RECT rc;
  static WCHAR** p_str = NULL;
  switch(msg)
  {
    case WM_CREATE:
    {
      
      GetClientRect(hwnd, &rc);
      list_cfg_t *cfg = (list_cfg_t*)lParam;
      p_str = cfg->list_objs;
      
      CreateWindow(BUTTON, *p_str, BS_CHECKBOX|WS_OWNERDRAW|WS_VISIBLE, rc.x, rc.y, rc.w, rc.h*1/3, hwnd, eID_BUTTON1, NULL, NULL);
           
      //OffsetRect(&rc, -10, rc.h*1/3);
      InflateRectEx(&rc, 0, -rc.h*1/3, -10, rc.h*1/3);
      CreateWindow(LISTBOX,L"Listbox2",LBS_LINE|LBS_NOTIFY|WS_BORDER,rc.x,rc.y,rc.w, rc.h*2/3,hwnd,eID_LISTBOX1,NULL,NULL);
      for(int i = 1; i < cfg->y_num; i++)
        SendMessage(GetDlgItem(hwnd, eID_LISTBOX1),LB_ADDSTRING,-1,(LPARAM)*(p_str+i));
    
      OffsetRect(&rc, rc.w, 0);
      SCROLLINFO sif;
      int High = SendMessage(GetDlgItem(hwnd, eID_LISTBOX1),LB_GETITEMHEIGHT,1,0);
      int count = SendMessage(GetDlgItem(hwnd, eID_LISTBOX1),LB_GETCOUNT,1,0);      
      sif.cbSize		=sizeof(sif);
      sif.fMask		=SIF_ALL;
      sif.nMin		=0;
      sif.nMax		=+High*count;
      sif.nValue		=0;
      sif.TrackSize		=30;
      sif.ArrowSize		=0;//20;      
      CreateWindow(SCROLLBAR, L"ROUND", SBS_VERT|WS_OWNERDRAW|SBS_NOARROWS|WS_TRANSPARENT,
                   rc.x, rc.y, 10, rc.h*2/3, hwnd, eID_SCROLLBAR2, NULL, NULL);           
      SendMessage(GetDlgItem(hwnd, eID_SCROLLBAR2),SBM_SETSCROLLINFO,TRUE,(LPARAM)&sif);       
      break;
    } 
    case WM_DRAWITEM:
    {
      DRAWITEM_HDR *ds = (DRAWITEM_HDR*)lParam;      
      if(ds->ID == eID_BUTTON1)
      {
        BUTTON1_OwnerDraw(ds);
        return TRUE;
      }   
      if(ds->ID == eID_SCROLLBAR2)
      {
        SCROLLBAR2_OwnerDraw(ds);
        return TRUE;
      }
    }
    case WM_ERASEBKGND:
    {
      HDC hdc =(HDC)wParam;
      RECT rc =*(RECT*)lParam;
      SetBrushColor(hdc, MapRGB(hdc, 59,64,71));
      FillRect(hdc, &rc); 
      
      return TRUE;
    }
    
    case WM_NOTIFY:
    {
      u16 code, id;
      NMHDR *nr=(NMHDR*)lParam;
      id = LOWORD(wParam);
      code = HIWORD(wParam);  
           
      if(id == eID_BUTTON1 && code == BN_CLICKED)
      {
        
        state = !state;
        if(state == TRUE)
        {
          ShowWindow(GetDlgItem(hwnd, eID_SCROLLBAR2), SW_SHOW); 
          ShowWindow(GetDlgItem(hwnd, eID_LISTBOX1), SW_SHOW); 
          SendMessage(GetDlgItem(hwnd, eID_LISTBOX1),LB_SETPOS,FALSE,0);
          SendMessage(GetDlgItem(hwnd, eID_LISTBOX1),LB_SETCURSEL,0,0);
          SetWindowText(GetDlgItem(hwnd, eID_BUTTON1), *(p_str)); 
        }
        else
        {
          ShowWindow(GetDlgItem(hwnd, eID_SCROLLBAR2), SW_HIDE); 
          ShowWindow(GetDlgItem(hwnd, eID_LISTBOX1), SW_HIDE);        
        }
        
      }
  		if(nr->idFrom == eID_LISTBOX1)
			{
				if(nr->code==LBN_POSCHANGE)
				{
          RECT rc;
          int i = 0;
          int count = 0;
					i =SendMessage(nr->hwndFrom,LB_GETPOS,0,0);
					
          count = SendMessage(nr->hwndFrom,LB_GETCOUNT,0,0);
          SendMessage(nr->hwndFrom,LB_GETITEMRECT, (count-1),(LPARAM)&rc);
          
          GUI_DEBUG("%d", rc.y);
          if(rc.y < 0)
          {
            int max = 0;
            SendMessage(GetDlgItem(hwnd, eID_SCROLLBAR2),SBM_GETRANGE,NULL,(LPARAM)&max);
            GUI_DEBUG("%d", max);
            SendMessage(GetDlgItem(hwnd, eID_SCROLLBAR2),SBM_SETVALUE,TRUE,max);
            
          }
          else
            SendMessage(GetDlgItem(hwnd, eID_SCROLLBAR2),SBM_SETVALUE,TRUE,i);
          
          
				}
        if(nr->code == LBN_SELCHANGE)
        {
          int i;

					i =SendMessage(nr->hwndFrom,LB_GETCURSEL,0,0);          
          
          SetWindowText(GetDlgItem(hwnd, eID_BUTTON1), *(p_str+i+1)); 
        
        }
			}      
      break;
    }
    case WM_PAINT:
    {
      RECT rc;
			PAINTSTRUCT ps;
			HDC hdc;      
      GetClientRect(hwnd, &rc);
      OffsetRect(&rc,0,rc.h);
      hdc =BeginPaint(hwnd,&ps);
//      DrawText(hdc, L"\\/", -1, &rc, DT_VCENTER | DT_CENTER);
      EndPaint(hwnd,&ps);
      break;
    }
  }
  return WM_NULL;
}
//定义一个控件类结构实例.
const WNDCLASS Dropdown_Listbox =
{
	.Tag = WNDCLASS_TAG,
	.Style = 0,
	.lpfnWndProc = Dropdown_Listbox_proc,
	.cbClsExtra = 0,
	.cbWndExtra = 0,
	.hInstance = NULL,
	.hIcon = NULL,
	.hCursor = NULL,
};
