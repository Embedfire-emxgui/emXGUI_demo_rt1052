/*
 * The Clear BSD License
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted (subject to the limitations in the disclaimer below) provided
 *  that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED BY THIS LICENSE.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "board.h"
#include "music.h"
#include "fsl_dmamux.h"
#include "fsl_sai_edma.h"
#include "fsl_debug_console.h"

#include "fsl_wm8960.h"
#include "pin_mux.h"
#include "clock_config.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
extern void BOARD_Codec_I2C_Init(void);
extern status_t BOARD_Codec_I2C_Send(
    uint8_t deviceAddress, uint32_t subAddress, uint8_t subAddressSize, const uint8_t *txBuff, uint8_t txBuffSize);
extern status_t BOARD_Codec_I2C_Receive(
    uint8_t deviceAddress, uint32_t subAddress, uint8_t subAddressSize, uint8_t *rxBuff, uint8_t rxBuffSize);		
codec_config_t boardCodecConfig = {
    .I2C_SendFunc = BOARD_Codec_I2C_Send,
    .I2C_ReceiveFunc = BOARD_Codec_I2C_Receive,
    .op.Init = WM8960_Init,
    .op.Deinit = WM8960_Deinit,
    .op.SetFormat = WM8960_ConfigDataFormat
};
 

/* SAI instance and clock */
#define DEMO_CODEC_WM8960
#define DEMO_SAI SAI1
#define DEMO_SAI_IRQ SAI1_IRQn
#define SAI_TxIRQHandler SAI1_IRQHandler

/* Select Audio/Video PLL (786.48 MHz) as sai1 clock source */
#define DEMO_SAI1_CLOCK_SOURCE_SELECT (2U)
/* Clock pre divider for sai1 clock source */
#define DEMO_SAI1_CLOCK_SOURCE_PRE_DIVIDER (0U)
/* Clock divider for sai1 clock source */
#define DEMO_SAI1_CLOCK_SOURCE_DIVIDER (15U)
/* Get frequency of sai1 clock */
#define DEMO_SAI_CLK_FREQ                                                        \
    (CLOCK_GetFreq(kCLOCK_AudioPllClk) / (DEMO_SAI1_CLOCK_SOURCE_DIVIDER + 1U) / \
     (DEMO_SAI1_CLOCK_SOURCE_PRE_DIVIDER + 1U))

/* I2C instance and clock */
#define DEMO_I2C LPI2C1

/* Select USB1 PLL (480 MHz) as master lpi2c clock source */
#define DEMO_LPI2C_CLOCK_SOURCE_SELECT (0U)
/* Clock divider for master lpi2c clock source */
#define DEMO_LPI2C_CLOCK_SOURCE_DIVIDER (5U)
/* Get frequency of lpi2c clock */
#define DEMO_I2C_CLK_FREQ ((CLOCK_GetFreq(kCLOCK_Usb1PllClk) / 8) / (DEMO_LPI2C_CLOCK_SOURCE_DIVIDER + 1U))

/* DMA */
#define DMAMUX0 DMAMUX
#define EXAMPLE_DMA DMA0
#define EXAMPLE_CHANNEL (0U)
#define EXAMPLE_SAI_TX_SOURCE kDmaRequestMuxSai1Tx

#define OVER_SAMPLE_RATE (384U)
#define BUFFER_SIZE (1470U)
#define BUFFER_NUM (2U)
/*******************************************************************************
 * Prototypes
 ******************************************************************************/
static void callback(I2S_Type *base, sai_edma_handle_t *handle, status_t status, void *userData);

/*******************************************************************************
 * Variables
 ******************************************************************************/
AT_NONCACHEABLE_SECTION_INIT(sai_edma_handle_t txHandle) = {0};
edma_handle_t dmaHandle = {0};
codec_handle_t codecHandle = {0};
AT_NONCACHEABLE_SECTION_ALIGN(uint16_t buffer[BUFFER_NUM][BUFFER_SIZE], 4);
volatile bool isFinished = false;
volatile uint32_t finishIndex = 0U;
volatile uint32_t emptyBlock = BUFFER_NUM;

