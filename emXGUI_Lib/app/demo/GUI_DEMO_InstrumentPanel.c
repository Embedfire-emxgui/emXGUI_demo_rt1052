#include "emXGUI.h"
#include "emXGUI_JPEG.h"

#define	ID_OK		0x1000
#define	ID_NEXT		0x1001

#define PANEL_W 400
#define PANEL_H 400 
#define TURN_SPEED  3

typedef struct
{
  HWND  hwnd;
  HDC   hdc_mem;
  HDC   hdc_mem_panel;
  
  char* m_panel_picpath;
  int point_type;
  int panel_value;
  int panel_value_inc;
}InstrumentPanel_T;

InstrumentPanel_T g_panel=
{
  .hwnd = NULL,
  .panel_value = 0,
  .panel_value_inc = 3,
  .point_type = 0,
  .m_panel_picpath = "instrument_panel.jpg",
};
static void	X_MeterPointer(HDC hdc,int cx,int cy,int r,u32 color,int st_angle,int angle_size,int dat_size,int dat_val,int style)
{
	int angle;
	POINT pt[8];

	////

	if(angle_size>360)	angle_size=360;

	angle = st_angle+(dat_val*angle_size)/dat_size;
	angle = MIN(angle,st_angle+angle_size);

	angle -= 90;

	

  if(style==0)
  {
    pt[0].x	 = (int)(cx + sin(angle*3.14/180)*r);
    pt[0].y	 = (int)(cy - cos(angle*3.14/180)*r);

    pt[1].x = (int)(cx + sin((angle+90)*3.14/180)*(r>>5));
    pt[1].y = (int)(cy - cos((angle+90)*3.14/180)*(r>>5));

    pt[2].x = (int)(cx + sin((angle+180)*3.14/180)*(r>>5));
    pt[2].y = (int)(cy - cos((angle+180)*3.14/180)*(r>>5));

    pt[3].x = (int)(cx + sin((angle+270)*3.14/180)*(r>>5));
    pt[3].y = (int)(cy - cos((angle+270)*3.14/180)*(r>>5));

    pt[4].x = pt[0].x;
    pt[4].y = pt[0].y;


    SetBrushColor(hdc,color);
    EnableAntiAlias(hdc, TRUE);
    FillPolygon(hdc,0,0,pt,5);
    EnableAntiAlias(hdc, FALSE);
    //SetPenColor(hdc,color);
    //AA_DrawPolygon(hdc,0,0,pt,5); //?????????
  }


  if(style==1)
  {
    pt[0].x	 = (int)(cx + sin(angle*3.14/180)*r);
    pt[0].y	 = (int)(cy - cos(angle*3.14/180)*r);

    pt[1].x = (int)(cx + sin((angle+4)*3.14/180)*(r-(r>>2)));
    pt[1].y = (int)(cy - cos((angle+4)*3.14/180)*(r-(r>>2)));

    pt[2].x = cx;
    pt[2].y = cy;

    pt[3].x = (int)(cx + sin((angle-4)*3.14/180)*(r-(r>>2)));
    pt[3].y = (int)(cy - cos((angle-4)*3.14/180)*(r-(r>>2)));

    pt[4].x = pt[0].x;
    pt[4].y = pt[0].y;


    FillPolygon(hdc,0,0,pt,5); //?????????

  }


  if(style==2)
  {
    POINT pt[7];

    pt[0].x	 = (int)(cx + sin(angle*3.14/180)*r);
    pt[0].y	 = (int)(cy - cos(angle*3.14/180)*r);

    pt[1].x = (int)(cx + sin((angle+6)*3.14/180)*(r-(r>>2)));
    pt[1].y = (int)(cy - cos((angle+6)*3.14/180)*(r-(r>>2)));

    pt[2].x = (int)(cx + sin((angle+1)*3.14/180)*(r-(r>>2)+(r>>4)));
    pt[2].y = (int)(cy - cos((angle+1)*3.14/180)*(r-(r>>2)+(r>>4)));

    pt[3].x = (int)cx;
    pt[3].y = (int)cy;

    pt[4].x = (int)(cx + sin((angle-1)*3.14/180)*(r-(r>>2)+(r>>4)));
    pt[4].y = (int)(cy - cos((angle-1)*3.14/180)*(r-(r>>2)+(r>>4)));

    pt[5].x = (int)(cx + sin((angle-6)*3.14/180)*(r-(r>>2)));
    pt[5].y = (int)(cy - cos((angle-6)*3.14/180)*(r-(r>>2)));

    pt[6].x = pt[0].x;
    pt[6].y = pt[0].y;

    FillPolygon(hdc,0,0,pt,7);

  }

  if(style==3) 
  {
    POINT pt[7];
    int x1,y1;

    x1 = (int)(cx + sin((angle-0)*3.14/180)*(r-(r>>5)));
    y1 = (int)(cy - cos((angle-0)*3.14/180)*(r-(r>>5)));

    pt[0].x	 = (int)(x1 + sin((angle-90)*3.14/180)*(2));
    pt[0].y	 = (int)(y1 - cos((angle-90)*3.14/180)*(2));

    pt[1].x	 = x1;
    pt[1].y	 = y1;

    pt[2].x	 = (int)(x1 + sin((angle+90)*3.14/180)*(2));
    pt[2].y	 = (int)(y1 - cos((angle+90)*3.14/180)*(2));


    pt[3].x	 = (int)(cx + sin((angle+90)*3.14/180)*(2));
    pt[3].y	 = (int)(cy - cos((angle+90)*3.14/180)*(2));

    pt[4].x	 = cx;
    pt[4].y	 = cy;

    pt[5].x	 = (int)(cx + sin((angle-90)*3.14/180)*(2));
    pt[5].y	 = (int)(cy - cos((angle-90)*3.14/180)*(2));

    pt[6].x = pt[0].x;
    pt[6].y = pt[0].y;

    FillPolygon(hdc,0,0,pt,7);

  }

  if(style==4) 
  {
    POINT pt[7];
    int x1,y1;

    x1 = (int)(cx + sin((angle-0)*3.14/180)*(r-(r>>5)));
    y1 = (int)(cy - cos((angle-0)*3.14/180)*(r-(r>>5)));

    pt[0].x	 = (int)(cx + sin((angle-60)*3.14/180)*(5));
    pt[0].y	 = (int)(cy - cos((angle-60)*3.14/180)*(5));

    pt[1].x	 = cx;
    pt[1].y	 = cy;

    pt[2].x	 = (int)(cx + sin((angle+60)*3.14/180)*(5));
    pt[2].y	 = (int)(cy - cos((angle+60)*3.14/180)*(5));


    pt[3].x	 = (int)(pt[2].x + sin((angle+0)*3.14/180)*(r-(r>>2)));
    pt[3].y	 = (int)(pt[2].y - cos((angle+0)*3.14/180)*(r-(r>>2)));

    pt[4].x	 = x1;
    pt[4].y	 = y1;

    pt[5].x	 = (int)(pt[0].x + sin((angle+0)*3.14/180)*(r-(r>>2)));
    pt[5].y	 = (int)(pt[0].y - cos((angle+0)*3.14/180)*(r-(r>>2)));

    pt[6].x = pt[0].x;
    pt[6].y = pt[0].y;

    FillPolygon(hdc,0,0,pt,7);

  }

  if(style==5) 
  {
    POINT pt[5];
    int x1,y1;

    x1 = (int)(cx + sin((angle-0)*3.14/180)*(r-(r>>5)));
    y1 = (int)(cy - cos((angle-0)*3.14/180)*(r-(r>>5)));

    pt[0].x	 = x1;
    pt[0].y	 = y1;

    pt[1].x = (int)(cx + sin((angle+12)*3.14/180)*(r-(r>>1)));
    pt[1].y = (int)(cy - cos((angle+12)*3.14/180)*(r-(r>>1)));

    pt[2].x = (int)cx;
    pt[2].y = (int)cy;

    pt[3].x = (int)(cx + sin((angle-12)*3.14/180)*(r-(r>>1)));
    pt[3].y = (int)(cy - cos((angle-12)*3.14/180)*(r-(r>>1)));

    pt[4].x = pt[0].x;
    pt[4].y = pt[0].y;

    FillPolygon(hdc,0,0,pt,5);

  }

  if(style==6) 
  {
    POINT pt[5];
    int x1,y1;

    x1 = (int)(cx + sin((angle-0)*3.14/180)*((r*15)>>4));
    y1 = (int)(cy - cos((angle-0)*3.14/180)*((r*15)>>4));

    pt[0].x	 = x1;
    pt[0].y	 = y1;

    pt[1].x = (int)(cx + sin((angle+15)*3.14/180)*((r*4)>>4));
    pt[1].y = (int)(cy - cos((angle+15)*3.14/180)*((r*4)>>4));

    pt[2].x = (int)cx;
    pt[2].y = (int)cy;

    pt[3].x = (int)(cx + sin((angle-15)*3.14/180)*((r*4)>>4));
    pt[3].y = (int)(cy - cos((angle-15)*3.14/180)*((r*4)>>4));

    pt[4].x = pt[0].x;
    pt[4].y = pt[0].y;

    FillPolygon(hdc,0,0,pt,5);

  }

	////
#if 0	
	SetBrushColor(hdc,fr_color);
	FillCircle(hdc,cx,cy,r>>3);
#endif

}
static LRESULT InstrumentPanel_WinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  RECT rc;
  switch(msg)
  {
    case WM_CREATE:
    {
      
      GetClientRect(hwnd, &rc);
      g_panel.panel_value_inc = 3;
      g_panel.hdc_mem =CreateMemoryDC(SURF_SCREEN,PANEL_W,PANEL_H);
      
      g_panel.hdc_mem_panel = CreateMemoryDC(SURF_SCREEN, PANEL_W, PANEL_H);
      //ClrDisplay(hdc_mem_panel, NULL, MapARGB(hdc_mem_panel, 255, 0, 0, 0));
      CreateWindow(BUTTON, L"OK", WS_VISIBLE, rc.w - 80, 8, 68, 32, hwnd, ID_OK, NULL, NULL); //′′?¨ò???°′?￥(ê?ày).
      
//      CreateWindow(BUTTON, L"->", WS_VISIBLE, rc.w - 80, 8 + 40, 68, 32, hwnd, ID_NEXT, NULL, NULL);

      u8 *jpeg_buf;
      u32 jpeg_size;
      JPG_DEC *dec;
      BOOL res;
      res = RES_Load_Content("instrument_panel.jpg", (char **)&jpeg_buf, &jpeg_size);
      if (res)
      {
        /* 根据图片数据创建JPG_DEC句柄 */
        dec = JPG_Open(jpeg_buf, jpeg_size);
        /* 读取图片文件信息 */

        /* 绘制至内存对象 */
        JPG_Draw(g_panel.hdc_mem_panel, 0, 0, dec);

        /* 关闭JPG_DEC句柄 */
        JPG_Close(dec);
      }

      /* 释放图片内容空间 */
      RES_Release_Content((char **)&jpeg_buf);
      SetTimer(hwnd, 0x2000, 1, TMR_START, NULL);
      break;
    }
    case WM_TIMER:
    {
      u16 id;
      id = LOWORD(wParam);

      if (id == 0x2000)
      {

        if (g_panel.panel_value > 180)
        {
          g_panel.panel_value_inc = -1 * (TURN_SPEED);
        }

        if (g_panel.panel_value < 0)
        {
          g_panel.panel_value_inc = TURN_SPEED;
        }
        g_panel.panel_value += g_panel.panel_value_inc;

        InvalidateRect(hwnd, NULL, FALSE);
      }
      break; 
    }
		case WM_NOTIFY: 
    {
      u16 code, id;

      code = HIWORD(wParam); 
      id = LOWORD(wParam); 

      if (id == ID_OK && code == BN_CLICKED) 
      {
        PostCloseMessage(hwnd);
      }

//      if (id == ID_NEXT && code == BN_CLICKED)
//      {
//        g_panel.point_type++;
//        if (g_panel.point_type > 6)
//        {
//          g_panel.point_type = 0;
//        }
//        InvalidateRect(hwnd, NULL, FALSE);
//      }
      break;
	}   
		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc;
			int x;
      
			hdc =BeginPaint(hwnd,&ps); 


			x =200;
      
			BitBlt(g_panel.hdc_mem,0,0,PANEL_W,PANEL_H,g_panel.hdc_mem_panel,0,0,SRCCOPY); 
			
			
			X_MeterPointer(g_panel.hdc_mem,PANEL_W/2,PANEL_H/2,120,MapRGB(g_panel.hdc_mem,250,20,20),0,360,360,g_panel.panel_value,0);

			BitBlt(hdc,x,20,PANEL_W,PANEL_H,g_panel.hdc_mem,0,0,SRCCOPY); 


			EndPaint(hwnd,&ps); 
			
      break;
		}    
    case WM_CLOSE:
    {
      
      DeleteDC(g_panel.hdc_mem);
      DeleteDC(g_panel.hdc_mem_panel);

      return DestroyWindow(hwnd); 
    }
    default:
      return DefWindowProc(hwnd, msg, wParam, lParam);
  }
  
  
  return WM_NULL;
}



void	GUI_DEMO_InstrumentPanel(void *param)
{
	WNDCLASS	wcex;
	MSG msg;


	wcex.Tag = WNDCLASS_TAG;

	wcex.Style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = InstrumentPanel_WinProc; 
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = NULL;
	wcex.hIcon = NULL;
	wcex.hCursor = NULL;


	g_panel.hwnd = CreateWindowEx(NULL,
                        &wcex,
                        L"emXGUI DEMO:DashBoard", 
                        WS_BORDER | WS_CLIPCHILDREN,
                        0, 0, GUI_XSIZE, GUI_YSIZE,    
                        NULL, NULL, NULL, NULL);

	
	ShowWindow(g_panel.hwnd, SW_SHOW);

	
	while (GetMessage(&msg, g_panel.hwnd))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
  
  
  memset(&g_panel,0,sizeof(InstrumentPanel_T));
}


