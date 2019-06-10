/*===================================================================*/
/*                                                                   */
/*  InfoNES_Mapper.h : InfoNES Mapper Function                       */
/*                                                                   */
/*  2000/05/16  InfoNES Project ( based on NesterJ and pNesX )       */
/*                                                                   */
/*===================================================================*/

#ifndef InfoNES_MAPPER_H_INCLUDED
#define InfoNES_MAPPER_H_INCLUDED

#include "InfoNES_Types.h"

/*-------------------------------------------------------------------*/
/*  Macros                                                           */
/*-------------------------------------------------------------------*/

#define DRAM_SIZE    0x1000
/* The address of 8Kbytes unit of the ROM */
#define ROMPAGE(a)     &ROM[ (a) * 0x2000 ]
/* From behind the ROM, the address of 8kbytes unit */
#define ROMLASTPAGE(a) &ROM[ Neshd->byRomSize * 0x4000 - ( (a) + 1 ) * 0x2000 ]
/* The address of 1Kbytes unit of the VROM */
#define VROMPAGE(a)    &VROM[ (a) * 0x400 ]
/* The address of 1Kbytes unit of the CRAM */
#define CRAMPAGE(a)   &PPURAM[ 0x0000 + ((a)&0x1F) * 0x400 ]
/* The address of 1Kbytes unit of the VRAM */
#define VRAMPAGE(a)    &PPURAM[ 0x2000 + (a) * 0x400 ]
/* Translate the pointer to ChrBuf into the address of Pattern Table */ 
#define PATTBL(a)      ( ( (a) - ChrBuf ) >> 2 )
/* The address of 8Kbytes unit of the Map5 ROM */
#define Map5_ROMPAGE(a)     &Map5_Wram[ ( (a) & 0x07 ) * 0x2000 ]
/* The address of 1Kbytes unit of the Map6 Chr RAM */
#define Map6_VROMPAGE(a)    &MAP6->Map6_Chr_Ram[ (a) * 0x400 ]
/* The address of 1Kbytes unit of the Map19 Chr RAM */
#define Map19_VROMPAGE(a)   &Map19_Chr_Ram[ (a) * 0x400 ]
/* The address of 1Kbytes unit of the Map85 Chr RAM */
#define Map85_VROMPAGE(a)   &Map85_Chr_Ram[ (a) * 0x400 ]

/*-------------------------------------------------------------------*/
/*  Table of Mapper initialize function                              */
/*-------------------------------------------------------------------*/

typedef struct 
{
  int nMapperNo;
  void (*pMapperInit)(void);
}MapperTable;

typedef enum
{
  Map1_SMALL,
  Map1_512K,
  Map1_1024K
}Map1_Size_t;
//#pragma pack(1)
typedef struct
{
    NDWORD Map1_256K_base;
    NDWORD Map1_swap;
    NDWORD Map1_bank1;
    NDWORD Map1_bank2;
    NDWORD Map1_bank3;
    NDWORD Map1_bank4;
    NDWORD Map1_HI1;
    NDWORD Map1_HI2; 
    NDWORD Map1_Cnt;
    Map1_Size_t Map1_Size; 
    NWORD  Map1_Last_Write_Addr;   
    NBYTE  Map1_Latch;  
    NBYTE  Map1_Regs[ 4 ]; 
}Mapper1Res;

typedef struct
{   
    NDWORD Map4_Rom_Bank;
    NDWORD Map4_Prg0;
    NDWORD Map4_Prg1;
    NDWORD Map4_Chr01;
    NDWORD Map4_Chr23;
    NDWORD Map4_Chr4;
    NDWORD Map4_Chr5;
    NDWORD Map4_Chr6;
    NDWORD Map4_Chr7;    
    NBYTE Map4_IRQ_Enable;
    NBYTE Map4_IRQ_Cnt;
    NBYTE Map4_IRQ_Latch;
    NBYTE Map4_IRQ_Request;
    NBYTE Map4_IRQ_Present;
    NBYTE Map4_IRQ_Present_Vbl;
    NBYTE  Map4_Regs[ 8 ];
}Mapper4Res;

typedef struct
{
    NDWORD Map6_IRQ_Cnt;
    NBYTE Map6_IRQ_Enable;   
    NBYTE Map6_Chr_Ram[0x2000*4 ];
}Mapper6Res;

