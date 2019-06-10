/*===================================================================*/
/*                                                                   */
/*  InfoNES_pAPU.h : InfoNES Sound Emulation Function                */
/*                                                                   */
/*  2014/09/25  InfoNES Project ( based on DarcNES and NesterJ )     */
/*                                                                   */
/*===================================================================*/
#ifndef InfoNES_PAPU_H_INCLUDED
#define InfoNES_PAPU_H_INCLUDED

//#define APU_REG_QUICK_ACCESS	//APU_REGøÏÀŸ∑√Œ 

/*-------------------------------------------------------------------*/
/*   44100/60=735		22050/60=367                                     */
/*-------------------------------------------------------------------*/
#define MAX_PCM   367	
#define WAV_MAX_NUM  1

#define  FRAME_SKIP  0

#ifndef  TRUE
#define  TRUE     1
#endif
#ifndef  FALSE
#define  FALSE    0
#endif

/*-------------------------------------------------------------------*/
/*   define this for realtime generated noise                        */
/*-------------------------------------------------------------------*/

#define  APU_WRA0       0x4000
#define  APU_WRA1       0x4001
#define  APU_WRA2       0x4002
#define  APU_WRA3       0x4003
#define  APU_WRB0       0x4004
#define  APU_WRB1       0x4005
#define  APU_WRB2       0x4006
#define  APU_WRB3       0x4007
#define  APU_WRC0       0x4008
#define  APU_WRC2       0x400A
#define  APU_WRC3       0x400B
#define  APU_WRD0       0x400C
#define  APU_WRD2       0x400E
#define  APU_WRD3       0x400F
#define  APU_WRE0       0x4010
#define  APU_WRE1       0x4011
#define  APU_WRE2       0x4012
#define  APU_WRE3       0x4013

#define  APU_SMASK      0x4015

/*-------------------------------------------------------------------*/
/*   length of generated noise                        							 */
/*-------------------------------------------------------------------*/

#define  APU_BASEFREQ   1789772.7272727272727272

/*-------------------------------------------------------------------*/
/*   to/from 16.16 fixed point                        							 */
/*-------------------------------------------------------------------*/
#define  APU_TO_FIXED(x)    ((x) << 16)
#define  APU_FROM_FIXED(x)  ((x) >> 16)

/*-------------------------------------------------------------------*/
/* channel structures 																							 */
/* As much data as possible is precalculated,												 */
/* to keep the sample processing as lean as possible                 */
/*-------------------------------------------------------------------*/ 
typedef struct rectangle_s
{
   unsigned char regs[4];
   unsigned char enabled;  
   int phaseacc;
   int freq;
   int output_vol;
   unsigned char fixed_envelope;
   unsigned char holdnote;
   unsigned char volume;
   int sweep_phase;
   int sweep_delay;
   unsigned char sweep_on;
   unsigned char sweep_shifts;
   unsigned char sweep_length;
   unsigned char sweep_inc;
	
   /* this may not be necessary in the future */
   int freq_limit;

   /* rectangle 0 uses a complement addition for sweep
   ** increases, while rectangle 1 uses subtraction
   */
   unsigned char sweep_complement;

   int env_phase;
   int env_delay;
   unsigned char env_vol;
   int vbl_length;
   unsigned char adder;
   int duty_flip;
} rectangle_t;

typedef struct triangle_s
{
   unsigned char regs[3];
   unsigned char enabled;
   int freq;
   int phaseacc;
   int output_vol;
   unsigned char adder;
   unsigned char holdnote;
   unsigned char counter_started;
   /* quasi-hack */
   int write_latency;
   int vbl_length;
   int linear_length;
} triangle_t;


typedef struct noise_s
{
   unsigned char regs[3];
   unsigned char enabled;
   int freq;
   int phaseacc;
   int output_vol;
   int env_phase;
   int env_delay;
   unsigned char env_vol;
   unsigned char fixed_envelope;
   unsigned char holdnote;
   unsigned char volume;
   int vbl_length;
   unsigned char xor_tap;
} noise_t;