volatile uint32_t music_len=735;
uint32_t tx_index=1U;
/*******************************************************************************
 * Code
 ******************************************************************************/

/*
 * AUDIO PLL setting: Frequency = Fref * (DIV_SELECT + NUM / DENOM)
 *                              = 24 * (32 + 77/100)
 *                              = 786.48 MHz
 */
const clock_audio_pll_config_t audioPllConfig = {
    .loopDivider = 37,  /* PLL loop divider. Valid range for DIV_SELECT divider value: 27~54. */
    .postDivider = 1,   /* Divider after the PLL, should only be 1, 2, 4, 8, 16. */
    .numerator = 632,    /* 30 bit numerator of fractional loop divider. */
    .denominator = 1000, /* 30 bit denominator of fractional loop divider */
};

void BOARD_EnableSaiMclkOutput(bool enable)
{
    if (enable)
    {
        IOMUXC_GPR->GPR1 |= IOMUXC_GPR_GPR1_SAI1_MCLK_DIR_MASK;
    }
    else
    {
        IOMUXC_GPR->GPR1 &= (~IOMUXC_GPR_GPR1_SAI1_MCLK_DIR_MASK);
    }
}

#include "InfoNES_pAPU.h"
#include "emXGUI.h"
extern WORD Abuf1[735];
extern WORD Abuf2[735];
//extern WORD *Abuf1;
//extern WORD *Abuf2;
extern __IO uint8_t Soundcount;
extern GUI_SEM* sai_complete_sem;
static void callback(I2S_Type *base, sai_edma_handle_t *handle, status_t status, void *userData)
{
    uint32_t temp = 0;
		sai_transfer_t xfer;
    if(kStatus_SAI_RxError == status)
    {
    }
    else
    {
#if Limit_Speed      
      GUI_SemPostISR(sai_complete_sem);  
//			isFinished = true;
#else
      if(Soundcount)
      {
          xfer.data = (uint8_t *)Abuf1;
          xfer.dataSize = 367*2;  
          SAI_TransferSendEDMA(SAI1, &txHandle, &xfer);
          Soundcount=0;
      }
      else
      {
          /*  xfer structure */
          xfer.data = (uint8_t *)Abuf2;
          xfer.dataSize = 367*2;  
          SAI_TransferSendEDMA(SAI1, &txHandle, &xfer);
          Soundcount=1;
      }
#endif      
    }
}

/*!
 * @brief Main function
 */
sai_transfer_format_t format;   
edma_config_t dmaConfig;



