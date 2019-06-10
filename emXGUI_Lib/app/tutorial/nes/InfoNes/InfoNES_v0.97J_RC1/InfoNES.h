/*===================================================================*/
/*                                                                   */
/*  InfoNES.h : NES Emulator for Win32, Linux(x86), Linux(PS2)       */
/*                                                                   */
/*  2000/05/14  InfoNES Project ( based on pNesX )                   */
/*                                                                   */
/*===================================================================*/

#ifndef InfoNES_H_INCLUDED
#define InfoNES_H_INCLUDED

/*-------------------------------------------------------------------*/
/*  Include files                                                    */
/*-------------------------------------------------------------------*/

#include "InfoNES_Types.h"

/*-------------------------------------------------------------------*/
/*  NES resources                                                    */
/*-------------------------------------------------------------------*/

#define RAM_SIZE     0x2000
#define SRAM_SIZE    0x2000
#define PPURAM_SIZE  0x4000
#define SPRRAM_SIZE  256




typedef struct
{
    NDWORD PAD1_Latch;
    NDWORD PAD2_Latch;
    NDWORD PAD_System;
    NDWORD PAD1_Bit;
    NDWORD PAD2_Bit; 
    NBYTE *PPU_BG_Base;//BG Base Address
    NBYTE *PPU_SP_Base;//Sprite Base Address    
    int  SpriteJustHit;//Sprite #0 Scanline Hit Position
    int  APU_Mute;//APU Mute(0:OFF,1:ON)     
    NWORD PPU_Addr;//PPU Address
    NWORD PPU_Temp;//PPU Address
    NWORD PPU_Increment;//The increase value of the PPU Address
    NWORD PPU_Scanline;//Current Scanline 
    NWORD PPU_SP_Height;//Sprite Height
    NWORD FrameStep;
    NWORD LcdRes;  //LcdResolution	
    NWORD PalTable[32];//Palette Table 
    NWORD *Linebuf[2];//Includding padding bytes 
	  NBYTE LineCount;
    NBYTE PPU_NameTableBank;//Name Table Bank    
    NBYTE byVramWriteEnable;//VRAM Write Enable ( 0: Disable, 1: Enable )
    NBYTE PPU_Latch_Flag;//PPU Address and Scroll Latch Flag
    NBYTE PPU_UpDown_Clip;//Up and Down Clipping Flag (0:non-clip,1:clip)
    NBYTE FrameIRQ_Enable;//Frame IRQ (0:Disabled,1:Enabled)  
    NBYTE ChrBufUpdate;//Update flag for ChrBuf                                       
    NBYTE MapperNo;//Mapper Number
    NBYTE ROM_Mirroring;//Mirroring 0:Horizontal 1:Vertical
    NBYTE ROM_SRAM;//It has SRAM
    NBYTE ROM_Trainer;//It has Trainer
    NBYTE ROM_FourScr;//Four screen VRAM 
    NBYTE PPU_R0;
    NBYTE PPU_R1;
    NBYTE PPU_R2;
    NBYTE PPU_R3;
    NBYTE PPU_R7;
    NBYTE PPU_Scr_V;
    NBYTE PPU_Scr_V_Next;
    NBYTE PPU_Scr_V_Byte;
    NBYTE PPU_Scr_V_Byte_Next;
    NBYTE PPU_Scr_V_Bit;
    NBYTE PPU_Scr_V_Bit_Next; 
    NBYTE PPU_Scr_H;
    NBYTE PPU_Scr_H_Next;
    NBYTE PPU_Scr_H_Byte;
    NBYTE PPU_Scr_H_Byte_Next;
    NBYTE PPU_Scr_H_Bit;
    NBYTE PPU_Scr_H_Bit_Next;
    NBYTE FrameCnt;      
    NBYTE APU_Reg[ 0x18 ];//APU Register    
    NBYTE PPU_ScanTable[ 264 ];//Scanline Table
    NBYTE SPRRAM[SPRRAM_SIZE];   
}NesResource;


/**************文件头说明****************/
/*
  0－3  4   字符串“NES^Z”用来识别.NES文件  
     4  1   16kB ROM的数目  
     5  1   8kB VROM的数目  
     6  1   D0：1＝垂直镜像，0＝水平镜像  
            D1：1＝有电池记忆，SRAM地址$6000-$7FFF  
            D2：1＝在$7000-$71FF有一个512字节的trainer  
            D3：1＝屏幕VRAM布局  
            D4－D7：ROM Mapper的低4位  
     7  1   D0－D3：保留，必须是0（准备作为副Mapper号^_^）  
            D4－D7：ROM Mapper的高4位  
  8－F  8   保留，必须是0
以下为文件内容：y的值取决于文件头结构体的byRomSize。PROM的大小是16K，VROM是8K
  16-y      ROM段升序排列，如果存在trainer，它的512字节摆在ROM段之前
   y-EOF    VROM段, 升序排列 
*/
#pragma pack(1)
typedef struct 
{
  NBYTE byID[ 4 ];
  NBYTE byRomSize;
  NBYTE byVRomSize;
  NBYTE byInfo1;
  NBYTE byInfo2;
  NBYTE byReserve[ 8 ];
}NesHeader;
#pragma pack()