typedef struct dmc_s
{
   unsigned char regs[4];

   /* bodge for timestamp queue */
   unsigned char enabled;
   
   int freq;
   int phaseacc;
   int output_vol;
   unsigned int address;
   unsigned int cached_addr;
   int dma_length;
   int cached_dmalength;
   unsigned char cur_byte;
   unsigned char looping;
   unsigned char irq_gen;
   unsigned char irq_occurred;
} dmc_t;

enum
{
   APU_FILTER_NONE,
   APU_FILTER_LOWPASS,
   APU_FILTER_WEIGHTED
};

/*-------------------------------------------------------------------*/
/*  APU queue structure                                              */
/*-------------------------------------------------------------------*/
#define  APUQUEUE_SIZE  1024  //4096
#define  APUQUEUE_MASK  (APUQUEUE_SIZE - 1)

/*-------------------------------------------------------------------*/
/*  apu ring buffer member                                           */
/*-------------------------------------------------------------------*/
typedef struct apudata_s
{
   unsigned int timestamp, address;
   unsigned char value;
} apudata_t;

/*-------------------------------------------------------------------*/
/*  look up table madness                                            */
/*-------------------------------------------------------------------*/
typedef struct 
{
	 int decay_lut[16];
	 int vbl_lut[32];
	 int trilength_lut[128];
}aputable;

typedef struct apu_s
{
   rectangle_t rectangle[2];
   triangle_t triangle;
   noise_t noise;
   dmc_t dmc;
   unsigned char enable_reg;

   int q_head, q_tail;
   int num_samples;
   int filter_type;
   int cycle_rate;
   int sample_rate;
   int refresh_rate;
   /* external sound chip */ 
	 aputable  table;
	 unsigned char Soundcount;
	 short *Abuf1;
	 short *Abuf2;
	 char WavNum;
} apu_t;

extern apu_t *APU; 

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*-------------------------------------------------------------------*/
/*  Function prototypes                                              */
/*-------------------------------------------------------------------*/	
	
#ifdef APU_REG_QUICK_ACCESS	
	void InfoNES_pAPUregwrite4000(unsigned char value);/*apureg $0x4000*/
	void InfoNES_pAPUregwrite4001(unsigned char value);/*apureg $0x4001*/
	void InfoNES_pAPUregwrite4002(unsigned char value);/*apureg $0x4002*/
	void InfoNES_pAPUregwrite4003(unsigned char value);/*apureg $0x4003*/
	void InfoNES_pAPUregwrite4004(unsigned char value);/*apureg $0x4004*/
	void InfoNES_pAPUregwrite4005(unsigned char value);/*apureg $0x4005*/
	void InfoNES_pAPUregwrite4006(unsigned char value);/*apureg $0x4006*/
	void InfoNES_pAPUregwrite4007(unsigned char value);/*apureg $0x4007*/
	void InfoNES_pAPUregwrite4008(unsigned char value);/*apureg $0x4008*/
	void InfoNES_pAPUregwrite400A(unsigned char value);/*apureg $0x400A*/
	void InfoNES_pAPUregwrite400B(unsigned char value);/*apureg $0x400B*/
	void InfoNES_pAPUregwrite400C(unsigned char value);/*apureg $0x400C*/
	void InfoNES_pAPUregwrite400E(unsigned char value);/*apureg $0x400E*/
	void InfoNES_pAPUregwrite400F(unsigned char value);/*apureg $0x400F*/
	void InfoNES_pAPUregwrite4010(unsigned char value);/*apureg $0x4010*/
	void InfoNES_pAPUregwrite4011(unsigned char value);/*apureg $0x4011*/
	void InfoNES_pAPUregwrite4012(unsigned char value);/*apureg $0x4012*/
	void InfoNES_pAPUregwrite4013(unsigned char value);/*apureg $0x4013*/
	void InfoNES_pAPUregwrite4015(unsigned char value);/*apureg $0x4015*/	
#else
	void InfoNES_pAPUregwrite(unsigned int address, unsigned char value);
#endif
	
	
	unsigned char InfoNES_pAPURead4015(void);	
	void InfoNES_pAPUprocess(short *buffer, int num_samples);	
	void InfoNES_pAPUVsync(void);
	void InfoNES_pAPUDone(void);
	void InfoNES_pAPUInit(void);
	
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _NES_APU_H_ */





