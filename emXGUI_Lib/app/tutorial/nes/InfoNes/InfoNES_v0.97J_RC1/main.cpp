

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

//#include "Include.h"
#include "emXGUI.h"
#include "K6502.h"
#include "InfoNES_pAPU.h"
#include "InfoNES_Mapper.h"
#include "InfoNES.h"
#include "InfoNES_System.h"
#include "x_libc.h"
//#include "x_commdlg.h"
//
#include	"CListMenu.h"
extern "C"
{
#include "host_gamepad.h"
#include "usb_host_app.h"
#include "InfoNES.h"
//#include "music.h"
extern const uint8_t music[];
}
/*============================================================================*/
#define vmalloc GUI_VMEM_Alloc
#define vfree   GUI_VMEM_Free
#define SYS_msleep GUI_msleep


GUI_SEM* sai_complete_sem = NULL;

#if 1 

#define	kmalloc	vmalloc
#define	kfree	vfree

#endif
char file_list[50][100];
int file_nums = 0;
int cur_index = 1;
/*============================================================================*/
typedef enum
{
  e_Post_List = 0,//��ͨ�Ĺر��б�
  e_Post_All,     //�˳�����APP
  e_Post_OK
}Close_Typedef;

Close_Typedef exit_type = e_Post_List;

#define	__Name	L"NES模拟�?InfoNES)"

/*============================================================================*/

#define	SOUND_BUFS	4


int g_nCurrent=0;

static	HANDLE	hInst;

static	HWND 	hwnd_UI;
static	HWND 	hwnd_List;
static	HDC		hdc_NES;

static	int		nes_thread_run=TRUE;
static	U32 	SYS_KeyVal;
static	int		MenuRet;

//static	char	cur_dir[256];

//static	char	rom_dir[256];
//static	char	rom_path[256];


static	int		DrawFrameFlag=FALSE;
static	int		FullScreen=FALSE;

WORD *WorkFrame=NULL;  //todo: workframe


//.nes File Header
NesHeader *Neshd;
NesResource *NES;
ApuResource *APU;
CpuResource *CPU;
ApuEvent *ApuEventQueue;


Mapper1Res *MAP1;
Mapper4Res *MAP4;
Mapper6Res *MAP6;
Mapper21Res *MAP21;
Mapper23Res *MAP23;
Mapper33Res *MAP33;
Mapper118Res *MAP118;


BYTE *RAM;    //8K
BYTE *SRAM;   //8K

BYTE *PPURAM; //16K
BYTE *ChrBuf; //32K
BYTE *ROM;
BYTE *VROM;
BYTE *SRAMBANK; //SRAM BANK(8Kb)

//ROM BANK ( 8Kb*4 )
BYTE *ROMBANK0;
BYTE *ROMBANK1;
BYTE *ROMBANK2;
BYTE *ROMBANK3;
//1Kb*16
BYTE *PPUBANK[ 16 ];

WORD wave_buffers[1470];


/*============================================================================*/

/*-------------------------------------------------------------------*/
/*  Variables for Timer & Wait loop                                  */
/*-------------------------------------------------------------------*/
#define LINE_PER_TIMER     (240*1)
//#define TIMER_PER_LINE     (30)


//static HANDLE m_hThread;

//static U16 wLines;
//static U16 wLinePerTimer;
//MMRESULT uTimerID;
//static BOOL bWaitFlag;
//CRITICAL_SECTION WaitFlagCriticalSection;
BOOL bAutoFrameskip = TRUE;

static void CPU_Task(void)
{
	uint8_t CPU_RunInfo[400]; //保存任务运行时间信息

	memset(CPU_RunInfo,0,400); //信息缓冲区清�?

//	vTaskList((char *)&CPU_RunInfo); //获取任务运行时间信息

//	printf("---------------------------------------------\r\n");
//	printf("任务�?任务状�?优先�?剩余�?任务序号\r\n");
//	printf("%s", CPU_RunInfo);
//	printf("---------------------------------------------\r\n");

//	memset(CPU_RunInfo,0,400); //信息缓冲区清�?

	vTaskGetRunTimeStats((char *)&CPU_RunInfo);

	printf("任务�?运行计数 使用率\r\n");
	printf("%s", CPU_RunInfo);
	printf("---------------------------------------------\r\n\n");

}
/*============================================================================*/
/*===================================================================*/
/*                                                                   */
/*           LoadSRAM() : Load a SRAM                                */
/*                                                                   */
/*===================================================================*/
int LoadSRAM(void)
{

#if 0
/*
 *  Load a SRAM
 *
 *  Return values
 *     0 : Normally
 *    -1 : SRAM data couldn't be read
 */

  FILE *fp;
  unsigned char pSrcBuf[ SRAM_SIZE ];
  unsigned char chData;
  unsigned char chTag;
  int nRunLen;
  int nDecoded;
  int nDecLen;
  int nIdx;

  // It doesn't need to save it
  nSRAM_SaveFlag = 0;

  // It is finished if the ROM doesn't have SRAM
  if ( !ROM_SRAM )
    return 0;

  // There is necessity to save it
  nSRAM_SaveFlag = 1;

  // The preparation of the SRAM file name
  strcpy( szSaveName, szRomName );
  strcpy( strrchr( szSaveName, '.' ) + 1, "srm" );

  /*-------------------------------------------------------------------*/
  /*  Read a SRAM data                                                 */
  /*-------------------------------------------------------------------*/

  // Open SRAM file
  fp = fopen( szSaveName, "rb" );
  if ( fp == NULL )
    return -1;

  // Read SRAM data
  fread( pSrcBuf, SRAM_SIZE, 1, fp );

  // Close SRAM file
  fclose( fp );

  /*-------------------------------------------------------------------*/
  /*  Extract a SRAM data                                              */
  /*-------------------------------------------------------------------*/

  nDecoded = 0;
  nDecLen = 0;

  chTag = pSrcBuf[ nDecoded++ ];

  while ( nDecLen < 8192 )
  {
    chData = pSrcBuf[ nDecoded++ ];

    if ( chData == chTag )
    {
      chData = pSrcBuf[ nDecoded++ ];
      nRunLen = pSrcBuf[ nDecoded++ ];
      for ( nIdx = 0; nIdx < nRunLen + 1; ++nIdx )
      {
        SRAM[ nDecLen++ ] = chData;
      }
    }
    else
    {
      SRAM[ nDecLen++ ] = chData;
    }
  }
  #endif
  // Successful
  return 0;
  
}


/*===================================================================*/
/*                                                                   */
/*           SaveSRAM() : Save a SRAM                                */
/*                                                                   */
/*===================================================================*/
int SaveSRAM()
{
	return 0;

#if 0
/*
 *  Save a SRAM
 *
 *  Return values
 *     0 : Normally
 *    -1 : SRAM data couldn't be written
 */

  FILE *fp;
  int nUsedTable[ 256 ];
  unsigned char chData;
  unsigned char chPrevData;
  unsigned char chTag;
  int nIdx;
  int nEncoded;
  int nEncLen;
  int nRunLen;
  unsigned char pDstBuf[ SRAM_SIZE ];

  if ( !nSRAM_SaveFlag )
    return 0;  // It doesn't need to save it

  /*-------------------------------------------------------------------*/
  /*  Compress a SRAM data                                             */
  /*-------------------------------------------------------------------*/

  memset( nUsedTable, 0, sizeof nUsedTable );

  for ( nIdx = 0; nIdx < SRAM_SIZE; ++nIdx )
  {
    ++nUsedTable[ SRAM[ nIdx++ ] ];
  }
  for ( nIdx = 1, chTag = 0; nIdx < 256; ++nIdx )
  {
    if ( nUsedTable[ nIdx ] < nUsedTable[ chTag ] )
      chTag = nIdx;
  }

  nEncoded = 0;
  nEncLen = 0;
  nRunLen = 1;

  pDstBuf[ nEncLen++ ] = chTag;

  chPrevData = SRAM[ nEncoded++ ];

  while ( nEncoded < SRAM_SIZE && nEncLen < SRAM_SIZE - 133 )
  {
    chData = SRAM[ nEncoded++ ];

    if ( chPrevData == chData && nRunLen < 256 )
      ++nRunLen;
    else
    {
      if ( nRunLen >= 4 || chPrevData == chTag )
      {
        pDstBuf[ nEncLen++ ] = chTag;
        pDstBuf[ nEncLen++ ] = chPrevData;
        pDstBuf[ nEncLen++ ] = nRunLen - 1;
      }
      else
      {
        for ( nIdx = 0; nIdx < nRunLen; ++nIdx )
          pDstBuf[ nEncLen++ ] = chPrevData;
      }

      chPrevData = chData;
      nRunLen = 1;
    }

  }
  if ( nRunLen >= 4 || chPrevData == chTag )
  {
    pDstBuf[ nEncLen++ ] = chTag;
    pDstBuf[ nEncLen++ ] = chPrevData;
    pDstBuf[ nEncLen++ ] = nRunLen - 1;
  }
  else
  {
    for ( nIdx = 0; nIdx < nRunLen; ++nIdx )
      pDstBuf[ nEncLen++ ] = chPrevData;
  }

  /*-------------------------------------------------------------------*/
  /*  Write a SRAM data                                                */
  /*-------------------------------------------------------------------*/

  // Open SRAM file
  fp = fopen( szSaveName, "wb" );
  if ( fp == NULL )
    return -1;

  // Write SRAM data
  fwrite( pDstBuf, nEncLen, 1, fp );

  // Close SRAM file
  fclose( fp );
#endif
  // Successful
  
}