typedef struct
{  
    NBYTE Map21_IRQ_Enable;
    NBYTE Map21_IRQ_Cnt;
    NBYTE Map21_IRQ_Latch;
    NBYTE Map21_Regs[ 10 ];
}Mapper21Res;

typedef struct
{   
    NBYTE Map23_IRQ_Enable;
    NBYTE Map23_IRQ_Cnt;
    NBYTE Map23_IRQ_Latch;
    NBYTE Map23_Regs[ 9 ];
}Mapper23Res;

typedef struct
{
    NBYTE Map33_Switch;
    NBYTE Map33_IRQ_Enable;
    NBYTE Map33_IRQ_Cnt;
    NBYTE Map33_Regs[ 8 ];    
}Mapper33Res;

typedef struct
{
    NDWORD Map118_Prg0;
    NDWORD Map118_Prg1;
    NDWORD Map118_Chr0; 
    NDWORD Map118_Chr1; 
    NDWORD Map118_Chr2; 
    NDWORD Map118_Chr3;
    NDWORD Map118_Chr4; 
    NDWORD Map118_Chr5; 
    NDWORD Map118_Chr6; 
    NDWORD Map118_Chr7;
    NBYTE Map118_IRQ_Enable;
    NBYTE Map118_IRQ_Cnt;
    NBYTE Map118_IRQ_Latch;
    NBYTE  Map118_Regs[ 8 ];    
}Mapper118Res;

typedef struct
{
    NDWORD Map69_IRQ_Cnt; 
    NBYTE Map32_Saved;
    NBYTE Map64_Cmd;
    NBYTE Map64_Prg;
    NBYTE Map64_Chr;
    NBYTE  Map69_IRQ_Enable;
    NBYTE  Map69_Regs[ 1 ];    
}MapperMISRes;

extern const MapperTable MapTab[];
extern Mapper1Res *MAP1;
extern Mapper4Res *MAP4;
extern Mapper6Res *MAP6;
extern Mapper21Res *MAP21;
extern Mapper23Res *MAP23;
extern Mapper33Res *MAP33;
extern Mapper118Res *MAP118;
extern MapperMISRes *MAPMIS;
/*-------------------------------------------------------------------*/
/*  Function prototypes                                              */
/*-------------------------------------------------------------------*/

void Map0_Init(void);
void Map0_Write( NWORD wAddr, NBYTE byData );
void Map0_Sram( NWORD wAddr, NBYTE byData );
void Map0_Apu( NWORD wAddr, NBYTE byData );
NBYTE Map0_ReadApu( NWORD wAddr );
void Map0_VSync(void);
void Map0_HSync(void);
void Map0_PPU( NWORD wAddr );
void Map0_RenderScreen( NBYTE byMode );

void Map1_Init(void);
void Map1_Write( NWORD wAddr, NBYTE byData );
void Map1_set_ROM_banks(void);

void Map2_Init(void);
void Map2_Write( NWORD wAddr, NBYTE byData );

void Map3_Init(void);
void Map3_Write( NWORD wAddr, NBYTE byData );

void Map4_Init(void);
void Map4_Write( NWORD wAddr, NBYTE byData );
void Map4_HSync(void);
void Map4_Set_CPU_Banks(void);
void Map4_Set_PPU_Banks(void);

void Map5_Init(void);
void Map5_Write( NWORD wAddr, NBYTE byData );
void Map5_Apu( NWORD wAddr, NBYTE byData );
NBYTE Map5_ReadApu( NWORD wAddr );
void Map5_HSync(void);
void Map5_RenderScreen( NBYTE byMode );
void Map5_Sync_Prg_Banks( void );

void Map6_Init(void);
void Map6_Write( NWORD wAddr, NBYTE byData );
void Map6_Apu( NWORD wAddr, NBYTE byData );
void Map6_HSync(void);

void Map7_Init(void);
void Map7_Write( NWORD wAddr, NBYTE byData );

void Map8_Init(void);
void Map8_Write( NWORD wAddr, NBYTE byData );

void Map9_Init(void);
void Map9_Write( NWORD wAddr, NBYTE byData );
void Map9_PPU( NWORD wAddr );

void Map10_Init(void);
void Map10_Write( NWORD wAddr, NBYTE byData );
void Map10_PPU( NWORD wAddr );

void Map11_Init(void);
void Map11_Write( NWORD wAddr, NBYTE byData );

void Map13_Init(void);
void Map13_Write( NWORD wAddr, NBYTE byData );