extern NesResource *NES;
/* RAM */
extern NBYTE *RAM;
/* SRAM */
extern NBYTE *SRAM;
/* ROM */
extern NBYTE *ROM;
/* SRAM BANK ( 8Kb ) */
extern NBYTE *SRAMBANK;
/* ROM BANK ( 8Kb * 4 ) */
extern NBYTE *ROMBANK0;
extern NBYTE *ROMBANK1;
extern NBYTE *ROMBANK2;
extern NBYTE *ROMBANK3;
/*-------------------------------------------------------------------*/
/*  PPU resources                                                    */
/*-------------------------------------------------------------------*/
/* PPU RAM */
extern NBYTE *PPURAM;
/* VROM */
extern NBYTE *VROM;
/* PPU BANK ( 1Kb * 16 ) */
extern NBYTE *PPUBANK[];
/* Sprite RAM */
extern NBYTE SPRRAM[];
/* PPU Register */
extern NBYTE PPU_R0;
extern NBYTE PPU_R1;
extern NBYTE PPU_R2;
extern NBYTE PPU_R3;
extern NBYTE PPU_R7;
extern NBYTE PPU_Scr_V;
extern NBYTE PPU_Scr_V_Next;
extern NBYTE PPU_Scr_V_Byte;
extern NBYTE PPU_Scr_V_Byte_Next;
extern NBYTE PPU_Scr_V_Bit;
extern NBYTE PPU_Scr_V_Bit_Next;
extern NBYTE PPU_Scr_H;
extern NBYTE PPU_Scr_H_Next;
extern NBYTE PPU_Scr_H_Byte;
extern NBYTE PPU_Scr_H_Byte_Next;
extern NBYTE PPU_Scr_H_Bit;
extern NBYTE PPU_Scr_H_Bit_Next;
extern NBYTE PPU_Latch_Flag;
extern NWORD PPU_Addr;
extern NWORD PPU_Temp;
extern NWORD PPU_Increment;
extern NBYTE PPU_Latch_Flag;
extern NBYTE PPU_UpDown_Clip;
/* Current Scanline */
extern NWORD PPU_Scanline;
/* Scanline Table */
extern NBYTE PPU_ScanTable[];
/* Name Table Bank */
extern NBYTE PPU_NameTableBank;
/* BG Base Address */
extern NBYTE *PPU_BG_Base;
/* Sprite Base Address */
extern NBYTE *PPU_SP_Base;
/* Sprite Height */
extern NWORD PPU_SP_Height;
/* VRAM Write Enable ( 0: Disable, 1: Enable ) */
extern NBYTE byVramWriteEnable;
/* Frame IRQ ( 0: Disabled, 1: Enabled )*/
extern NBYTE FrameIRQ_Enable;
extern NWORD FrameStep;
/*-------------------------------------------------------------------*/
/*  Display and Others resouces                                      */
/*-------------------------------------------------------------------*/
/* Frame Skip */
extern NWORD FrameSkip;
extern NWORD FrameCnt;
extern NWORD FrameWait;
extern NBYTE *ChrBuf;
extern NBYTE ChrBufUpdate;
extern NWORD PalTable[];
/*-------------------------------------------------------------------*/
/*  APU and Pad resources                                            */
/*-------------------------------------------------------------------*/
extern NBYTE APU_Reg[];
extern int APU_Mute;
extern NDWORD PAD1_Latch;
extern NDWORD PAD2_Latch;
extern NDWORD PAD_System;
extern NDWORD PAD1_Bit;
extern NDWORD PAD2_Bit;
/* .nes File Header */
extern NesHeader *Neshd;
/* Mapper No. */
extern NBYTE MapperNo;
/* Other */
extern NBYTE ROM_Mirroring;
extern NBYTE ROM_SRAM;
extern NBYTE ROM_Trainer;
extern NBYTE ROM_FourScr;