/*===================================================================*/
/*                                                                   */
/*                  InfoNES_Menu() : Menu screen                     */
/*                                                                   */
/*===================================================================*/
int InfoNES_Menu(void)
{
/*
 *  Menu screen
 *
 *  Return values
 *     0 : Normally
 *    -1 : Exit InfoNES
 */

	// Nothing to do here
  return MenuRet;
}


/*===================================================================*/
/*                                                                   */
/*               InfoNES_ReadRom() : Read ROM image file             */
/*                                                                   */
/*===================================================================*/
int InfoNES_ReadRom( const char *pszFileName )
{
/*
 *  Read ROM image file
 *
 *  Parameters
 *    const char *pszFileName          (Read)
 *
 *  Return values
 *     0 : Normally
 *    -1 : Error
 */

  FIL fp;
  FRESULT res_sd;
  int  size;
	int  nIdx;
//	char buf[64];
  UINT fnum;   
	res_sd = f_open(&fp,pszFileName,FA_OPEN_EXISTING | FA_READ) ;
	if(res_sd != FR_OK)
	{
		GUI_DEBUG("NES File Open Error!");
	}


	//Read ROM Header
	f_read(&fp,Neshd,16,&fnum);
	if ((Neshd->byID[0]!= 0x4e)||(Neshd->byID[1]!= 0x45)
		||(Neshd->byID[2]!= 0x53)||(Neshd->byID[3]!= 0x1a))
	{
		return -1;
	}
	
    //获取Mapper�?
    NES->MapperNo =Neshd->byInfo1>>4;
    for(nIdx=4;nIdx<8&&Neshd->byReserve[nIdx]==0;++nIdx );

    if(nIdx==8)
    {
    	NES->MapperNo|=(Neshd->byInfo2&0xf0);//Mapper Number is 8bits
    }

    //检查是否支持该Mapper
    nIdx =0;
    while(1)
    {
    	if(MapTab[ nIdx ].nMapperNo == NES->MapperNo)
    	{
    		break;
    	}

    	if(MapTab[ nIdx ].nMapperNo == -1)
    	{
    		GUI_DEBUG("Mapper is unsupported: #%d\r\n",NES->MapperNo);
    		f_close(&fp);
    		return -1;
    	}

    	nIdx++;
    }

    GUI_DEBUG("NES MapperNO: #%d\r\n",NES->MapperNo);

    switch(NES->MapperNo)
    {
        case 1:		MAP1=(Mapper1Res*)vmalloc(sizeof(Mapper1Res));break;
        case 4:		MAP4=(Mapper4Res*)vmalloc(sizeof(Mapper4Res));break;
        case 6:		MAP6=(Mapper6Res*)vmalloc(sizeof(Mapper6Res));break;
        case 21:	MAP21=(Mapper21Res*)vmalloc(sizeof(Mapper21Res));break;
        case 23:	MAP23=(Mapper23Res*)vmalloc(sizeof(Mapper23Res));break;
        case 33:	MAP33=(Mapper33Res*)vmalloc(sizeof(Mapper33Res));break;
        case 118:	MAP118=(Mapper118Res*)vmalloc(sizeof(Mapper118Res));break;

        default:	break;
    }

  	memset( SRAM, 0, SRAM_SIZE );
  	//If trainer presents Read Triner at 0x7000-0x71ff
  	if ( Neshd->byInfo1 & 4 )
  	{
      f_read(&fp,&SRAM[0x1000],512,&fnum);
  	}

  	size = Neshd->byRomSize * 0x4000 ;
    /* Allocate Memory for ROM Image */
    ROM = (U8 *)vmalloc( size );

    /* Read ROM Image */
    f_read(&fp,ROM,size,&fnum);
//    x_sprintf(buf,"ROMSize  = %d*0x4000\r\n",Neshd->byRomSize);
//    GUI_DEBUG(buf);

//    x_sprintf(buf,"VROMSize = %d*0x2000\r\n",Neshd->byVRomSize);
//    DebugPuts(buf);

    if ( Neshd->byVRomSize > 0 )
    {
      size = Neshd->byVRomSize * 0x2000;

      /* Allocate Memory for VROM Image */
      VROM = (U8*)vmalloc( size );
      if(VROM) /* Read VROM Image */
      {
     
        f_read(&fp,VROM,size,&fnum);
      }

    }
    else
    {
    	VROM =NULL;
    }

    /* File close */
    f_close( &fp );

  	return 0;

}

/*===================================================================*/
/*                                                                   */
/*           InfoNES_ReleaseRom() : Release a memory for ROM           */
/*                                                                   */
/*===================================================================*/
void InfoNES_ReleaseRom()
{
#if 1
/*
 *  Release a memory for ROM
 *
 */
    switch(NES->MapperNo)
    {
        case 1:		vfree(MAP1);MAP1=NULL;break;
        case 4:		vfree(MAP4);MAP4=NULL;break;
        case 6:		vfree(MAP6);MAP6=NULL;break;
        case 21:	vfree(MAP21);MAP21=NULL;break;
        case 23:	vfree(MAP23);MAP23=NULL;break;
        case 33:	vfree(MAP33);MAP33=NULL;break;
        case 118:	vfree(MAP118);MAP118=NULL;break;

        default:	break;
    }
	
  if ( ROM!=NULL )
  {
    vfree( ROM );
    ROM = NULL;
    GUI_DEBUG("Release ROM\r\n");
    
  }

  if ( VROM!=NULL )
  {
	vfree( VROM );
    VROM = NULL;
    GUI_DEBUG("Release VROM\r\n");
    
  }


  #endif
}

/*===================================================================*/
/*                                                                   */
/*      InfoNES_LoadFrame() :                                        */
/*           Transfer the contents of work frame on the screen       */
/*                                                                   */
/*===================================================================*/

//DWORD	t0,t1,frame=0,time;
static DWORD	nes_fps=0;
static DWORD	screen_fps=0;


void InfoNES_LoadFrame(void)
{
	static	int t0=0;
	static	int t1=0;
	static	int frame=0;

	int time;

	if(frame==0)
	{
		t0=GUI_GetTickCount();		
	}
				
	t1=GUI_GetTickCount();
	time	=t1-t0;
	if(time >= 1000)
	{
		nes_fps	=(frame*1000)/time;
		frame	=0;		
	}
	else
	{
		frame++;
	}
	////////
  
  	DrawFrameFlag=TRUE;
  	InvalidateRect(hwnd_UI,NULL,FALSE);
  	SYS_msleep(10);
}

#if 0
void InfoNES_LoadFrame(void)
{


/*
 *  Transfer the contents of work frame on the screen
 *
 */

  // Set screen data
  memcpy( pScreenMem, WorkFrame, NES_DISP_WIDTH * NES_DISP_HEIGHT * 2 );

  // Screen update
  HDC hDC = GetDC( hWndMain );

  HDC hMemDC = CreateCompatibleDC( hDC );

  HBITMAP hOldBmp = (HBITMAP)SelectObject( hMemDC, hScreenBmp );

  StretchBlt( hDC, 0, 0, NES_DISP_WIDTH * wScreenMagnification, 
              NES_DISP_HEIGHT * wScreenMagnification, hMemDC, 
              0, 0, NES_DISP_WIDTH, NES_DISP_HEIGHT, SRCCOPY );
 /*
   BitBlt( hDC, 0, 0, NES_DISP_WIDTH * wScreenMagnification, 
              NES_DISP_HEIGHT * wScreenMagnification, hMemDC, 
              0, 0,  SRCCOPY );
*/

  SelectObject( hMemDC, hOldBmp );

  DeleteDC( hMemDC );
  ReleaseDC( hWndMain, hDC );

}
#endif