void Map15_Init(void);
void Map15_Write( NWORD wAddr, NBYTE byData );

void Map16_Init(void);
void Map16_Write( NWORD wAddr, NBYTE byData );
void Map16_HSync(void);

void Map17_Init(void);
void Map17_Apu( NWORD wAddr, NBYTE byData );
void Map17_HSync(void);

void Map18_Init(void);
void Map18_Write( NWORD wAddr, NBYTE byData );
void Map18_HSync(void);

void Map19_Init(void);
void Map19_Write( NWORD wAddr, NBYTE byData );
void Map19_Apu( NWORD wAddr, NBYTE byData );
NBYTE Map19_ReadApu( NWORD wAddr );
void Map19_HSync(void);

void Map21_Init(void);
void Map21_Write( NWORD wAddr, NBYTE byData );
void Map21_HSync(void);

void Map22_Init(void);
void Map22_Write( NWORD wAddr, NBYTE byData );

void Map23_Init(void);
void Map23_Write( NWORD wAddr, NBYTE byData );
void Map23_HSync(void);

void Map24_Init(void);
void Map24_Write( NWORD wAddr, NBYTE byData );
void Map24_HSync(void);

void Map25_Init(void);
void Map25_Write( NWORD wAddr, NBYTE byData );
void Map25_Sync_Vrom( int nBank );
void Map25_HSync(void);

void Map26_Init(void);
void Map26_Write( NWORD wAddr, NBYTE byData );
void Map26_HSync(void);

void Map32_Init(void);
void Map32_Write( NWORD wAddr, NBYTE byData );

void Map33_Init(void);
void Map33_Write( NWORD wAddr, NBYTE byData );
void Map33_HSync(void);

void Map34_Init(void);
void Map34_Write( NWORD wAddr, NBYTE byData );
void Map34_Sram( NWORD wAddr, NBYTE byData );

void Map40_Init(void);
void Map40_Write( NWORD wAddr, NBYTE byData );
void Map40_HSync(void);

void Map41_Init(void);
void Map41_Write( NWORD wAddr, NBYTE byData );
void Map41_Sram( NWORD wAddr, NBYTE byData );

void Map42_Init(void);
void Map42_Write( NWORD wAddr, NBYTE byData );
void Map42_HSync(void);

void Map43_Init(void);
void Map43_Write( NWORD wAddr, NBYTE byData );
void Map43_Apu( NWORD wAddr, NBYTE byData );
NBYTE Map43_ReadApu( NWORD wAddr );
void Map43_HSync(void);

void Map44_Init(void);
void Map44_Write( NWORD wAddr, NBYTE byData );
void Map44_HSync(void);
void Map44_Set_CPU_Banks(void);
void Map44_Set_PPU_Banks(void);

void Map45_Init(void);
void Map45_Sram( NWORD wAddr, NBYTE byData );
void Map45_Write( NWORD wAddr, NBYTE byData );
void Map45_HSync(void);
void Map45_Set_CPU_Bank4( NBYTE byData );
void Map45_Set_CPU_Bank5( NBYTE byData );
void Map45_Set_CPU_Bank6( NBYTE byData );
void Map45_Set_CPU_Bank7( NBYTE byData );
void Map45_Set_PPU_Banks(void);

void Map46_Init(void);
void Map46_Sram( NWORD wAddr, NBYTE byData );
void Map46_Write( NWORD wAddr, NBYTE byData );
void Map46_Set_ROM_Banks(void);

void Map47_Init(void);
void Map47_Sram( NWORD wAddr, NBYTE byData );
void Map47_Write( NWORD wAddr, NBYTE byData );
void Map47_HSync(void);
void Map47_Set_CPU_Banks(void);
void Map47_Set_PPU_Banks(void);

void Map48_Init(void);
void Map48_Write( NWORD wAddr, NBYTE byData );
void Map48_HSync(void);

void Map49_Init(void);
void Map49_Sram( NWORD wAddr, NBYTE byData );
void Map49_Write( NWORD wAddr, NBYTE byData );
void Map49_HSync(void);
void Map49_Set_CPU_Banks(void);
void Map49_Set_PPU_Banks(void);

void Map50_Init(void);
void Map50_Apu( NWORD wAddr, NBYTE byData );
void Map50_HSync(void);

void Map51_Init(void);
void Map51_Sram( NWORD wAddr, NBYTE byData );
void Map51_Write( NWORD wAddr, NBYTE byData );
void Map51_Set_CPU_Banks(void);