int AudioTest(void)
{
    sai_config_t config;
    uint32_t mclkSourceClockHz = 0U;
    sai_transfer_format_t format;   
    edma_config_t dmaConfig;
	
    uint32_t delayCycle = 500000U;

    CLOCK_InitAudioPll(&audioPllConfig);
    BOARD_InitDebugConsole();

    /*Clock setting for LPI2C*/
    CLOCK_SetMux(kCLOCK_Lpi2cMux, DEMO_LPI2C_CLOCK_SOURCE_SELECT);
    CLOCK_SetDiv(kCLOCK_Lpi2cDiv, DEMO_LPI2C_CLOCK_SOURCE_DIVIDER);

    /*Clock setting for SAI1*/
    CLOCK_SetMux(kCLOCK_Sai1Mux, DEMO_SAI1_CLOCK_SOURCE_SELECT);
    CLOCK_SetDiv(kCLOCK_Sai1PreDiv, DEMO_SAI1_CLOCK_SOURCE_PRE_DIVIDER);
    CLOCK_SetDiv(kCLOCK_Sai1Div, DEMO_SAI1_CLOCK_SOURCE_DIVIDER);

    /*Enable MCLK clock*/
    BOARD_EnableSaiMclkOutput(true);
    BOARD_Codec_I2C_Init();

    PRINTF("SAI example started!\n\r");

    /* Create EDMA handle */
    /*
     * dmaConfig.enableRoundRobinArbitration = false;
     * dmaConfig.enableHaltOnError = true;
     * dmaConfig.enableContinuousLinkMode = false;
     * dmaConfig.enableDebugMode = false;
     */
		EDMA_GetDefaultConfig(&dmaConfig);
		EDMA_Init(EXAMPLE_DMA, &dmaConfig);
		EDMA_CreateHandle(&dmaHandle, EXAMPLE_DMA, EXAMPLE_CHANNEL);


    DMAMUX_Init(DMAMUX0);
    DMAMUX_SetSource(DMAMUX0, EXAMPLE_CHANNEL, EXAMPLE_SAI_TX_SOURCE);
    DMAMUX_EnableChannel(DMAMUX0, EXAMPLE_CHANNEL);

    /* Init SAI module */
    /*
     * config.masterSlave = kSAI_Master;
     * config.mclkSource = kSAI_MclkSourceSysclk;
     * config.protocol = kSAI_BusLeftJustified;
     * config.syncMode = kSAI_ModeAsync;
     * config.mclkOutputEnable = true;
     */
    SAI_TxGetDefaultConfig(&config);

    SAI_TxInit(DEMO_SAI, &config);

    /* Configure the audio format */
    format.bitWidth = kSAI_WordWidth16bits;
    format.channel = 0U;
    format.sampleRate_Hz = kSAI_SampleRate11025Hz;//kSAI_SampleRate22050Hz;

    format.masterClockHz = DEMO_SAI_CLK_FREQ;

    format.protocol = config.protocol;
    format.stereo = kSAI_Stereo;
    format.isFrameSyncCompact = false;

    format.watermark = FSL_FEATURE_SAI_FIFO_COUNT / 2U;


    /* Use default setting to init codec */
    CODEC_Init(&codecHandle, &boardCodecConfig);
    CODEC_SetFormat(&codecHandle, format.masterClockHz, format.sampleRate_Hz, format.bitWidth);
		
    while (delayCycle)
    {
        __ASM("nop");
        delayCycle--;
    }
    NVIC_SetPriority(DEMO_SAI_IRQ, 8U);
    NVIC_SetPriority(DMA0_DMA16_IRQn, 8U);
    SAI_TransferTxCreateHandleEDMA(DEMO_SAI, &txHandle, callback, NULL,&dmaHandle);

    
    
    mclkSourceClockHz = DEMO_SAI_CLK_FREQ;
    SAI_TransferTxSetFormatEDMA(DEMO_SAI, &txHandle, &format, mclkSourceClockHz, format.masterClockHz);

//    uint32_t temp = 0;
//    sai_transfer_t xfer;  
//  //temp = (uint32_t)music;
//    /*  xfer structure */
//    xfer.data = (uint8_t *)music;
//    xfer.dataSize = 48000;  
//    SAI_TransferSendEDMA(SAI1, &txHandle, &xfer);
//    getRegValue();
//    while(!isFinished); 
//  
//    
//    /* Once transfer finish, disable SAI instance. */
//    SAI_TransferAbortSendEDMA(DEMO_SAI, &txHandle);
//    SAI_Deinit(DEMO_SAI);
//    PRINTF("\n\r SAI EDMA example finished!\n\r ");
//    while (1)
//    {
//    }
		return 0;
}



void nes_audio(int len,uint16_t *nes_audio_buffer)
{
		//uint32_t cpy_index = 0U, tx_index=0U;
		
		music_len=len;
		/* Waiting until finished. */
//		while(!isFinished)

//				PRINTF("music_len = %d\r\n",music_len);
//				if(music_len< 5)
//					break;
				//if(isFinished)//&& (cpy_index < music_len/BUFFER_SIZE)
						 /* Fill in the buffers. */
						//memcpy(buffer,nes_audio_buffer,music_len*2);
//		int i;	

////		if(isFinished)
//		{	
//			isFinished=0;					
//			if(tx_index)
//			{
//				for(i=0;i<music_len;i++)
//				{
//					buffer[0][2*i]=nes_audio_buffer[i];
//					buffer[0][2*i+1]=nes_audio_buffer[i];
//				}
//			}
//			else 
//			{
//				for(i=0;i<music_len;i++)
//				{
//					buffer[1][2*i]=nes_audio_buffer[i];
//					buffer[1][2*i+1]=nes_audio_buffer[i];
//				}			
//			}
//		}
						
}