/*===================================================================*/
/*                                                                   */
/*             InfoNES_PadState() : Get a joypad state               */
/*                                                                   */
/*===================================================================*/
extern uint8_t Gamepad_Val[2];
void InfoNES_PadState( DWORD *pdwPad1, DWORD *pdwPad2, DWORD *pdwSystem )
{

	*pdwPad1 = Gamepad_Val[0];
	*pdwPad2 = Gamepad_Val[1];
#if 0
/*
 *  Get a joypad state
 *
 *  Parameters
 *    U32 *pdwPad1                   (Write)
 *      Joypad 1 State
 *
 *    U32 *pdwPad2                   (Write)
 *      Joypad 2 State
 *
 *    U32 *pdwSystem                 (Write)
 *      Input for InfoNES
 *
 */


//  static U32 dwSysOld=0;


  /* Joypad 1 */
  /*
  *pdwPad1 =   ( GetAsyncKeyState( 'X' )        < 0 ) |
             ( ( GetAsyncKeyState( 'Z' )        < 0 ) << 1 ) |
             ( ( GetAsyncKeyState( 'A' )        < 0 ) << 2 ) |
             ( ( GetAsyncKeyState( 'S' )        < 0 ) << 3 ) |
             ( ( GetAsyncKeyState( VK_UP )      < 0 ) << 4 ) |
             ( ( GetAsyncKeyState( VK_DOWN )    < 0 ) << 5 ) |
             ( ( GetAsyncKeyState( VK_LEFT )    < 0 ) << 6 ) |
             ( ( GetAsyncKeyState( VK_RIGHT )   < 0 ) << 7 );
*/




   *pdwPad1	=P1_KeyVal|SYS_KeyVal;
   *pdwPad2 =P2_KeyVal;

   *pdwSystem =SYS_KeyVal;

   return;



  ////


  #endif
}

/*===================================================================*/
/*                                                                   */
/*             InfoNES_MemoryCopy() : memcpy                         */
/*                                                                   */
/*===================================================================*/
void *InfoNES_MemoryCopy( void *dest, const void *src, int count )
{
/*
	int i;
	U8 *s,*d;
	
	s=(U8*)src;
	d=(U8*)dest;
	for(i=0;i<count;i++)
	{
		*d++=*s++;
	}
	return	dest;
*/
/*
 *  memcpy
 *
 *  Parameters
 *    void *dest                       (Write)
 *      Points to the starting address of the copied block乫s destination
 *
 *    const void *src                  (Read)
 *      Points to the starting address of the block of memory to copy
 *
 *    int count                        (Read)
 *      Specifies the size, in U8s, of the block of memory to copy
 *
 *  Return values
 *    Pointer of destination
 */
 

  memcpy( dest, (void*)src, count );
  return dest;


}

/*===================================================================*/
/*                                                                   */
/*             InfoNES_MemorySet() : Get a joypad state              */
/*                                                                   */
/*===================================================================*/
void *InfoNES_MemorySet( void *dest, int c, int count )
{ 
 /*
	int i;
	U8	*d;
	
	d=(U8*)dest;
	for(i=0;i<count;i++)
	{
		*d++=c;
	}
	return	dest;
*/

/*
 *  memset
 *
 *  Parameters
 *    void *dest                       (Write)
 *      Points to the starting address of the block of memory to fill
 *
 *    int c                            (Read)
 *      Specifies the U8 value with which to fill the memory block
 *
 *    int count                        (Read)
 *      Specifies the size, in U8s, of the block of memory to fill
 *
 *  Return values
 *    Pointer of destination
 */
 

  memset( dest,  c ,count); 
  return dest;
 
 
}

/*===================================================================*/
/*                                                                   */
/*                DebugPrint() : Print debug message                 */
/*                                                                   */
/*===================================================================*/
void InfoNES_DebugPrint( char *pszMsg )
{
//  GUI_DEBUG(pszMsg);
}

/*===================================================================*/
/*                                                                   */
/*        InfoNES_SoundInit() : Sound Emulation Initialize           */
/*                                                                   */
/*===================================================================*/
void InfoNES_SoundInit( void ) 
{


}

/*===================================================================*/
/*                                                                   */
/*        InfoNES_SoundOpen() : Sound Open                           */
/*                                                                   */
/*===================================================================*/
//char buf0[16*8192];
extern "C"
{
#include "fsl_sai_edma.h"
extern sai_edma_handle_t txHandle;
extern volatile bool isFinished;
}
#if 1
AT_NONCACHEABLE_SECTION_ALIGN(WORD Abuf1[735], 4);
AT_NONCACHEABLE_SECTION_ALIGN(WORD Abuf2[735], 4);
#else
WORD *Abuf1;
WORD *Abuf2;
#endif
__IO uint8_t Soundcount;
int InfoNES_SoundOpen( int samples_per_sync, int sample_rate ) 
{
#if 1
  sai_transfer_t xfer;
//	APU->Soundcount=0;
  Soundcount = 1;
  uint32_t temp = 0;
	NES->APU_Mute=0;
  
  xfer.data = (uint8_t *)Abuf1;
  xfer.dataSize = 367*2;  
  SAI_TransferSendEDMA(SAI1, &txHandle, &xfer);
  Soundcount=0;  
#endif  
	return 1;
}

/*===================================================================*/
/*                                                                   */
/*        InfoNES_SoundClose() : Sound Close                         */
/*                                                                   */
/*===================================================================*/
void InfoNES_SoundClose( void ) 
{

	return;
	
#if 0
  lpSndDevice->SoundClose();
  delete lpSndDevice;
#endif
//??	waveOutClose(0);
	//vfree(buf0);
}

/*===================================================================*/
/*                                                                   */
/*            InfoNES_SoundOutput4() : Sound Output 4 Waves          */           
/*                                                                   */
/*===================================================================*/
//#define	BUF_SIZE	(16*1024)
//#define	rec_freq	735

//U8 wave[ 1024 ];

void InfoNES_SoundOutput( int samples,WORD *wave )
{
  sai_transfer_t xfer = {0};
  uint32_t t0 = 0;
  uint32_t temp = 0;
//  int i;	
//  int count = 0;
#if 1
//  t0 = GUI_GetTickCount();
//  GUI_SemWait(sai_complete_sem, 0xFFFFFFFF);
  if(Soundcount)
  for(int i=0,t=0;i<samples;i++,t+=2)
  {     
//    if(i == 367)
//    {
//      GUI_DEBUG("%d",t);
//    }
    Abuf1[t] = wave[i]<<5;
    Abuf1[t+1] = wave[i]<<5;
  }
  else 
    for(int i=0,t=0;i<samples;i++,t+=2)
    {       
      Abuf2[t]= wave[i]<<5; 
      Abuf2[t+1]= wave[i]<<5; 
    }
  
//	while(!isFinished);     
//  isFinished = false;  
//  
//  if(Soundcount)
//  {
//      xfer.data = (uint8_t *)Abuf1;
//      xfer.dataSize = 367*2;  
//      SAI_TransferSendEDMA(SAI1, &txHandle, &xfer);
//      Soundcount=0;
//  }
//  else
//  {
//			/*  xfer structure */
//      xfer.data = (uint8_t *)Abuf2;
//      xfer.dataSize = 367*2;  
//      SAI_TransferSendEDMA(SAI1, &txHandle, &xfer);
//      Soundcount=1;
//  }
//  GUI_DEBUG("%d", GUI_GetTickCount()-t0);
#endif  
}

void InfoNES_MessageBox( char *pszMsg, ... )
{

}
/*===================================================================*/
/*                                                                   */
/*            InfoNES_StartTimer() : Start MM Timer                  */           
/*                                                                   */
/*===================================================================*/
static void InfoNES_StartTimer()
{
#if 0
  TIMECAPS caps;

  timeGetDevCaps( &caps, sizeof(caps) );
  timeBeginPeriod( caps.wPeriodMin );

  uTimerID = 
    timeSetEvent( caps.wPeriodMin * TIMER_PER_LINE, caps.wPeriodMin, TimerFunc, 0, (UINT)TIME_PERIODIC );

  // Calculate proper timing
  wLinePerTimer = LINE_PER_TIMER * caps.wPeriodMin;

  // Initialize timer variables
  wLines = 0;
  bWaitFlag = TRUE;

  // Initialize Critical Section Object
  InitializeCriticalSection( &WaitFlagCriticalSection );
 #endif
}

/*===================================================================*/
/*                                                                   */
/*            InfoNES_StopTimer() : Stop MM Timer                    */           
/*                                                                   */
/*===================================================================*/
static void InfoNES_StopTimer()
{
#if 0
  if ( 0 != uTimerID )
  {
    TIMECAPS caps;
    timeKillEvent( uTimerID );
    uTimerID = 0;
    timeGetDevCaps( &caps, sizeof(caps) );
    timeEndPeriod( caps.wPeriodMin * TIMER_PER_LINE );
  }
  // Delete Critical Section Object
  DeleteCriticalSection( &WaitFlagCriticalSection );
 #endif
}

/*===================================================================*/
/*                                                                   */
/*           TimerProc() : MM Timer Callback Function                */
/*                                                                   */
/*===================================================================*/
static void TimerFunc( UINT nID, UINT uMsg, U32 dwUser, U32 dw1, U32 dw2)
{
#if 0
  if ( NULL != m_hThread )
  {  
   // EnterCriticalSection( &WaitFlagCriticalSection );
    bWaitFlag = FALSE;
   // LeaveCriticalSection( &WaitFlagCriticalSection );
  
  }
  #endif
}