#define NAME_TABLE0  8
#define NAME_TABLE1  9
#define NAME_TABLE2  10
#define NAME_TABLE3  11
#define NAME_TABLE_V_MASK 2
#define NAME_TABLE_H_MASK 1
#define SPR_Y    0
#define SPR_CHR  1
#define SPR_ATTR 2
#define SPR_X    3
#define SPR_ATTR_COLOR  0x3
#define SPR_ATTR_V_FLIP 0x80
#define SPR_ATTR_H_FLIP 0x40
#define SPR_ATTR_PRI    0x20
#define R0_NMI_VB      0x80
#define R0_NMI_SP      0x40
#define R0_SP_SIZE     0x20
#define R0_BG_ADDR     0x10
#define R0_SP_ADDR     0x08
#define R0_INC_ADDR    0x04
#define R0_NAME_ADDR   0x03
#define R1_BACKCOLOR   0xe0
#define R1_SHOW_SP     0x10
#define R1_SHOW_SCR    0x08
#define R1_CLIP_SP     0x04
#define R1_CLIP_BG     0x02
#define R1_MONOCHROME  0x01
#define R2_IN_VBLANK   0x80
#define R2_HIT_SP      0x40
#define R2_MAX_SP      0x20
#define R2_WRITE_FLAG  0x10
#define SCAN_TOP_OFF_SCREEN     0
#define SCAN_ON_SCREEN          1
#define SCAN_BOTTOM_OFF_SCREEN  2
#define SCAN_UNKNOWN            3
#define SCAN_VBLANK             4
#define SCAN_TOP_OFF_SCREEN_START       0 
#define SCAN_ON_SCREEN_START            8
#define SCAN_BOTTOM_OFF_SCREEN_START  232
#define SCAN_UNKNOWN_START            240
#define SCAN_VBLANK_START             243
#define SCAN_VBLANK_END               262
#define STEP_PER_SCANLINE             113
#define STEP_PER_FRAME                29828
/* NES display size */
#define NES_DISP_WIDTH      256
#define NES_DISP_HEIGHT     224

#define RESOLUTION256_224 256
#define RESOLUTION320_224 320

/* Develop Scroll Registers */
#define InfoNES_SetupScr() \
{ \
  /* V-Scroll Register */ \
	 NES->PPU_Scr_V_Next = ( NBYTE )( NES->PPU_Addr & 0x001f ); \
   NES->PPU_Scr_V_Byte_Next = NES->PPU_Scr_V_Next >> 3; \
  /*PPU_Scr_V_Bit_Next = PPU_Scr_V_Next & 0x07;*/ \
  /* H-Scroll Register */ \
  /* PPU_Scr_H_Byte_Next = ( NBYTE )( PPU_Addr & 0x001f ); */ \
  /* NameTableBank */ \
  NES->PPU_NameTableBank = NAME_TABLE0 + ( ( NES->PPU_Addr & 0x0C00 ) >> 10 ); \
}

#define PAD_SYS_QUIT   1
#define PAD_SYS_OK     2
#define PAD_SYS_CANCEL 4
#define PAD_SYS_UP     8
#define PAD_SYS_DOWN   0x10
#define PAD_SYS_LEFT   0x20
#define PAD_SYS_RIGHT  0x40
#define PAD_PUSH(a,b)  ( ( (a) & (b) ) != 0 )

/*-------------------------------------------------------------------*/
/*  Mapper Function                                                  */
/*-------------------------------------------------------------------*/

/* Initialize Mapper */
extern void (*MapperInit)();
/* Write to Mapper */
extern void (*MapperWrite)( NWORD wAddr, NBYTE byData );
/* Write to SRAM */
extern void (*MapperSram)( NWORD wAddr, NBYTE byData );
/* Write to APU */
extern void (*MapperApu)( NWORD wAddr, NBYTE byData );
/* Read from Apu */
extern NBYTE (*MapperReadApu)( NWORD wAddr );
/* Callback at VSync */
extern void (*MapperVSync)();
/* Callback at HSync */
extern void (*MapperHSync)();
/* Callback at PPU read/write */
extern void (*MapperPPU)( NWORD wAddr );
/* Callback at Rendering Screen 1:BG, 0:Sprite */
extern void (*MapperRenderScreen)( NBYTE byMode );

/*-------------------------------------------------------------------*/
/*  ROM information                                                  */
/*-------------------------------------------------------------------*/


/*-------------------------------------------------------------------*/
/*  Function prototypes                                              */
/*-------------------------------------------------------------------*/
/* Initialize InfoNES */
void InfoNES_Init(void);
/* Completion treatment */
void InfoNES_Fin(void);
/* Load a cassette */
int InfoNES_Load( char *pszFileName );
/* Reset InfoNES */
int InfoNES_Reset(void);
/* Initialize PPU */
void InfoNES_SetupPPU(void);
/* Set up a Mirroring of Name Table */
void InfoNES_Mirroring( int nType );
/* The main loop of InfoNES */ 
void InfoNES_Main(void);
/* The loop of emulation */
void InfoNES_Cycle(void);
/* A function in H-Sync */
int InfoNES_HSync(void);
/* Render a scanline */
void InfoNES_DrawLine(void);
/* Get a position of scanline hits sprite #0 */
void InfoNES_GetSprHitY(void);
/* Develop character data */
void InfoNES_SetupChr(void);


#endif /* !InfoNES_H_INCLUDED */