void Map57_Init(void);
void Map57_Write( NWORD wAddr, NBYTE byData );

void Map58_Init(void);
void Map58_Write( NWORD wAddr, NBYTE byData );

void Map60_Init(void);
void Map60_Write( NWORD wAddr, NBYTE byData );

void Map61_Init(void);
void Map61_Write( NWORD wAddr, NBYTE byData );

void Map62_Init(void);
void Map62_Write( NWORD wAddr, NBYTE byData );

void Map64_Init(void);
void Map64_Write( NWORD wAddr, NBYTE byData );

void Map65_Init(void);
void Map65_Write( NWORD wAddr, NBYTE byData );
void Map65_HSync(void);

void Map66_Init(void);
void Map66_Write( NWORD wAddr, NBYTE byData );

void Map67_Init(void);
void Map67_Write( NWORD wAddr, NBYTE byData );
void Map67_HSync(void);

void Map68_Init(void);
void Map68_Write( NWORD wAddr, NBYTE byData );
void Map68_SyncMirror(void);

void Map69_Init(void);
void Map69_Write( NWORD wAddr, NBYTE byData );
void Map69_HSync(void);

void Map70_Init(void);
void Map70_Write( NWORD wAddr, NBYTE byData );

void Map71_Init(void);
void Map71_Write( NWORD wAddr, NBYTE byData );

void Map72_Init(void);
void Map72_Write( NWORD wAddr, NBYTE byData );

void Map73_Init(void);
void Map73_Write( NWORD wAddr, NBYTE byData );
void Map73_HSync(void);

void Map74_Init(void);
void Map74_Write( NWORD wAddr, NBYTE byData );
void Map74_HSync(void);
void Map74_Set_CPU_Banks(void);
void Map74_Set_PPU_Banks(void);

void Map75_Init(void);
void Map75_Write( NWORD wAddr, NBYTE byData );

void Map76_Init(void);
void Map76_Write( NWORD wAddr, NBYTE byData );

void Map77_Init(void);
void Map77_Write( NWORD wAddr, NBYTE byData );

void Map78_Init(void);
void Map78_Write( NWORD wAddr, NBYTE byData );

void Map79_Init(void);
void Map79_Apu( NWORD wAddr, NBYTE byData );

void Map80_Init(void);
void Map80_Sram( NWORD wAddr, NBYTE byData );

void Map82_Init(void);
void Map82_Sram( NWORD wAddr, NBYTE byData );

void Map83_Init(void);
void Map83_Write( NWORD wAddr, NBYTE byData );
void Map83_Apu( NWORD wAddr, NBYTE byData );
NBYTE Map83_ReadApu( NWORD wAddr );
void Map83_HSync(void);

void Map85_Init(void);
void Map85_Write( NWORD wAddr, NBYTE byData );
void Map85_HSync(void);

void Map86_Init(void);
void Map86_Sram( NWORD wAddr, NBYTE byData );

void Map87_Init(void);
void Map87_Sram( NWORD wAddr, NBYTE byData );

void Map88_Init(void);
void Map88_Write( NWORD wAddr, NBYTE byData );

void Map89_Init(void);
void Map89_Write( NWORD wAddr, NBYTE byData );

void Map90_Init(void);
void Map90_Write( NWORD wAddr, NBYTE byData );
void Map90_Apu( NWORD wAddr, NBYTE byData );
NBYTE Map90_ReadApu( NWORD wAddr );
void Map90_HSync(void);
void Map90_Sync_Mirror( void );
void Map90_Sync_Prg_Banks( void );
void Map90_Sync_Chr_Banks( void );

void Map91_Init(void);
void Map91_Sram( NWORD wAddr, NBYTE byData );

void Map92_Init(void);
void Map92_Write( NWORD wAddr, NBYTE byData );

void Map93_Init(void);
void Map93_Sram( NWORD wAddr, NBYTE byData );

void Map94_Init(void);
void Map94_Write( NWORD wAddr, NBYTE byData );

void Map95_Init(void);
void Map95_Write( NWORD wAddr, NBYTE byData );
void Map95_Set_CPU_Banks(void);
void Map95_Set_PPU_Banks(void);

void Map96_Init(void);
void Map96_Write( NWORD wAddr, NBYTE byData );
void Map96_PPU( NWORD wAddr );
void Map96_Set_Banks(void);