/*===================================================================*/
/*                                                                   */
/*            InfoNES_Wait() : Wait Emulation if required            */
/*                                                                   */
/*===================================================================*/
void InfoNES_Wait()
{
//	FrameSkip		=1;
	
   /*
  if(wLines++ > wLinePerTimer)
  {
  	wLines	=0;
  	//Sleep(0);
  }*/

}

/*============================================================================*/
/*============================================================================*/
/*============================================================================*/
/*============================================================================*/
/*============================================================================*/
/*============================================================================*/
/*============================================================================*/
/*============================================================================*/

/*============================================================================*/
/*============================================================================*/
#define	NES_MENU_QUIT	1
#define	NES_MENU_LOAD	2


#define	NES_NULL	0
#define	NES_RUN		1
#define	NES_LOAD	2
#define	NES_QUIT	3

static	int	nes_cmd=NES_NULL;
/*============================================================================*/
extern "C"{

extern u16 sms_fb[];

}


static void draw_frame(HDC hdc)
{
//	LONG xx,yy,x,y,r,g,b;
//  	RECT rc;

  	BITMAP bm;
  	////
  WCHAR buf[128];

#if 1 //NES
  	bm.Format	=BM_XRGB1555;
	bm.Width	=NES_DISP_WIDTH;
	bm.Height	=NES_DISP_HEIGHT;
	bm.WidthBytes =NES_DISP_WIDTH*2;
	bm.LUT		=NULL;
	bm.Bits		=(U8*)WorkFrame;
#endif

#if 0 //SMS
  	bm.Format	=BM_XRGB1555;
	bm.Width	=320;
	bm.Height	=256;
	bm.WidthBytes =bm.Width*2;
	bm.LUT		=NULL;
	bm.Bits		=(U8*)sms_fb;
#endif
	
	DrawBitmap(hdc_NES,0,0,&bm,NULL);

	if(1)
  	{
  	  	

//  	  	rc.x	=0;
//  	  	rc.y	=0;
//  	  	rc.w	=200;
//  	  	rc.h	=16;
  	  	x_wsprintf(buf,L"FPS: %d/%d",nes_fps,screen_fps);
//        GUI_DEBUG("%d", nes_fps);
        SetTextColor(hdc_NES,MapRGB(hdc,255,0,0));
  	  	TextOut(hdc_NES,1,1,buf,-1);

  	}

  	if(FullScreen == FALSE)
  	{

  	 	BitBlt(hdc,0,0,256,240,hdc_NES,0,0,SRCCOPY);
  	}
  	else
  	{

  		StretchBlt(hdc,0,0,GUI_XSIZE,GUI_YSIZE,hdc_NES,0,0,256,240,SRCCOPY);
	}
  		
}


/*============================================================================*/

static	void	nes_thread(void *p)
{

//	char buf[128];

//	SYS_thread_set_priority(NULL,-2);

//	wLinePerTimer 	= LINE_PER_TIMER;
//	wLines        	= 0;
//	bWaitFlag    	= TRUE;
	ROM		=NULL;
	VROM	=NULL;

	nes_thread_run =TRUE;
	
	while(1)
	{

		if(nes_cmd==NES_QUIT)
		{
			SYS_KeyVal =0;
//			P1_KeyVal =0;
//			P2_KeyVal =0;
			InfoNES_Fin();
			break;
		}
		////
		
		if(nes_cmd==NES_LOAD)
		{ 	
			SYS_KeyVal =0;
//			P1_KeyVal =0;
//			P2_KeyVal =0;
      //释放ROM
			InfoNES_Fin();

			if(InfoNES_Load(file_list[cur_index])==0)
			{
//				x_sprintf(buf,"VROM:        %08XH\r\n",VROM);
//				GUI_DEBUG(buf);

//				x_sprintf(buf,"ROM:         %08XH\r\n",ROM);
//				GUI_DEBUG(buf);

//				x_sprintf(buf,"RAM:         %08XH\r\n",RAM);
//				GUI_DEBUG(buf);

//				x_sprintf(buf,"SRAM:        %08XH\r\n",SRAM);
//				GUI_DEBUG(buf);

//				x_sprintf(buf,"PPURAM:      %08XH\r\n",PPURAM);
//				GUI_DEBUG(buf);

				InfoNES_Init();
				GUI_DEBUG("NES Start Run!\r\n");

				nes_cmd=NES_RUN;
			}
			else
			{
				nes_cmd =NES_NULL;
			}


		}
		////
		
		if(nes_cmd==NES_RUN)
		{
			InfoNES_Cycle();
		}
		////
		
		if(nes_cmd==NES_NULL)
		{
			SYS_msleep(50);
		}
		////*/
	}
	//PostSemaphore(h_draw);
	GUI_DEBUG("NES Thread Exit\r\n");
	nes_thread_run =FALSE;
	PostCloseMessage(hwnd_UI);
  GUI_Thread_Delete(GUI_GetCurThreadHandle());
}

/*============================================================================*/


/*============================================================================*/



#define	NES_FILE	1
#define	SMS_FILE	2
#define	UNKNOW_FILE 0xFF
#if 0
static void __scan_file(const char *path,struct dir_info *d_info)
{
	dir_obj *dir;
	int i;

	dir =SYS_opendir(path);
	if(dir!=NULL)
	{
		while(1)
		{
			char *fn;

			SYS_readdir(dir,d_info);

			fn =d_info->name_buf;
			if(fn[0] == 0)
			{
				break;
			}

			if(d_info->attr & FA_DIR)
			{
				continue;
			}

			i =x_strlen(fn);
			while(i > 0)
			{
				if(fn[i]=='.')
				{

					int type = UNKNOW_FILE;

					if(x_strncasecmp(&fn[i],".nes",4)==0)
					{
						type =NES_FILE;
					}

					if(x_strncasecmp(&fn[i],".sms",4)==0)
					{
						type =SMS_FILE;
					}

					if(type != UNKNOW_FILE)
					{
						WCHAR *wbuf;

						i	=(x_strlen(fn)+1)*2;

						wbuf	=(WCHAR*)vmalloc(i);
						x_mbstowcs_cp936(wbuf,fn,i);

						SendMessage(hListWnd,LB_ADDSTRING,SendMessage(hListWnd,LB_GETCOUNT,0,0),(LPARAM)wbuf);
						vfree(wbuf);

					}

					break;
				}

				i--;
			}


		}

		SYS_closedir(dir);

	}
}
#endif
/*============================================================================*/

#define	ID_START		0x1000
#define	ID_SELECT		0x1001
#define	ID_A			0x1002
#define	ID_B			0x1003
#define	ID_LOAD			0x1004
#define	ID_RESET		0x1005
#define	ID_FULLSCREEN	0x1006
#define	ID_CANCEL		0x1007

#define	ID_LIST			0x2001
static void MusicList_ReturnButton_OwnerDraw(DRAWITEM_HDR *ds) //绘制一个按钮外�?
{
  HWND hwnd;
  HDC hdc;
  RECT rc;
  WCHAR wbuf[128];

  hwnd = ds->hwnd; //button的窗口句�?
  hdc = ds->hDC;   //button的绘图上下文句柄.
  rc = ds->rc;     //button的绘制矩形区.




  SetBrushColor(hdc, MapRGB(hdc, 0,0,0));

  FillCircle(hdc, rc.x, rc.y, rc.w);
  SetBrushColor(hdc, MapRGB(hdc, 0,0,0));
  FillRect(hdc, &rc); //用矩形填充背�?

  if (IsWindowEnabled(hwnd) == FALSE)
  {
    SetTextColor(hdc, MapRGB(hdc, 165,160,160));
  }
  else if (ds->State & BST_PUSHED)
  { //按钮是按下状�?
  //    GUI_DEBUG("ds->ID=%d,BST_PUSHED",ds->ID);
  //		SetBrushColor(hdc,MapRGB(hdc,150,200,250)); //设置填充�?BrushColor用于所有Fill类型的绘图函�?
  //		SetPenColor(hdc,MapRGB(hdc,250,0,0));        //设置绘制�?PenColor用于所有Draw类型的绘图函�?
    SetTextColor(hdc, MapRGB(hdc, 105, 105, 105));      //设置文字�?
  }
  else
  { //按钮是弹起状�?
  //		SetBrushColor(hdc,MapRGB(hdc,255,255,255));
  //		SetPenColor(hdc,MapRGB(hdc,0,250,0));
    SetTextColor(hdc, MapRGB(hdc, 255, 255, 255));
  }


  //	SetBrushColor(hdc,COLOR_BACK_GROUND);

  //	FillRect(hdc,&rc); //用矩形填充背�?
  //	DrawRect(hdc,&rc); //画矩形外�?
  //  
  //  FillCircle(hdc,rc.x+rc.w/2,rc.x+rc.w/2,rc.w/2); //用矩形填充背景FillCircle
  //	DrawCircle(hdc,rc.x+rc.w/2,rc.x+rc.w/2,rc.w/2); //画矩形外�?

  /* 使用控制图标字体 */
  SetFont(hdc, ctrlFont48);
  //  SetTextColor(hdc,MapRGB(hdc,255,255,255));

  GetWindowText(ds->hwnd, wbuf, 128); //获得按钮控件的文�?

  DrawText(hdc, wbuf, -1, &rc, DT_VCENTER);//绘制文字(居中对齐方式)
  rc.x = 35; 
  //   rc.y = 20;
  /* 恢复默认字体 */
  SetFont(hdc, defaultFont);
  DrawText(hdc, L"����", -1, &rc, DT_VCENTER);
}



