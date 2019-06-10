/*===================================================================*/
/*                                                                   */
/*                 Mapper 33 (Taito TC0190/TC0350)                   */
/*                                                                   */
/*===================================================================*/

// NBYTE Map33_Regs[ 8 ];
// NBYTE Map33_Switch;
// NBYTE Map33_IRQ_Enable;
// NBYTE Map33_IRQ_Cnt;

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 33                                             */
/*-------------------------------------------------------------------*/
void Map33_Init()
{
  int nPage;

  /* Initialize Mapper */
  MapperInit = Map33_Init;

  /* Write to Mapper */
  MapperWrite = Map33_Write;

  /* Write to SRAM */
  MapperSram = Map0_Sram;

  /* Write to APU */
  MapperApu = Map0_Apu;

  /* Read from APU */
  MapperReadApu = Map0_ReadApu;

  /* Callback at VSync */
  MapperVSync = Map0_VSync;

  /* Callback at HSync */
  MapperHSync = Map0_HSync;

  /* Callback at PPU */
  MapperPPU = Map0_PPU;

  /* Callback at Rendering Screen ( 1:BG, 0:Sprite ) */
  MapperRenderScreen = Map0_RenderScreen;

  /* Set SRAM Banks */
  SRAMBANK = SRAM;

  /* Set ROM Banks */
  ROMBANK0 = ROMPAGE( 0 );
  ROMBANK1 = ROMPAGE( 1 );
  ROMBANK2 = ROMLASTPAGE( 1 );
  ROMBANK3 = ROMLASTPAGE( 0 );

  /* Set PPU Banks */
  if ( Neshd->byVRomSize > 0 )
  {
    for ( nPage = 0; nPage < 8; ++nPage )
    {
      PPUBANK[ nPage ] = VROMPAGE( nPage );
      MAP33->Map33_Regs[ nPage ] = nPage;
    }
    InfoNES_SetupChr();
  } 
  else 
  {
    for ( nPage = 0; nPage < 8; ++nPage )
    {
      MAP33->Map33_Regs[ nPage ] = 0;
    }
  }

  /* Initialize State Registers */
  MAP33->Map33_Switch = 0;
  MAP33->Map33_IRQ_Enable = 0;
  MAP33->Map33_IRQ_Cnt = 0;

  /* Set up wiring of the interrupt pin */
  K6502_Set_Int_Wiring( 1, 1 ); 
}

/*-------------------------------------------------------------------*/
/*  Mapper 33 Write Function                                         */
/*-------------------------------------------------------------------*/
void Map33_Write( NWORD wAddr, NBYTE byData )
{
  /* Set ROM Banks */
  switch ( wAddr )
  {
    case 0x8000:
      byData %= ( Neshd->byRomSize << 1 );
      ROMBANK0 = ROMPAGE( byData );
      break;

    case 0x8001:
      byData %= ( Neshd->byRomSize << 1 );
      ROMBANK1 = ROMPAGE( byData );
      break;

    case 0x8002:
      MAP33->Map33_Regs[ 0 ] = byData * 2;
      MAP33->Map33_Regs[ 1 ] = byData * 2 + 1;

      PPUBANK[ 0 ] = VROMPAGE( MAP33->Map33_Regs[ 0 ] % ( Neshd->byVRomSize << 3 ) );
      PPUBANK[ 1 ] = VROMPAGE( MAP33->Map33_Regs[ 1 ] % ( Neshd->byVRomSize << 3 ) );
      InfoNES_SetupChr();
      break;

    case 0x8003:
      MAP33->Map33_Regs[ 2 ] = byData * 2;
      MAP33->Map33_Regs[ 3 ] = byData * 2 + 1;

      PPUBANK[ 2 ] = VROMPAGE( MAP33->Map33_Regs[ 2 ] % ( Neshd->byVRomSize << 3 ) );
      PPUBANK[ 3 ] = VROMPAGE( MAP33->Map33_Regs[ 3 ] % ( Neshd->byVRomSize << 3 ) );
      InfoNES_SetupChr();
      break;

    case 0xa000:
      MAP33->Map33_Regs[ 4 ] = byData;
      PPUBANK[ 4 ] = VROMPAGE( MAP33->Map33_Regs[ 4 ] % ( Neshd->byVRomSize << 3 ) );
      InfoNES_SetupChr();
      break;

    case 0xa001:
      MAP33->Map33_Regs[ 5 ] = byData;
      PPUBANK[ 5 ] = VROMPAGE( MAP33->Map33_Regs[ 5 ] % ( Neshd->byVRomSize << 3 ) );
      InfoNES_SetupChr();
      break;

    case 0xa002:
      MAP33->Map33_Regs[ 6 ] = byData;
      PPUBANK[ 6 ] = VROMPAGE( MAP33->Map33_Regs[ 6 ] % ( Neshd->byVRomSize << 3 ) );
      InfoNES_SetupChr();
      break;

    case 0xa003:
      MAP33->Map33_Regs[ 7 ] = byData;
      PPUBANK[ 7 ] = VROMPAGE( MAP33->Map33_Regs[ 7 ] % ( Neshd->byVRomSize << 3 ) );
      InfoNES_SetupChr();
      break;

    case 0xc000:
      MAP33->Map33_IRQ_Cnt = byData;
      break;

    case 0xc001:
    case 0xc002:
    case 0xe001:
    case 0xe002:
      if ( MAP33->Map33_IRQ_Cnt == byData )
      {
        MAP33->Map33_IRQ_Enable = 0xff;
      } else {
        MAP33->Map33_IRQ_Enable = byData;
      }
      break;

    case 0xe000:
      if ( byData & 0x40 )
      {
        InfoNES_Mirroring( 0 );
      } else {
        InfoNES_Mirroring( 1 );
      }
      break;
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 33 H-Sync Function                                        */
/*-------------------------------------------------------------------*/
void Map33_HSync()
{
/*
 *  Callback at HSync
 *
 */
  if ( MAP33->Map33_IRQ_Enable )
  {
    if ( MAP33->Map33_IRQ_Enable == 0xff )
    {
      if ( NES->PPU_Scanline == (NWORD)( 0xff - MAP33->Map33_IRQ_Cnt ) )
      {
        IRQ_REQ;
        MAP33->Map33_IRQ_Cnt = 0;
        MAP33->Map33_IRQ_Enable = 0;
      }
    } else {
      if ( MAP33->Map33_IRQ_Cnt == 0xff )
      {
        IRQ_REQ;
        MAP33->Map33_IRQ_Cnt = 0;
        MAP33->Map33_IRQ_Enable = 0;
      } else {
        MAP33->Map33_IRQ_Cnt++;
      }
    }
  }
}