void Map97_Init(void);
void Map97_Write( NWORD wAddr, NBYTE byData );

void Map99_Init(void);
void Map99_Apu( NWORD wAddr, NBYTE byData );
NBYTE Map99_ReadApu( NWORD wAddr );

void Map100_Init(void);
void Map100_Write( NWORD wAddr, NBYTE byData );
void Map100_HSync(void);
void Map100_Set_CPU_Banks(void);
void Map100_Set_PPU_Banks(void);

void Map101_Init(void);
void Map101_Write( NWORD wAddr, NBYTE byData );

void Map105_Init(void);
void Map105_Write( NWORD wAddr, NBYTE byData );
void Map105_HSync(void);

void Map107_Init(void);
void Map107_Write( NWORD wAddr, NBYTE byData );

void Map108_Init(void);
void Map108_Write( NWORD wAddr, NBYTE byData );

void Map109_Init(void);
void Map109_Apu( NWORD wAddr, NBYTE byData );
void Map109_Set_PPU_Banks(void);

void Map110_Init(void);
void Map110_Apu( NWORD wAddr, NBYTE byData );

void Map112_Init(void);
void Map112_Write( NWORD wAddr, NBYTE byData );
void Map112_HSync(void);
void Map112_Set_CPU_Banks(void);
void Map112_Set_PPU_Banks(void);

void Map113_Init(void);
void Map113_Apu( NWORD wAddr, NBYTE byData );
void Map113_Write( NWORD wAddr, NBYTE byData );

void Map114_Init(void);
void Map114_Sram( NWORD wAddr, NBYTE byData );
void Map114_Write( NWORD wAddr, NBYTE byData );
void Map114_HSync(void);
void Map114_Set_CPU_Banks(void);
void Map114_Set_PPU_Banks(void);

void Map115_Init(void);
void Map115_Sram( NWORD wAddr, NBYTE byData );
void Map115_Write( NWORD wAddr, NBYTE byData );
void Map115_HSync(void);
void Map115_Set_CPU_Banks(void);
void Map115_Set_PPU_Banks(void);

void Map116_Init(void);
void Map116_Write( NWORD wAddr, NBYTE byData );
void Map116_HSync(void);
void Map116_Set_CPU_Banks(void);
void Map116_Set_PPU_Banks(void);

void Map117_Init(void);
void Map117_Write( NWORD wAddr, NBYTE byData );
void Map117_HSync(void);

void Map118_Init(void);
void Map118_Write( NWORD wAddr, NBYTE byData );
void Map118_HSync(void);
void Map118_Set_CPU_Banks(void);
void Map118_Set_PPU_Banks(void);

void Map119_Init(void);
void Map119_Write( NWORD wAddr, NBYTE byData );
void Map119_HSync(void);
void Map119_Set_CPU_Banks(void);
void Map119_Set_PPU_Banks(void);

void Map122_Init(void);
void Map122_Sram( NWORD wAddr, NBYTE byData );

void Map133_Init(void);
void Map133_Apu( NWORD wAddr, NBYTE byData );

void Map134_Init(void);
void Map134_Apu( NWORD wAddr, NBYTE byData );

void Map135_Init(void);
void Map135_Apu( NWORD wAddr, NBYTE byData );
void Map135_Set_PPU_Banks(void);

void Map140_Init(void);
void Map140_Sram( NWORD wAddr, NBYTE byData );
void Map140_Apu( NWORD wAddr, NBYTE byData );

void Map151_Init(void);
void Map151_Write( NWORD wAddr, NBYTE byData );

void Map160_Init(void);
void Map160_Write( NWORD wAddr, NBYTE byData );
void Map160_HSync(void);

void Map180_Init(void);
void Map180_Write( NWORD wAddr, NBYTE byData );

void Map181_Init(void);
void Map181_Apu( NWORD wAddr, NBYTE byData );

void Map182_Init(void);
void Map182_Write( NWORD wAddr, NBYTE byData );
void Map182_HSync(void);

void Map183_Init(void);
void Map183_Write( NWORD wAddr, NBYTE byData );
void Map183_HSync(void);

void Map185_Init(void);
void Map185_Write( NWORD wAddr, NBYTE byData );

void Map187_Init(void);
void Map187_Write( NWORD wAddr, NBYTE byData );
void Map187_Apu( NWORD wAddr, NBYTE byData );
NBYTE Map187_ReadApu( NWORD wAddr );
void Map187_HSync(void);
void Map187_Set_CPU_Banks(void);
void Map187_Set_PPU_Banks(void);