static void MusicList_Button_OwnerDraw(DRAWITEM_HDR *ds) //绘制一个按钮外�?
{
	HWND hwnd;
	HDC hdc;
	RECT rc;
	WCHAR wbuf[128];

	hwnd = ds->hwnd; //button的窗口句�?
	hdc = ds->hDC;   //button的绘图上下文句柄.
	rc = ds->rc;     //button的绘制矩形区.

	SetBrushColor(hdc, MapRGB(hdc, 0,0,0));
	FillRect(hdc, &rc); //用矩形填充背�?

	if (IsWindowEnabled(hwnd) == FALSE)
	{
		SetTextColor(hdc, MapRGB(hdc, 165,160,160));
	}
	else if (ds->State & BST_PUSHED)
	{ //按钮是按下状�?
//    GUI_DEBUG("ds->ID=%d,BST_PUSHED",ds->ID);
//		SetBrushColor(hdc,MapRGB(hdc,150,200,250)); //设置填充�?BrushColor用于所有Fill类型的绘图函�?
//		SetPenColor(hdc,MapRGB(hdc,250,0,0));        //设置绘制�?PenColor用于所有Draw类型的绘图函�?
		SetTextColor(hdc, MapRGB(hdc, 105, 105, 105));      //设置文字�?
	}
	else
	{ //按钮是弹起状�?
//		SetBrushColor(hdc,MapRGB(hdc,255,255,255));
//		SetPenColor(hdc,MapRGB(hdc,0,250,0));
		SetTextColor(hdc, MapRGB(hdc, 255, 255, 255));
	}


	//	SetBrushColor(hdc,COLOR_BACK_GROUND);

	//	FillRect(hdc,&rc); //用矩形填充背�?
	//	DrawRect(hdc,&rc); //画矩形外�?
	//  
	//  FillCircle(hdc,rc.x+rc.w/2,rc.x+rc.w/2,rc.w/2); //用矩形填充背景FillCircle
	//	DrawCircle(hdc,rc.x+rc.w/2,rc.x+rc.w/2,rc.w/2); //画矩形外�?

	  /* 使用控制图标字体 */
	SetFont(hdc, ctrlFont64);
	//  SetTextColor(hdc,MapRGB(hdc,255,255,255));

	GetWindowText(ds->hwnd, wbuf, 128); //获得按钮控件的文�?

	DrawText(hdc, wbuf, -1, &rc, DT_VCENTER | DT_CENTER);//绘制文字(居中对齐方式)


  /* 恢复默认字体 */
	SetFont(hdc, defaultFont);

}

/*============================================================================*/
/*============================================================================*/
#if 0
static	struct	scan_dir_data s_data;
static	char	scan_file_path[512];
static	char	scan_file_name_buf[256];

static	LRESULT	LoadProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	int x,y;
	RECT rc;
	
	switch(msg)
	{
		case	WM_CREATE:
		
				x=0;
				y=0;

				GetClientRect(hwnd,&rc);

				hListWnd=CreateWindow(LISTBOX,L"list",WS_VISIBLE,x,y,rc.w-x,rc.h-32,hwnd,ID_LIST,hInst,0);

				//x_sprintf(scan_file_path,"%s\\ROM",cur_dir);
				x_strcpy(scan_file_path,rom_dir);

				if(1)
				{
					struct dir_info d_info;

					d_info.name_buf =scan_file_name_buf;
					d_info.name_buf_size =sizeof(scan_file_name_buf);

					__scan_file(scan_file_path,&d_info);
				}

				x=0;
				y=rc.h-32;
				CreateWindow(BUTTON,L"取消",WS_BORDER|WS_VISIBLE,x,y,rc.w>>1,32,hwnd,ID_CANCEL,hInst,0);

				x=rc.w>>1;
				CreateWindow(BUTTON,L"确定",WS_BORDER|WS_VISIBLE,x,y,rc.w>>1,32,hwnd,ID_LOAD,hInst,0);
		
				break;
				/////
				
		case	WM_NOTIFY:
				{
					U16	code,id;
					
					code	=HIWORD(wParam);
					id		=LOWORD(wParam);
					
					if(id==ID_CANCEL )
					{
						PostCloseMessage(hwnd);
					}
					
					if(id==ID_LOAD)
					{
						int i;
						WCHAR text_buf[128];
						WCHAR wbuf[128];
						CHAR  buf[128];
																				
						i=SendMessage(GetDlgItem(hwnd,ID_LIST),LB_GETCURSEL,0,0);
						SendMessage(GetDlgItem(hwnd,ID_LIST),LB_GETTEXT,i,(LPARAM)wbuf);
							
						x_wcstombs_cp936(buf,wbuf,128);
							
							
						x_sprintf(rom_path,"%s/%s",rom_dir,buf);
										
						//	DebugPuts("Load ROM: ");
						//	DebugPuts(rom_path);
						//	DebugPuts("\r\n");
								
						SYS_KeyVal	|=PAD_SYS_QUIT;
						nes_cmd=NES_LOAD;
						////
							
						x_wsprintf(text_buf,L"%s:%s",__Name,wbuf);
						SetWindowText(hwnd_UI,text_buf);
						PostMessage(hwnd_UI,WM_NCPAINT,0,0);
							
						SendMessage(hwnd,WM_CLOSE,0,0);
					}
					
					
				}
				break;
				////////
				
				
		default:
				return	DefWindowProc(hwnd,msg,wParam,lParam);
	
	}
	return	WM_NULL;
}

/*============================================================================*/

static	int	DlgLoadFile(HWND hParent)
{

	HWND hwnd;
	MSG msg;
	WNDCLASS wcex;
	RECT rc;
	POINT pt;
	
	////

	//hInst	=hInstance;
	

//	EnableWindow(hParent,FALSE);
	
	wcex.Tag 	= WNDCLASS_TAG;

	wcex.Style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)LoadProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInst;
	wcex.hIcon			= 0;//LoadIcon(hInstance, (LPCTSTR)IDI_WIN32_APP_TEST);
	wcex.hCursor		= 0;//LoadCursor(NULL, IDC_ARROW);

//	RegisterClassEx(&wcex);

	GetClientRect(hParent,&rc);
	pt.x=0;
	pt.y=0;
	ClientToScreen(hParent,&pt,1);
	rc.x	=pt.x;
	rc.y	=pt.y;
	
	InflateRectEx(&rc,-20,-20,-20,-40);

	DialogWindow(
			&wcex,L"Load NES/SMS File",WS_BORDER|WS_DLGFRAME|WS_VISIBLE,
			rc.x,rc.y,rc.w,rc.h,
			hParent,0,hInst,NULL);

#if 0
	hwnd	=CreateWindowEx(	WS_EX_NODRAG,&wcex,L"Load NES File",WS_BORDER|WS_DLGFRAME|WS_VISIBLE,
								rc.x,rc.y,rc.w,rc.h,
								NULL,0,hInst,NULL);
							
	ShowWindow(hwnd,SW_SHOW);
	UpdateWindow(hwnd);
	while(GetMessage(&msg,hwnd))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}	
#endif
//	EnableWindow(hParent,TRUE);
//	ShowWindow(hParent,SW_SHOW);
	return	TRUE;
}
#endif
/*============================================================================*/
/*
void	Load(HWND hParent)
{
	
	SYS_thread_create(LoadDlg,hParent,8192,NULL,0);
	
}
*/
/*============================================================================*/


