#include "emXGUI.h"
#include "GUI_NES_DIALOG.h"
#include "host_gamepad.h"
#include "usb_host_app.h"
#include "InfoNES.h"
NES_DIALOG_Typedef g_NES_Dialog;
void Start_NES(void* param)
{
  while(1)
  {
    NES_Main("0:4.nes");
  }
}
__attribute__((at(0x81000000)))u16 buff[256][242];
static LRESULT GUI_NES_PROC(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch(msg)
  {
    case WM_CREATE:
    {
      //分配画面缓冲区
      g_NES_Dialog.buf = (u16*)GUI_VMEM_Alloc(NES_DISP_WIDTH*NES_DISP_HEIGHT*sizeof(u16));
      memset(g_NES_Dialog.buf,255,NES_DISP_WIDTH*NES_DISP_HEIGHT*sizeof(u16));
      
      SetTimer(hwnd, 1,10, TMR_SINGLE|TMR_START, NULL);
      break;
    }
    case WM_TIMER:
    {
      GUI_Thread_Create(Start_NES,"GUI_Nes",8*1024,NULL,5,5);
      break;
    }
    case WM_PAINT:
    {
      PAINTSTRUCT ps;
      SURFACE *pSurf;
      HDC hdc, hdc_mem;
      hdc = BeginPaint(hwnd,&ps);
      pSurf = CreateSurface(SURF_RGB565, NES_DISP_WIDTH, NES_DISP_HEIGHT, 0, g_NES_Dialog.buf);
      hdc_mem =CreateDC(pSurf,NULL);
//      
      StretchBlt(hdc, 0, 0, 800, 480, hdc_mem, 0, 0, NES_DISP_WIDTH,  NES_DISP_HEIGHT, SRCCOPY);
      DeleteSurface(pSurf);
      DeleteDC(hdc_mem);
//      SetBrushColor(hdc, MapRGB(hdc, 255, 255, 255));
      EndPaint(hwnd,&ps);
      break;      
      
    }
    case WM_LBUTTONUP:
    {
      PostCloseMessage(hwnd);
      break;
    }      
    
    
    
    case WM_DESTROY:
    {
      GUI_VMEM_Free(g_NES_Dialog.buf);
      return PostQuitMessage(hwnd);	
    }
    default:
      return DefWindowProc(hwnd, msg, wParam, lParam); 
  }
  
  return WM_NULL;

}

extern uint8_t Gamepad_state;
void	GUI_NES_DIALOG(void *param)
{
	
  WNDCLASS	wcex;
	MSG msg;

//  USB_HostApplicationInit();
//  while (1)
//  {
//      USB_Host_Polling();
//      if(Gamepad_state==0X0F)
//      {
//        break;
//      }					
//  }  

	wcex.Tag = WNDCLASS_TAG;

	wcex.Style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = GUI_NES_PROC; //设置主窗口消息处理的回调函数.
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = NULL;//hInst;
	wcex.hIcon = NULL;//LoadIcon(hInstance, (LPCTSTR)IDI_WIN32_APP_TEST);
	wcex.hCursor = NULL;//LoadCursor(NULL, IDC_ARROW);

	//创建主窗口
	g_NES_Dialog.hwnd = CreateWindowEx(WS_EX_NOFOCUS,
                        &wcex,
                        L"GUI_NES_DIALOG",
                        WS_VISIBLE,
                        0, 0, GUI_XSIZE, GUI_YSIZE,
                        NULL, NULL, NULL, NULL);
   //显示主窗口
	ShowWindow(g_NES_Dialog.hwnd, SW_SHOW);
	//开始窗口消息循环(窗口关闭并销毁时,GetMessage将返回FALSE,退出本消息循环)。
	while (GetMessage(&msg, g_NES_Dialog.hwnd))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}  
}

   




#if 1
void GUI_NES_DIALOGTest(void *param)
{
  static int thread = 0;
  int app = 0;
  
  if(thread == 0)
  {
     GUI_Thread_Create(GUI_NES_DIALOGTest,"GUI_Nes",1*1024,NULL,5,5);
     thread = 1;
     return;
  }
  if(thread == 1)
  {
		if(app==0)
		{
			app=1;
      GUI_NES_DIALOG(NULL);
			app=0;
			thread=0;
      
      GUI_Thread_Delete(GUI_GetCurThreadHandle());
		}    
  }
}
#endif

