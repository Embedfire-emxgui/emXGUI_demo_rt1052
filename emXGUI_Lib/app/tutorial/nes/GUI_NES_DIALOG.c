#include "emXGUI.h"
#include "GUI_NES_DIALOG.h"


NES_DIALOG_Typedef g_NES_Dialog;
void Start_NES(void* param)
{
  while(1)
  {
    NES_Main("0:4.nes");
  }
}
__attribute__((at(0x81000000)))u16 buff[800][480];
static LRESULT GUI_NES_PROC(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch(msg)
  {
    case WM_CREATE:
    {

//      g_NES_Dialog.buf = (u16*)GUI_VMEM_Alloc(GUI_XSIZE*GUI_YSIZE*sizeof(u16));
//      memset(g_NES_Dialog.buf,0,GUI_XSIZE*GUI_YSIZE*sizeof(u16));
      
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
      pSurf = CreateSurface(SURF_RGB565, 256, 242, 0, buff[0]);
      hdc_mem =CreateDC(pSurf,NULL);
      
      BitBlt(hdc, 0, 0, 256,  242, hdc_mem, 0 , 0, SRCCOPY);
      DeleteSurface(pSurf);
      DeleteDC(hdc_mem);
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


void	GUI_NES_DIALOG(void *param)
{
	
  WNDCLASS	wcex;
	MSG msg;


	wcex.Tag = WNDCLASS_TAG;

	wcex.Style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = GUI_NES_PROC; //设置主窗口消息处理的回调函数.
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = NULL;//hInst;
	wcex.hIcon = NULL;//LoadIcon(hInstance, (LPCTSTR)IDI_WIN32_APP_TEST);
	wcex.hCursor = NULL;//LoadCursor(NULL, IDC_ARROW);

	//创建主窗口
	g_NES_Dialog.hwnd = CreateWindowEx(WS_EX_NOFOCUS|WS_EX_FRAMEBUFFER,
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