static LRESULT Dlg_Load_WinProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
  static struct __obj_list *menu_list = NULL;
  static WCHAR (*wbuf)[128];    
  switch(msg)
  {
    case WM_CREATE:
    {
      int i = 0;
      list_menu_cfg_t cfg;
      RECT rc;
      GetClientRect(hwnd, &rc);   
      /* 需要分配N+1项，最后一项为�?*/
      menu_list = (struct __obj_list *)GUI_VMEM_Alloc(sizeof(struct __obj_list)*(file_nums+1));
      wbuf = (WCHAR (*)[128])GUI_VMEM_Alloc(sizeof(WCHAR *)*file_nums);  
      if(menu_list == NULL) return 0;
      
      for(;i < file_nums; i++)
      {
        char *p ;
//        GUI_DEBUG("%s", file_list[i]);
        //strcpy(p, file_list[i]);
        
        //删除目录字段
        p = strstr(file_list[i], "nes/");
            
        GUI_DEBUG("%s", p+4);
        x_mbstowcs_cp936(wbuf[i], p+4, 100);
        menu_list[i].pName = wbuf[i];
        menu_list[i].cbStartup = NULL;
        menu_list[i].icon = (void*)L"S";
        menu_list[i].bmp = NULL;
        menu_list[i].color = RGB_WHITE;
      } 
      /* 最后一项为�?*/
      menu_list[i].pName = NULL;
      menu_list[i].cbStartup = NULL;
      menu_list[i].icon = NULL;
      menu_list[i].bmp = NULL;
      menu_list[i].color = NULL;         

      cfg.list_objs = menu_list; 
      cfg.x_num = 3;
      cfg.y_num = 2; 
      cfg.bg_color = 0;
      CreateWindow(&wcex_ListMenu,
                          L"ListMenu1",
                          WS_VISIBLE | LMS_ICONFRAME|LMS_PAGEMOVE,
                          rc.x + 100, rc.y + 80, rc.w - 200, rc.h-80,
                          hwnd,
                          0x2000,
                          NULL,
                          &cfg); 
      SendMessage(GetDlgItem(hwnd,0x2000), MSG_SET_SEL, 0, 0);
      CreateWindow(BUTTON, L"L", BS_FLAT | BS_NOTIFY | WS_OWNERDRAW |WS_VISIBLE,
                   0, rc.h * 1 / 2, 70, 70, hwnd, 0x2001, NULL, NULL);
      SetWindowFont(GetDlgItem(hwnd,0x2001), ctrlFont48); 
      CreateWindow(BUTTON, L"K", BS_FLAT | BS_NOTIFY | WS_OWNERDRAW | WS_VISIBLE,
                   rc.w - 65, rc.h * 1 / 2, 70, 70, hwnd, 0x2002, NULL, NULL);
      SetWindowFont(GetDlgItem(hwnd,0x2002), ctrlFont48);   
      CreateWindow(BUTTON, L"F", BS_FLAT | BS_NOTIFY|WS_OWNERDRAW |WS_VISIBLE,
                   0, 0, 240, 80, hwnd, 0x2003, NULL, NULL);  



      break;
    }    
    case WM_PAINT:
    {
      PAINTSTRUCT ps;
      HDC hdc;
      RECT rc;//窗口大小
      GetClientRect(hwnd, &rc); //获得客户区矩�?
      hdc = BeginPaint(hwnd, &ps);
 
      //背景
      SetBrushColor(hdc, MapRGB(hdc, 0,0,0));
      FillRect(hdc, &rc);  
      //DrawBitmap(hdc,0,0,&bm_0,NULL);   
      rc.x = 0;
      rc.y = 0;
      rc.w = 800;
      rc.h = 80;
      SetTextColor(hdc, MapRGB(hdc, 250, 250, 250));
      DrawText(hdc, L"��Ϸ�б�", -1, &rc, DT_VCENTER|DT_CENTER);
      EndPaint(hwnd, &ps);
      break;
    }  
    case WM_DRAWITEM:
    {

      DRAWITEM_HDR *ds;

      ds = (DRAWITEM_HDR*)lParam;

      if(ds->ID == 0x2003)
        MusicList_ReturnButton_OwnerDraw(ds);
      else
        MusicList_Button_OwnerDraw(ds); //执行自绘制按�?
      return TRUE;

    } 
    case WM_NOTIFY:
    {
      u16 code, id;	
      LM_NMHDR *nm;
      code = HIWORD(wParam);
      id = LOWORD(wParam); 

      nm = (LM_NMHDR*)lParam;

      if (code == LMN_CLICKED)
      {
        switch (id)
        {
          case 0x2000:
          {
//            HWND wnd;
            cur_index = nm->idx;//切换至下一�?
//            SYS_KeyVal	|=PAD_SYS_QUIT;
            NES->PAD_System = PAD_SYS_QUIT;
            nes_cmd = NES_LOAD;
                   
            exit_type = e_Post_List;
            PostCloseMessage(hwnd);
                       
            
            
          }

        break;
        }

      }


      if (code == BN_CLICKED && id == 0x2001)
      {
        SendMessage(GetDlgItem(hwnd, 0x2000), MSG_MOVE_PREV, TRUE, 0);
      }

      if (code == BN_CLICKED && id == 0x2002)
      {
        SendMessage(GetDlgItem(hwnd, 0x2000), MSG_MOVE_NEXT, TRUE, 0);
      }         
      if (code == BN_CLICKED && id == 0x2003)
      {
        exit_type = e_Post_All;
        PostCloseMessage(hwnd);
      }   
      break;
    }    
    case WM_CLOSE:
    {
//      NES->PAD_System = PAD_SYS_QUIT;
//      nes_cmd=NES_LOAD;
      if(exit_type == e_Post_List)
      {
        GUI_VMEM_Free(menu_list);
        GUI_VMEM_Free(wbuf); 
        return DestroyWindow(hwnd);
      }
      if(exit_type == e_Post_All)
      {
        GUI_VMEM_Free(menu_list);
        GUI_VMEM_Free(wbuf);  
        PostCloseMessage(hwnd_UI); 
        return DestroyWindow(hwnd);        
      }    
//      else if(exit_type == e_Post_OK)
//        DestroyWindow(hwnd); //����DestroyWindow���������ٴ��ڣ��ú��������WM_DESTROY��Ϣ����
      break;
    }
		default:
				return	DefWindowProc(hwnd,msg,wParam,lParam);    
  }
  return WM_NULL;

}

static	RECT def_win_rc;
static	long def_win_style;

static FRESULT f_readdir_gui(char* dir,DIR* directory,FILINFO* fileInformation)
{
  FRESULT error = FR_OK;
  DIR* g_directory = directory; /* Directory object */
  FILINFO* g_fileInformation = fileInformation;
  
  
  error = f_opendir(g_directory, dir);
  if (error)
  {
    PRINTF("打开路径失败\r\n");
    return error;
  }
  
  for (;;)
  {
    error = f_readdir(g_directory, g_fileInformation);
    
    /* To the end. */
    if ((error != FR_OK) || (g_fileInformation->fname[0U] == 0U))
    {
      break;
    }
    if (g_fileInformation->fname[0] == '.')
    {
      continue;
    }
    if (g_fileInformation->fattrib & AM_DIR)
    {
      PRINTF("文件�?: %s\r\n", g_fileInformation->fname);
    }
    else
    {
      x_sprintf(file_list[file_nums],"%s/%s", dir, g_fileInformation->fname);
//      GUI_DEBUG("%s", file_list[file_nums]);
      file_nums++;
     
    }  
  }
  return error;
}