void Map188_Init(void);
void Map188_Write( NWORD wAddr, NBYTE byData );

void Map189_Init(void);
void Map189_Apu( NWORD wAddr, NBYTE byData );
void Map189_Write( NWORD wAddr, NBYTE byData );
void Map189_HSync(void);

void Map191_Init(void);
void Map191_Apu( NWORD wAddr, NBYTE byData );
void Map191_Set_CPU_Banks(void);
void Map191_Set_PPU_Banks(void);

void Map193_Init(void);
void Map193_Sram( NWORD wAddr, NBYTE byData );

void Map194_Init(void);
void Map194_Write( NWORD wAddr, NBYTE byData );

void Map200_Init(void);
void Map200_Write( NWORD wAddr, NBYTE byData );

void Map201_Init(void);
void Map201_Write( NWORD wAddr, NBYTE byData );

void Map202_Init(void);
void Map202_Apu( NWORD wAddr, NBYTE byData );
void Map202_Write( NWORD wAddr, NBYTE byData );
void Map202_WriteSub( NWORD wAddr, NBYTE byData );

void Map222_Init(void);
void Map222_Write( NWORD wAddr, NBYTE byData );

void Map225_Init(void);
void Map225_Write( NWORD wAddr, NBYTE byData );

void Map226_Init(void);
void Map226_Write( NWORD wAddr, NBYTE byData );

void Map227_Init(void);
void Map227_Write( NWORD wAddr, NBYTE byData );

void Map228_Init(void);
void Map228_Write( NWORD wAddr, NBYTE byData );

void Map229_Init(void);
void Map229_Write( NWORD wAddr, NBYTE byData );

void Map230_Init(void);
void Map230_Write( NWORD wAddr, NBYTE byData );

void Map231_Init(void);
void Map231_Write( NWORD wAddr, NBYTE byData );

void Map232_Init(void);
void Map232_Write( NWORD wAddr, NBYTE byData );

void Map233_Init(void);
void Map233_Write( NWORD wAddr, NBYTE byData );

void Map234_Init(void);
void Map234_Write( NWORD wAddr, NBYTE byData );
void Map234_Set_Banks(void);

void Map235_Init(void);
void Map235_Write( NWORD wAddr, NBYTE byData );

void Map236_Init(void);
void Map236_Write( NWORD wAddr, NBYTE byData );

void Map240_Init(void);
void Map240_Apu( NWORD wAddr, NBYTE byData );

void Map241_Init(void);
void Map241_Write( NWORD wAddr, NBYTE byData );

void Map242_Init(void);
void Map242_Write( NWORD wAddr, NBYTE byData );

void Map243_Init(void);
void Map243_Apu( NWORD wAddr, NBYTE byData );

void Map244_Init(void);
void Map244_Write( NWORD wAddr, NBYTE byData );

void Map245_Init(void);
void Map245_Write( NWORD wAddr, NBYTE byData );
void Map245_HSync(void);
#if 0
void Map245_Set_CPU_Banks(void);
void Map245_Set_PPU_Banks(void);
#endif 

void Map246_Init(void);
void Map246_Sram( NWORD wAddr, NBYTE byData );

void Map248_Init(void);
void Map248_Write( NWORD wAddr, NBYTE byData );
void Map248_Apu( NWORD wAddr, NBYTE byData );
void Map248_Sram( NWORD wAddr, NBYTE byData );
void Map248_HSync(void);
void Map248_Set_CPU_Banks(void);
void Map248_Set_PPU_Banks(void);

void Map249_Init(void);
void Map249_Write( NWORD wAddr, NBYTE byData );
void Map249_Apu( NWORD wAddr, NBYTE byData );
void Map249_HSync(void);

void Map251_Init(void);
void Map251_Write( NWORD wAddr, NBYTE byData );
void Map251_Sram( NWORD wAddr, NBYTE byData );
void Map251_Set_Banks(void);

void Map252_Init(void);
void Map252_Write( NWORD wAddr, NBYTE byData );
void Map252_HSync(void);

void Map255_Init(void);
void Map255_Write( NWORD wAddr, NBYTE byData );
void Map255_Apu( NWORD wAddr, NBYTE byData );
NBYTE Map255_ReadApu( NWORD wAddr );

#endif /* !InfoNES_MAPPER_H_INCLUDED */
