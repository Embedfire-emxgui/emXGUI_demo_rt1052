#include "emXGUI.h"

#define BackGroundCol   59,64,71

//定义控件ID
enum	eID{
	eID_EXIT =0x1000,

  eID_BUTTON1,
  eID_BUTTON2,
  eID_BUTTON3,
  eID_BUTTON4,
  eID_BUTTON5,
  
  eID_LISTBOX1,
  
  eID_SCROLLBAR1,
  eID_PROGBAR3,
  
  eID_RB1,
  eID_RB2,
  eID_RB3,
  eID_RB4,
  
  eID_TB1,
  eID_TB2,
  eID_TB3,
  eID_TB4,
  eID_TB5,
  eID_TB6,
  eID_TB7,
  eID_TB8,
  eID_TB9,
  eID_TB10,
  eID_TB11, 
  
  eID_CB1,
  eID_CB2,
  eID_CB3,
  eID_CB4
};

static LRESULT GUI_ShowComponent_Proc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
  switch(msg)
  {
    static u8 pb1_val=0;
    case WM_CREATE:
    {
      RECT rc_in;
      RECT rc[3];
      HWND wnd;
      
      GetClientRect(hwnd, &rc_in);
      InflateRect(&rc_in, -50, -50);
      MakeMatrixRect(rc, &rc_in, 20, 0, 3, 1);
      rc[0].w = 120;
      rc[0].h = 40;
      CreateWindow(BUTTON, L"Normal", WS_VISIBLE,
                   rc[0].x, rc[0].y, rc[0].w, rc[0].h, hwnd, eID_BUTTON1, NULL, NULL);       
      OffsetRect(&rc[0], 0, rc[0].h+10);
      CreateWindow(BUTTON, L"Flat", BS_FLAT|WS_VISIBLE,
                   rc[0].x, rc[0].y, rc[0].w, rc[0].h, hwnd, eID_BUTTON2, NULL, NULL);    
      OffsetRect(&rc[0], 0, rc[0].h+10);
      CreateWindow(BUTTON, L"3D", BS_3D|WS_VISIBLE,
                   rc[0].x, rc[0].y, rc[0].w, rc[0].h, hwnd, eID_BUTTON2, NULL, NULL);  
      OffsetRect(&rc[0], 0, rc[0].h+10);
      CreateWindow(BUTTON, L"ROUND", BS_ROUND|WS_VISIBLE,
                   rc[0].x, rc[0].y, rc[0].w, rc[0].h, hwnd, eID_BUTTON2, NULL, NULL);     

      OffsetRect(&rc[0], 0, rc[0].h+20); 

      wnd=CreateWindow(LISTBOX,L"Listbox2",LBS_LINE|WS_BORDER|WS_VISIBLE,rc[0].x,rc[0].y,150,120,hwnd,eID_LISTBOX1,NULL,NULL);
      SendMessage(wnd,LB_ADDSTRING,0,(LPARAM)L"Item-2-0");
      SendMessage(wnd,LB_ADDSTRING,1,(LPARAM)L"Item-2-1");
      SendMessage(wnd,LB_ADDSTRING,2,(LPARAM)L"Item-2-2");

      SendMessage(wnd,LB_SETITEMHEIGHT,0,40);
      SendMessage(wnd,LB_SETITEMHEIGHT,1,40);
      SendMessage(wnd,LB_SETITEMHEIGHT,2,40);
      
      OffsetRect(&rc[1], -20, 0);
      rc[1].w = 240;
      rc[1].h = 50;

      SCROLLINFO sif;
      sif.cbSize		=sizeof(sif);
      sif.fMask		=SIF_ALL;
      sif.nMin		=0;
      sif.nMax		=+100;
      sif.nValue		=0;
      sif.TrackSize		=30;
      sif.ArrowSize		=20;//20;
      GUI_DEBUG("%d, %d", rc[1].x, rc[1].y);
      CreateWindow(SCROLLBAR,L"VScroll",WS_VISIBLE,rc[1].x,rc[1].y,rc[1].w,rc[1].h ,hwnd,eID_SCROLLBAR1,NULL,NULL);
      SendMessage(GetDlgItem(hwnd, eID_SCROLLBAR1),SBM_SETSCROLLINFO,TRUE,(LPARAM)&sif);      
      
      PROGRESSBAR_CFG cfg;	
      cfg.cbSize	 = sizeof(PROGRESSBAR_CFG);
           //开启所有的功能
      cfg.fMask    = PB_CFG_ALL;
           //文字格式水平，垂直居中
      cfg.TextFlag = DT_VCENTER|DT_CENTER;  
      OffsetRect(&rc[1], 0, rc[1].h+20);
      
      rc[1].h = 48;
      CreateWindow(PROGRESSBAR,L" ",
                              PBS_TEXT|PBS_ALIGN_LEFT|WS_VISIBLE,
                              rc[1].x,rc[1].y,280,rc[1].h,hwnd,eID_PROGBAR3,NULL,NULL);
      SendMessage(GetDlgItem(hwnd, eID_PROGBAR3),PBM_GET_CFG,TRUE,(LPARAM)&cfg);
      SendMessage(GetDlgItem(hwnd, eID_PROGBAR3),PBM_SET_CFG,TRUE,(LPARAM)&cfg);
      SendMessage(GetDlgItem(hwnd, eID_PROGBAR3), PBM_SET_RANGLE, TRUE, 50);   
     
      SetTimer(hwnd,1,200,TMR_START,NULL);
      
      /****************************TextBox*********************************/
      
      OffsetRect(&rc[1], 0, rc[1].h+20);
      rc[1].w = 150;
      CreateWindow(BUTTON,L"Radiobox1",BS_RADIOBOX|WS_VISIBLE,
                       rc[1].x,rc[1].y,rc[1].w,rc[1].h,hwnd,(1<<16)|eID_RB1,NULL,NULL);
      OffsetRect(&rc[1],0,rc[1].h+10);
      CreateWindow(BUTTON,L"Radiobox2",BS_RADIOBOX|WS_VISIBLE,rc[1].x,rc[1].y,rc[1].w,rc[1].h,hwnd,(1<<16)|eID_RB2,NULL,NULL);
      OffsetRect(&rc[1],0,rc[1].h+10);
      CreateWindow(BUTTON,L"Radiobox3",BS_RADIOBOX|WS_VISIBLE,
                       rc[1].x,rc[1].y,rc[1].w,rc[1].h,hwnd,(1<<16)|eID_RB3,NULL,NULL);
      OffsetRect(&rc[1],0,rc[1].h+10);
      CreateWindow(BUTTON,L"Radiobox4",BS_RADIOBOX|WS_VISIBLE,
                       rc[1].x,rc[1].y,rc[1].w,rc[1].h,hwnd,(1<<16)|eID_RB4,NULL,NULL);
                       
      OffsetRect(&rc[2], +60, 0);
      rc[2].w = 150;
      rc[2].h = 40;
      wnd = CreateWindow(TEXTBOX,L"Table",WS_VISIBLE,
                        rc[2].x,rc[2].y,rc[2].w,rc[2].h,hwnd,eID_TB1,NULL,NULL);
      SendMessage(wnd,TBM_SET_TEXTFLAG,0,DT_SINGLELINE|DT_LEFT|DT_VCENTER|DT_BKGND|DT_BORDER);
      
      OffsetRect(&rc[2], 0, rc[2].h);
      RECT table_rc[3];
      MakeMatrixRect(table_rc, &rc[2], 0, 0, 3, 1);
      InflateRectEx(&table_rc[0], 0,0,-6,0);
      wnd = CreateWindow(TEXTBOX,L"1",WS_VISIBLE,
                          table_rc[0].x,table_rc[0].y,table_rc[0].w,table_rc[0].h,hwnd,eID_TB2,NULL,NULL);
      SendMessage(wnd,TBM_SET_TEXTFLAG,0,DT_SINGLELINE|DT_LEFT|DT_VCENTER|DT_BKGND|DT_BORDER);  
      InflateRectEx(&table_rc[1], 6,0,-3,0);
      wnd = CreateWindow(TEXTBOX,L"13",WS_VISIBLE,
                   table_rc[1].x,table_rc[1].y,table_rc[1].w,table_rc[1].h,hwnd,eID_TB3,NULL,NULL);
      SendMessage(wnd,TBM_SET_TEXTFLAG,0,DT_SINGLELINE|DT_RIGHT|DT_VCENTER|DT_BKGND|DT_BORDER);  
      
      InflateRectEx(&table_rc[2], 3,0,0,0);
      wnd = CreateWindow(TEXTBOX,L"ms",WS_VISIBLE,
                   table_rc[2].x,table_rc[2].y,table_rc[2].w,table_rc[2].h,hwnd,eID_TB4,NULL,NULL);
      SendMessage(wnd,TBM_SET_TEXTFLAG,0,DT_SINGLELINE|DT_LEFT|DT_VCENTER|DT_BKGND|DT_BORDER);      
      
      OffsetRect(&table_rc[0], 0, table_rc[0].h);
      wnd = CreateWindow(TEXTBOX,L"2",WS_VISIBLE,
                          table_rc[0].x,table_rc[0].y,table_rc[0].w,table_rc[0].h,hwnd,eID_TB5,NULL,NULL);
      SendMessage(wnd,TBM_SET_TEXTFLAG,0,DT_SINGLELINE|DT_LEFT|DT_VCENTER|DT_BKGND|DT_BORDER);        

      OffsetRect(&table_rc[1], 0, table_rc[1].h);
      wnd = CreateWindow(TEXTBOX,L"46",WS_VISIBLE,
                          table_rc[1].x,table_rc[1].y,table_rc[1].w,table_rc[1].h,hwnd,eID_TB6,NULL,NULL);
      SendMessage(wnd,TBM_SET_TEXTFLAG,0,DT_SINGLELINE|DT_RIGHT|DT_VCENTER|DT_BKGND|DT_BORDER); 
      
      OffsetRect(&table_rc[2], 0, table_rc[2].h);
      wnd = CreateWindow(TEXTBOX,L"ms",WS_VISIBLE,
                   table_rc[2].x,table_rc[2].y,table_rc[2].w,table_rc[2].h,hwnd,eID_TB7,NULL,NULL);
      SendMessage(wnd,TBM_SET_TEXTFLAG,0,DT_SINGLELINE|DT_LEFT|DT_VCENTER|DT_BKGND|DT_BORDER);       


      OffsetRect(&table_rc[0], 0, table_rc[0].h);
      wnd = CreateWindow(TEXTBOX,L"3",WS_VISIBLE,
                          table_rc[0].x,table_rc[0].y,table_rc[0].w,table_rc[0].h,hwnd,eID_TB8,NULL,NULL);
      SendMessage(wnd,TBM_SET_TEXTFLAG,0,DT_SINGLELINE|DT_LEFT|DT_VCENTER|DT_BKGND|DT_BORDER);        

      OffsetRect(&table_rc[1], 0, table_rc[1].h);
      wnd = CreateWindow(TEXTBOX,L"61",WS_VISIBLE,
                          table_rc[1].x,table_rc[1].y,table_rc[1].w,table_rc[1].h,hwnd,eID_TB9,NULL,NULL);
      SendMessage(wnd,TBM_SET_TEXTFLAG,0,DT_SINGLELINE|DT_RIGHT|DT_VCENTER|DT_BKGND|DT_BORDER); 
      
      OffsetRect(&table_rc[2], 0, table_rc[2].h);
      wnd = CreateWindow(TEXTBOX,L"ms",WS_VISIBLE,
                   table_rc[2].x,table_rc[2].y,table_rc[2].w,table_rc[2].h,hwnd,eID_TB10,NULL,NULL);
      SendMessage(wnd,TBM_SET_TEXTFLAG,0,DT_SINGLELINE|DT_LEFT|DT_VCENTER|DT_BKGND|DT_BORDER);  

      OffsetRect(&rc[2], -20, rc[2].h*3+20);
      rc[2].w = 200;
      CreateWindow(BUTTON,L"Checkbox1",BS_CHECKBOX|WS_VISIBLE,
                       rc[2].x,rc[2].y,rc[2].w,rc[2].h,hwnd,eID_CB1,NULL,NULL);
          
      OffsetRect(&rc[2],0,rc[2].h+10);
          //创建复选框Checkbox2（BS_CHECKBOX）
      CreateWindow(BUTTON,L"Checkbox2",BS_CHECKBOX|WS_VISIBLE,
                        rc[2].x,rc[2].y,rc[2].w,rc[2].h,hwnd,eID_CB2,NULL,NULL);
      OffsetRect(&rc[2],0,rc[2].h+10);
      CreateWindow(BUTTON,L"Checkbox3",BS_PUSHLIKE|BS_CHECKBOX|WS_VISIBLE,
                  rc[2].x,rc[2].y,rc[2].w,rc[2].h,hwnd,eID_CB3,NULL,NULL);
      OffsetRect(&rc[2],0,rc[2].h+10);
      CreateWindow(BUTTON,L"Checkbox4",BS_PUSHLIKE|BS_CHECKBOX|WS_VISIBLE,
                  rc[2].x,rc[2].y,rc[2].w,rc[2].h,hwnd,eID_CB4,NULL,NULL);
      break;
    }
    case	WM_TIMER:
		{
			pb1_val +=1;
			if(pb1_val > 100)
			{
				pb1_val =0;
			}
      //设置进度值
			SendMessage(GetDlgItem(hwnd, eID_PROGBAR3),PBM_SET_VALUE,TRUE,pb1_val);
      return TRUE;
		}    
    case WM_ERASEBKGND:
    {
      HDC hdc =(HDC)wParam;
      RECT rc =*(RECT*)lParam;

      SetBrushColor(hdc, MapRGB(hdc, BackGroundCol));
      FillRect(hdc, &rc); 
      
      
      return TRUE;
    }
		case	WM_NOTIFY: //通知消息
		{
			NMHDR *nr;
			nr =(NMHDR*)lParam; //lParam参数，是以NMHDR结构体开头.
			u16 ctr_id;

			ctr_id =LOWORD(wParam); //wParam低16位是发送该消息的控件ID.
			if(ctr_id == eID_SCROLLBAR1)
			{
				NM_SCROLLBAR *sb_nr;
				int i;

				sb_nr =(NM_SCROLLBAR*)nr; //Scrollbar的通知消息实际为 NM_SCROLLBAR扩展结构,里面附带了更多的信息.
				switch(nr->code)
				{
					case SBN_THUMBTRACK: //滑块移动
					{

						i =sb_nr->nTrackValue; //获得滑块当前位置值
						SendMessage(nr->hwndFrom,SBM_SETVALUE,TRUE,i); //设置位置值
						
					}
					break;

				}
			}
		}
		break;    
    
    default:
      return DefWindowProc(hwnd,msg,wParam,lParam);
  }
  return WM_NULL;
}


void GUI_DEMO_ShowComponent(void *param)
{
	HWND hwnd;
	MSG	msg;
	WNDCLASS wcex;

	wcex.Tag	 		= WNDCLASS_TAG;

	wcex.Style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)GUI_ShowComponent_Proc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= NULL;
	wcex.hIcon			= NULL;
	wcex.hCursor		= NULL;



	hwnd	=CreateWindowEx(WS_EX_NODRAG|WS_EX_FRAMEBUFFER,&wcex,
							L"Show Wave",
							WS_CLIPCHILDREN,
							0,0,GUI_XSIZE,GUI_YSIZE,
							NULL,0,NULL,NULL);


	ShowWindow(hwnd,SW_SHOW);

	while(GetMessage(&msg,hwnd))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}


}