bool usb_HostTask_run = TRUE;
bool usb_HostTask_state = TRUE;
static void USB_HostTask(void *param)
{
    while (usb_HostTask_run==TRUE)
    {
        //GUI_DEBUG("2");
        USB_HostTaskFn(param);
    }
    usb_HostTask_state = FALSE;
    PostCloseMessage(hwnd_UI);
    GUI_Thread_Delete(GUI_GetCurThreadHandle());
}
bool usb_HostApplicationMouse_run = TRUE;
bool usb_HostApplicationMouse_state = TRUE;
static void USB_HostApplicationMouseTask(void *param)
{
    while (usb_HostApplicationMouse_run==TRUE)
    {
      //GUI_DEBUG("1");
        USB_HostHidGamepad1Task(param);
      
    }
    usb_HostApplicationMouse_state = FALSE;
    PostCloseMessage(hwnd_UI);
    GUI_Thread_Delete(GUI_GetCurThreadHandle());
}
DIR dir_object;    //目录对象结构�?
FILINFO file_info; //文件信息描述结构�?
static	LRESULT	WinProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	HDC  hdc;
	RECT rc,rc0,m_rc[5];

	switch(msg)
	{
		case	WM_CREATE:
		
				GetWindowRect(hwnd,&def_win_rc);
				def_win_style =GetWindowLong(hwnd,GWL_STYLE);
				hwnd_UI		=hwnd;
				
//				P1_KeyVal =0;
//				P2_KeyVal =0;
				SYS_KeyVal =0;

				MenuRet		=0;
        usb_HostTask_run = TRUE;
        usb_HostTask_state = TRUE;
        usb_HostApplicationMouse_run = TRUE;
        usb_HostApplicationMouse_state = TRUE;
//				rom_path[0]	=0;
				nes_cmd		=NES_NULL;
				
				nes_fps		=0;
				hdc_NES		=CreateMemoryDC(SURF_SCREEN,256,240);
				ClrDisplay(hdc_NES,NULL,MapRGB(hdc_NES,0,0,0));
				
				GetClientRect(hwnd,&rc);
//        FullScreen=TRUE;
				SetRectArea(&rc0,2,rc.h-24,rc.w-2*2,24);
        
        sai_complete_sem = GUI_SemCreate(0, 1);
        
				MakeMatrixRect(m_rc,&rc0,2,1,5,1);
				CreateWindow(BUTTON,L"ROMS",WS_BORDER|WS_VISIBLE|BS_NOTIFY,m_rc[0].x,m_rc[0].y,m_rc[0].w,m_rc[0].h,hwnd,ID_LOAD,hInst,0);
				CreateWindow(BUTTON,L"Start",WS_BORDER|WS_VISIBLE|BS_NOTIFY,m_rc[1].x,m_rc[1].y,m_rc[1].w,m_rc[1].h,hwnd,ID_START,hInst,0);
				CreateWindow(BUTTON,L"Select",WS_BORDER|WS_VISIBLE|BS_NOTIFY,m_rc[2].x,m_rc[2].y,m_rc[2].w,m_rc[2].h,hwnd,ID_SELECT,hInst,0);
				CreateWindow(BUTTON,L"Reset",WS_BORDER|WS_VISIBLE|BS_NOTIFY,m_rc[3].x,m_rc[3].y,m_rc[3].w,m_rc[3].h,hwnd,ID_RESET,hInst,0);
				CreateWindow(BUTTON,L"全屏",WS_BORDER|WS_VISIBLE|BS_NOTIFY,m_rc[4].x,m_rc[4].y,m_rc[4].w,m_rc[4].h,hwnd,ID_FULLSCREEN,hInst,0);
				
        
				nes_thread_run=FALSE;
//				SYS_thread_create(nes_thread,NULL,8*1024,NULL,0);
        GUI_Thread_Create(nes_thread, "NES", 12*256,NULL, 5,5);
////				SYS_thread_create(sms_thread,NULL,10*1024,NULL,0);
//        GUI_Thread_Create(USB_HostTask, "NESUSB", 12*1024,NULL, 3,5);
//        GUI_Thread_Create(USB_HostApplicationMouseTask, "NESUSHHOST", 12*1024,NULL, 4,5);
				/*
				sprintf(buf,"%s\\ROM",cur_dir);
				scan_files_to_list(GetDlgItem(hwnd,ID_LIST),buf);
				*/
    if (xTaskCreate(USB_HostTask, "usb host task", 2000L / sizeof(portSTACK_TYPE), g_HostHandle, 4, NULL) != pdPASS)
    {
        usb_echo("create host task error\r\n");
    }
    if (xTaskCreate(USB_HostApplicationMouseTask, "mouse task", 2000L / sizeof(portSTACK_TYPE), &g_HostHidGamepad1, 3,
                    NULL) != pdPASS)
    {
        usb_echo("create mouse task error\r\n");
    }          
    NMHDR nm;
    nm.code = BN_CLICKED;//HIWORD(wParam);
    nm.idFrom = ID_FULLSCREEN;//LOWORD(wParam);        
    SendMessage(hwnd, WM_NOTIFY, NULL, (LPARAM)&nm);
//    SetTimer(hwnd,1,1000,TMR_START,NULL);
    break;
				////////
		
		case	WM_TIMER:
		{
			//NES_Execu();
      CPU_Task();
      
//        USB_HostTaskFn(g_HostHandle); 
//        USB_HostHidGamepad1Task(&g_HostHidGamepad1);            
		}
		break;
		////
#if 0
		case	WM_KEYDOWN:
				
				switch(wParam)
				{
					case	VK_UP:
							//bugPrintf("KeyDown ->VK_UP\n");
							P1_KeyVal	|=NES_VK_UP;
							break;
							////////
					
					case	VK_DOWN:
							//bugPrintf("KeyDown ->VK_DOWN\n");
							P1_KeyVal	|=NES_VK_DOWN;
							break;
							////////
							
					case	VK_LEFT:
							//DebugPrintf("KeyDown ->VK_LEFT\n");
							P1_KeyVal	|=NES_VK_LEFT;
							break;
							////////
							
					case	VK_RIGHT:
							//DebugPrintf("KeyDown ->VK_RIGHT\n");
							P1_KeyVal	|=NES_VK_RIGHT;
							break;
							////////
							
					case	VK_ESCAPE:
					case	VK_NUMPAD0:
							//DebugPrintf("KeyDown ->VK_ESCAPE\n");
							P1_KeyVal	|=NES_VK_A;
							break;
							////////
							
					case	VK_RETURN:
					case	VK_NUMPAD1:
							//DebugPrintf("KeyDown ->VK_RETURN\n");
							P1_KeyVal	|=NES_VK_B;
							break;
							////////
							
					case	VK_F2:
							SYS_KeyVal	|=NES_VK_START;

							break;
							////////
							
					case	VK_F3:
							SYS_KeyVal	|=NES_VK_SELECT;

							break;
							////////
							
							
							
					
				}
				break;
				////////
				
		case	WM_KEYUP:
		
				switch(wParam)
				{
					case	VK_UP:
							//DebugPrintf("KeyUp ->VK_UP\n");
							P1_KeyVal	&= ~NES_VK_UP;
							break;
							////////
					
					case	VK_DOWN:
							//DebugPrintf("KeyUp ->VK_DOWN\n");
							P1_KeyVal	&= ~NES_VK_DOWN;
							break;
							////////
							
					case	VK_LEFT:
							//DebugPrintf("KeyUp ->VK_LEFT\n");
							P1_KeyVal	&= ~NES_VK_LEFT;
							break;
							////////
							
					case	VK_RIGHT:
							//DebugPrintf("KeyUp ->VK_RIGHT\n");
							P1_KeyVal	&= ~NES_VK_RIGHT;
							break;
							////////
							
					case	VK_ESCAPE:
					case	VK_NUMPAD0:
							//DebugPrintf("KeyUp ->VK_ESCAPE\n");
							P1_KeyVal	&= ~NES_VK_A;
							break;
							////////
							
					case	VK_RETURN:
					case	VK_NUMPAD1:
							//DebugPrintf("KeyUp ->VK_RETURN\n");
							P1_KeyVal	&= ~NES_VK_B;
							break;
							////////
							
					case	VK_F2:
							SYS_KeyVal	&= ~NES_VK_START;
							break;
							////////
							
					case	VK_F3:
							SYS_KeyVal	&= ~NES_VK_SELECT;
							break;
							////////
							
					case	VK_F4:
						/*
							if(FullScreen==FALSE)
							{
								ShowWindow(GetDesktopWindow(),SW_HIDE);
								FullScreen=TRUE;	
							}
							else
							{
								ShowWindow(GetDesktopWindow(),SW_SHOW);
								ShowWindow(hwnd,SW_SHOW);
								FullScreen=FALSE;

							}
						*/
							break;
							////////
							
					
				}
				break;
				////////
				
#endif
        case	WM_NOTIFY:
				{
					U16	code,id;
					NMHDR *nm;
					
					nm	=(NMHDR*)lParam;
					if(nm==NULL)
					{
						break;
					}
					
					code =nm->code;//HIWORD(wParam);
					id	 =nm->idFrom;//LOWORD(wParam);

//					if(id==ID_START)
//					{
//						if(code==BN_PUSHED)
//						{
//							SYS_KeyVal	|=NES_VK_START;
//						}

//						if(code==BN_CLICKED)
//						{
//							SYS_KeyVal	&=~NES_VK_START;
//						}
//					}		

					if(id==ID_FULLSCREEN && code==BN_CLICKED)
					{
						HWND wnd;
						////进入全屏

						ShowCursor(FALSE);

						GetWindowRect(hwnd,&def_win_rc);
						def_win_style =GetWindowLong(hwnd,GWL_STYLE);
						SetWindowLong(hwnd,GWL_STYLE,def_win_style&(~(WS_CAPTION|WS_DLGFRAME|WS_BORDER)));

						SetWindowPos(hwnd,NULL,0,0,GUI_XSIZE,GUI_YSIZE,0);

						//隐藏所有子窗口
						wnd =GetWindow(hwnd,GW_CHILD);
						while(wnd)
						{
							ShowWindow(wnd,SW_HIDE);
							wnd =GetWindow(wnd,GW_HWNDNEXT);
						}

						FullScreen=TRUE;
					}

				}
				break;
				
				////////
		case	WM_LBUTTONDOWN:
    {
//      nes_cmd = NES_NULL;
//      ShowWindow(hwnd_List,SW_SHOW);
//      RedrawWindow(hwnd_List, NULL, RDW_ALLCHILDREN|RDW_INVALIDATE|RDW_ERASE);
      if(1)
      {
        WNDCLASS wcex;   
        wcex.Tag	 		= WNDCLASS_TAG;
        wcex.Style			= CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc	= (WNDPROC)Dlg_Load_WinProc;
        wcex.cbClsExtra		= 0;
        wcex.cbWndExtra		= 0;
        wcex.hInstance		= NULL;
        wcex.hIcon			= NULL;
        wcex.hCursor		= NULL;
        if(1)
        {

          

          CreateWindowEx(NULL,
                                &wcex,L"GameList",
                                WS_OVERLAPPED|WS_VISIBLE,
                                0,0,800,480,
                                hwnd,0,NULL,NULL);

        }          
        
      
      }      
      break;
    }
				/////
		case	WM_ERASEBKGND:
		
				//if(FullScreen == FALSE)
				{
          GUI_DEBUG("1");
					hdc	=(HDC)wParam;//GetDC(hwnd);
					GetClientRect(hwnd,&rc);
					
//	 				BitBlt(hdc,0,0,256,240,hdc_NES,0,0,SRCCOPY);
//	 				
//	 				rc.y +=240;
//	 				rc.h -=240;
          rc.x = 0;
          rc.y = 0;
          rc.w = 800;
          rc.h = 480;
	 				ClrDisplay(hdc,&rc,MapRGB(hdc,0,0,0));
 				}
				return	TRUE;
				////
		case	WM_PAINT:
		{
				HDC hdc;
				PAINTSTRUCT ps;
				hdc =BeginPaint(hwnd,&ps);

				if(DrawFrameFlag == TRUE )
				{
					static	int t0=0;
					static	int t1=0;
					static	int frame=0;

					int time;

					if(frame==0)
					{
						t0=GUI_GetTickCount();
					}

					t1=GUI_GetTickCount();
					time	=t1-t0;
					if(time>=1000)
					{
						screen_fps = (frame*1000)/time;
						frame = 0;
					}
					else
					{
						frame++;
					}
					////////
					draw_frame(hdc);

					DrawFrameFlag=FALSE;
				}

				EndPaint(hwnd,&ps);

		}
		break;
		///
		
		case	WM_CLOSE:
    SYS_KeyVal |= PAD_SYS_QUIT;
		NES->PAD_System = PAD_SYS_QUIT;
		nes_cmd		= NES_QUIT;     //设置nes_cmd 标志,让NES线程结束.
		MenuRet		=-1;
    usb_HostApplicationMouse_run = FALSE;
    usb_HostTask_run = FALSE;
		if(nes_thread_run==FALSE&& usb_HostApplicationMouse_state == FALSE && usb_HostTask_state == FALSE)
		{ 
      file_nums = 0;
      cur_index = 0;
      exit_type = e_Post_OK;
      GUI_SemDelete(sai_complete_sem);
			
      //PostCloseMessage(hwnd_List);
      DeleteDC(hdc_NES);
			DestroyWindow(hwnd); 
		}
		break;
		/////

		default:
		return	DefWindowProc(hwnd,msg,wParam,lParam);
	}
		
	return WM_NULL;
}



/*============================================================================*/

//argv:文件的根目录
extern "C" int	InfoNES_WinMain(HANDLE hInstance,void *argv)
{
	HWND hwnd;
	MSG msg;
	WNDCLASS wcex;
	RECT rc;

	hInst	=hInstance;
//	SYS_thread_set_priority(NULL,-5);
//	DebugPuts("InfoNES\r\n");
//  USB_HostApplicationInit();
//	if(argv!=NULL)
//	{
//		memcpy(rom_dir,argv,sizeof(rom_dir));
//	}
//	else
//	{
//		//SYS_get_process_data(PROC_CUR_DIR,cur_dir,sizeof(cur_dir));
//		x_strcpy(rom_dir,"B:");
//	}
	////

  Neshd =(NesHeader*)vmalloc(sizeof(NesHeader));
  memset(Neshd,0,sizeof(NesHeader));
  NES   =(NesResource*)vmalloc(sizeof(NesResource));
  memset(NES,0,sizeof(NesResource));
  CPU   =(CpuResource*)kmalloc(sizeof(CpuResource));
  memset(CPU,0,sizeof(CpuResource));
  RAM   =(BYTE*)kmalloc(RAM_SIZE);
  memset(RAM,0,RAM_SIZE);
  SRAM  =(BYTE*)kmalloc(SRAM_SIZE);
  memset(SRAM,0,SRAM_SIZE);
  PPURAM =(BYTE*)vmalloc(PPURAM_SIZE);
  memset(PPURAM,0,PPURAM_SIZE);
  ChrBuf =(BYTE*)vmalloc(256*2*8*8);
  memset(ChrBuf,0,256*2*8*8);
  APU =(ApuResource*)vmalloc(sizeof(ApuResource));
  memset(APU,0,sizeof(ApuResource));
  ApuEventQueue =(ApuEvent*)vmalloc(APU_EVENT_MAX*sizeof(ApuEvent));
  memset(ApuEventQueue,0,APU_EVENT_MAX*sizeof(ApuEvent));
  
//  Abuf1=(WORD*)GUI_GRAM_Alloc(734*sizeof(WORD));
//  
//  Abuf2=(WORD*)GUI_GRAM_Alloc(734*sizeof(WORD));  

  
//  wave_buffers =(WORD*)GUI_VMEM_Alloc(1470);
  WorkFrame =(WORD*)GUI_GRAM_Alloc(256*240*2);

	hInst	=hInstance;
	

	wcex.Tag		 	= WNDCLASS_TAG;

	wcex.Style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WinProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= NULL;//LoadIcon(hInstance, (LPCTSTR)IDI_WIN32_APP_TEST);
	wcex.hCursor		= NULL;//LoadCursor(NULL, IDC_ARROW);
	
	//SetRectArea(&rc,20,0,256,240+24);
	
	AdjustWindowRect(&rc,WS_OVERLAPPEDWINDOW);
	
	hwnd = CreateWindow(	&wcex,__Name,WS_OVERLAPPEDWINDOW|WS_CLIPCHILDREN|WS_EX_FRAMEBUFFER,
							0,0,800,480,
							NULL,0,hInst,NULL);
							
	ShowWindow(hwnd,SW_SHOW);
	while(GetMessage(&msg,hwnd))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}


   	vfree(Neshd);
   	vfree(NES);
    kfree(CPU);
   	kfree(RAM);
   	kfree(SRAM);
   	vfree(PPURAM);
    vfree(ChrBuf);
   	vfree((APU));
  	vfree(ApuEventQueue);

//  	GUI_GRAM_Free(Abuf1);
//    GUI_GRAM_Free(Abuf2);
    SAI_TransferAbortSendEDMA(SAI1, &txHandle);
    SAI_Deinit(SAI1);  
   	vfree(WorkFrame);

	return TRUE;
}

void	GUI_NES_DIALOG(void *param)
{

  WNDCLASS wcex;  
  MSG msg;  
  wcex.Tag	 		= WNDCLASS_TAG;
  wcex.Style			= CS_HREDRAW | CS_VREDRAW;
  wcex.lpfnWndProc	= (WNDPROC)Dlg_Load_WinProc;
  wcex.cbClsExtra		= 0;
  wcex.cbWndExtra		= 0;
  wcex.hInstance		= NULL;
  wcex.hIcon			= NULL;
  wcex.hCursor		= NULL;
  if(1)
  {
    
    f_readdir_gui("nes",&dir_object,&file_info);
    hwnd_List = CreateWindowEx(WS_EX_NOFOCUS,
                          &wcex,L"GameList",
                          WS_OVERLAPPED|WS_VISIBLE,
                          0,0,800,480,
                          NULL,0,NULL,NULL);

  }          
	ShowWindow(hwnd_List,SW_SHOW);
	while(GetMessage(&msg,hwnd_List))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}  
}

/*============================================================================*/

class	NES_SIM{


	int LoadSRAM(void);

private:

};
/*============================================================================*/

#if	1
static int run=FALSE;

static void list_thread(void *p)
{
	//InfoNES_WinMain(NULL,NULL);
  GUI_NES_DIALOG(NULL);
	
	GUI_Thread_Delete(xTaskGetHandle("LIST_WIN"));
}

static void win_thread(void *p)
{
	InfoNES_WinMain(NULL,NULL);
 
	run =FALSE;
	GUI_Thread_Delete(xTaskGetHandle("NES_WIN"));
}

extern "C" void NES_Simulator(void* param)
{
	run =TRUE;
  
//	SYS_thread_create(win_thread,NULL,10*1024,NULL,0);
  GUI_Thread_Create(win_thread, "NES_WIN", 12*1024, NULL, 6, 5);
  GUI_Thread_Create(list_thread, "LIST_WIN", 12*1024, NULL, 6, 5);
//	sms_load(NULL);
	while(run==TRUE)
	{
		SYS_msleep(100);
	}
	SYS_msleep(100);
  
}
#endif

#ifdef	APP_OUTPUT
//const void *pWinMainPtr =(void*)__main;;
const void *pWinMainPtr =(void*)win_thread;;
#endif

/*============================================================================*/

