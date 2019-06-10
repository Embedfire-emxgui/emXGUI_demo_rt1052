/*===================================================================*/
/*                                                                   */
/*  InfoNES_pAPU.cpp : InfoNES Sound Emulation Function              */
/*                                                                   */
/*  2014/09/25  InfoNES Project ( based on DarcNES and NesterJ )     */
/*                                                                   */
/*===================================================================*/

/*-------------------------------------------------------------------*/
/*  Include files                                                    */
/*-------------------------------------------------------------------*/
#include "K6502.h"
#include "K6502_rw.h"
#include "InfoNES_System.h"
#include "InfoNES_pAPU.h"
  
/*-------------------------------------------------------------------*/
/*   APU resources                                                   */
/*-------------------------------------------------------------------*/

#define log_printf printf
#define  APU_VOLUME_DECAY(x)  ((x) -= ((x) >> 7))

/*-------------------------------------------------------------------*/
/*  vblank length table used for rectangles, triangle, noise         */
/*-------------------------------------------------------------------*/
static const unsigned char vbl_length[32] =
{
  5, 127, 10, 1, 19,  2, 40,  3, 80,  4, 30,  5, 7,  6, 13,  7,
  6,   8, 12, 9, 24, 10, 48, 11, 96, 12, 36, 13, 8, 14, 16, 15,
};

/*-------------------------------------------------------------------*/
/* Frequency Limit of Rectangle Channels                             */
/*-------------------------------------------------------------------*/
static const int freq_limit[8] =
{
   0x3FF, 0x555, 0x666, 0x71C, 0x787, 0x7C1, 0x7E0, 0x7F0
};

/*-------------------------------------------------------------------*/
/* noise frequency lookup table                            					 */
/*-------------------------------------------------------------------*/
static const int noise_freq[16] =
{
     4,    8,   16,   32,   64,   96,  128,  160,
   202,  254,  380,  508,  762, 1016, 2034, 4068
};

/*-------------------------------------------------------------------*/
/* DMC Transfer Clocks Table                                          */
/*-------------------------------------------------------------------*/
const int dmc_clocks[16] =
{
   428, 380, 340, 320, 286, 254, 226, 214,
   190, 160, 142, 128, 106,  85,  72,  54
};

/* ratios of pos/neg pulse for rectangle waves */
static const int duty_lut[4] = { 2, 4, 8, 12 };

void InfoNES_pAPUdmcreload(dmc_t *chan);

#ifdef APU_REG_QUICK_ACCESS	

/*-------------------------------------------------------------------*/
/*     Write from $0x4000                                            */
/*-------------------------------------------------------------------*/
void InfoNES_pAPUregwrite4000(unsigned char value)
{  
   rectangle_t * rect = &(APU->rectangle[0]);	
	/* rectangles */
	//case APU_WRA0:

	rect->regs[0] = value;
	rect->volume = value & 0x0F;
	rect->env_delay = APU->table.decay_lut[value & 0x0F];
	rect->holdnote = (value & 0x20) ? TRUE : FALSE;
	rect->fixed_envelope = (value & 0x10) ? TRUE : FALSE;
	rect->duty_flip = duty_lut[value >> 6];
}

/*-------------------------------------------------------------------*/
/*     Write from $0x4001							                               */
/*-------------------------------------------------------------------*/
void InfoNES_pAPUregwrite4001(unsigned char value)
{  
   rectangle_t * rect = &(APU->rectangle[0]);
	
	//case APU_WRA1:

	rect->regs[1] = value;
	rect->sweep_on = (value & 0x80) ? TRUE : FALSE;
	rect->sweep_shifts = value & 7;
	rect->sweep_delay = APU->table.decay_lut[(value >> 4) & 7];

	rect->sweep_inc = (value & 0x08) ? TRUE : FALSE;
	rect->freq_limit = freq_limit[value & 7];
}

/*-------------------------------------------------------------------*/
/*     Write from $0x4002						                                 */
/*-------------------------------------------------------------------*/
void InfoNES_pAPUregwrite4002(unsigned char value)
{  
   rectangle_t * rect =  &(APU->rectangle[0]);
	
	//case APU_WRA2:

	rect->regs[2] = value;
	rect->freq = (APU->rectangle[0].freq & ~0xFF) | value;
}

/*-------------------------------------------------------------------*/
/*     Write from $0x4003 						                               */
/*-------------------------------------------------------------------*/
void InfoNES_pAPUregwrite4003(unsigned char value)
{  
   rectangle_t * rect = &(APU->rectangle[0]);
	
		//case APU_WRA3:

		rect->regs[3] = value;

		rect->vbl_length = APU->table.vbl_lut[value >> 3];
		rect->env_vol = 0;
		rect->freq = ((value & 7) << 8) | (rect->freq & 0xFF);
		rect->adder = 0;
}

/*-------------------------------------------------------------------*/
/*     Write from $0x4004 						                               */
/*-------------------------------------------------------------------*/
void InfoNES_pAPUregwrite4004(unsigned char value)
{  
   rectangle_t * rect = &(APU->rectangle[1]);
	
	/* rectangles */

	//case APU_WRB0:

	rect->regs[0] = value;
	rect->volume = value & 0x0F;
	rect->env_delay = APU->table.decay_lut[value & 0x0F];
	rect->holdnote = (value & 0x20) ? TRUE : FALSE;
	rect->fixed_envelope = (value & 0x10) ? TRUE : FALSE;
	rect->duty_flip = duty_lut[value >> 6];
}

/*-------------------------------------------------------------------*/
/*     Write from  $0x4005 						                               */
/*-------------------------------------------------------------------*/
void InfoNES_pAPUregwrite4005(unsigned char value)
{  
   rectangle_t * rect = &(APU->rectangle[1]);
	
	//case APU_WRB1:
	rect->regs[1] = value;
	rect->sweep_on = (value & 0x80) ? TRUE : FALSE;
	rect->sweep_shifts = value & 7;
	rect->sweep_delay = APU->table.decay_lut[(value >> 4) & 7];

	rect->sweep_inc = (value & 0x08) ? TRUE : FALSE;
	rect->freq_limit = freq_limit[value & 7];
}

/*-------------------------------------------------------------------*/
/*     Write from $0x4006                                 					 */
/*-------------------------------------------------------------------*/
void InfoNES_pAPUregwrite4006(unsigned char value)
{  
   rectangle_t * rect = &(APU->rectangle[1]);
	
	//case APU_WRB2:

	rect->regs[2] = value;
	rect->freq = (APU->rectangle[1].freq & ~0xFF) | value;
}

/*-------------------------------------------------------------------*/
/*     Write from $0x4007    						                             */
/*-------------------------------------------------------------------*/
void InfoNES_pAPUregwrite4007(unsigned char value)
{  
   rectangle_t * rect = &(APU->rectangle[1]);
	
		//case APU_WRB3:

		rect->regs[3] = value;
		rect->vbl_length = APU->table.vbl_lut[value >> 3];
		rect->env_vol = 0;
		rect->freq = ((value & 7) << 8) | (rect->freq & 0xFF);
		rect->adder = 0;
}

/*-------------------------------------------------------------------*/
/*     Write from $0x4008                                            */
/*-------------------------------------------------------------------*/
void InfoNES_pAPUregwrite4008(unsigned char value)
{  
	/* triangle */
	//case APU_WRC0:

	triangle_t * tri = &(APU->triangle);
	tri->regs[0] = value;
	tri->holdnote = (value & 0x80) ? TRUE : FALSE;

	if (FALSE == tri->counter_started && tri->vbl_length)
	tri->linear_length = APU->table.trilength_lut[value & 0x7F];
}

/*-------------------------------------------------------------------*/
/*     Write from $0x400A                                            */
/*-------------------------------------------------------------------*/
void InfoNES_pAPUregwrite400A(unsigned char value)
{  
	//case APU_WRC2:

	triangle_t * tri = &(APU->triangle);
	tri->regs[1] = value;
	tri->freq = APU_TO_FIXED((((tri->regs[2] & 7) << 8) + value) + 1);
}

/*-------------------------------------------------------------------*/
/*     Write from $0x400B                                            */
/*-------------------------------------------------------------------*/
void InfoNES_pAPUregwrite400B(unsigned char value)
{  
	//case APU_WRC3:

	triangle_t * tri = &(APU->triangle);
	tri->regs[2] = value;
	tri->write_latency = (int) (228 / APU_FROM_FIXED(APU->cycle_rate));

	tri->freq = APU_TO_FIXED((((value & 7) << 8) + tri->regs[1]) + 1);
	tri->vbl_length = APU->table.vbl_lut[value >> 3];
	tri->counter_started = FALSE;
	tri->linear_length = APU->table.trilength_lut[tri->regs[0] & 0x7F];

}

/*-------------------------------------------------------------------*/
/*     Write from $0x400C                                            */
/*-------------------------------------------------------------------*/
void InfoNES_pAPUregwrite400C(unsigned char value)
{  
	   /* noise */
	//case APU_WRD0:
	noise_t * noise = &(APU->noise);
	noise->regs[0] = value;
	noise->env_delay = APU->table.decay_lut[value & 0x0F];
	noise->holdnote = (value & 0x20) ? TRUE : FALSE;
	noise->fixed_envelope = (value & 0x10) ? TRUE : FALSE;
	noise->volume = value & 0x0F;
}

/*-------------------------------------------------------------------*/
/*     Write from $0x400E                                            */
/*-------------------------------------------------------------------*/
void InfoNES_pAPUregwrite400E(unsigned char value)
{  
	// case APU_WRD2:

	noise_t * noise = &(APU->noise);
	noise->regs[1] = value;
	noise->freq = APU_TO_FIXED(noise_freq[value & 0x0F]);

	noise->xor_tap = (value & 0x80) ? 0x40: 0x02;
}

/*-------------------------------------------------------------------*/
/*     Write from $0x400F                                            */
/*-------------------------------------------------------------------*/
void InfoNES_pAPUregwrite400F(unsigned char value)
{  
// case APU_WRD3:

	noise_t * noise = &(APU->noise);
	noise->regs[2] = value;
	
	noise->vbl_length = APU->table.vbl_lut[value >> 3];
	noise->env_vol = 0; /* reset envelope */
}

/*-------------------------------------------------------------------*/
/*     Write from $0x4010                                            */
/*-------------------------------------------------------------------*/
void InfoNES_pAPUregwrite4010(unsigned char value)
{  
	/* DMC */
	//case APU_WRE0:

	 dmc_t * dmc = &(APU->dmc);
	 
	 dmc->regs[0] = value;
	 
	 dmc->freq = APU_TO_FIXED(dmc_clocks[value & 0x0F]);
	 dmc->looping = (value & 0x40) ? TRUE : FALSE;
	 
	 if (value & 0x80)
		 dmc->irq_gen = TRUE;
	 else
	 {
		 dmc->irq_gen = FALSE;
		 dmc->irq_occurred = FALSE;
	 }
}

/*-------------------------------------------------------------------*/
/*     Write from $0x4011                                            */
/*-------------------------------------------------------------------*/
void InfoNES_pAPUregwrite4011(unsigned char value)
{  
	//case APU_WRE1: /* 7-bit DAC */

	dmc_t * dmc = &(APU->dmc);
	value &= 0x7F; /* bit 7 ignored */
	dmc->output_vol += ((value - APU->dmc.regs[1]) << 8);
	dmc->regs[1] = value;
}

/*-------------------------------------------------------------------*/
/*     Write from $0x4012                                            */
/*-------------------------------------------------------------------*/
void InfoNES_pAPUregwrite4012(unsigned char value)
{  
	// case APU_WRE2:
	dmc_t * dmc = &(APU->dmc);
	dmc->regs[2] = value;
	dmc->cached_addr = 0xC000 + (unsigned short) (value << 6);
}

/*-------------------------------------------------------------------*/
/*     Write from $0x4013                                            */
/*-------------------------------------------------------------------*/
void InfoNES_pAPUregwrite4013(unsigned char value)
{  
	  // case APU_WRE3:
	 dmc_t * dmc = &(APU->dmc);
	 dmc->regs[3] = value;
	 dmc->cached_dmalength = ((value << 4) + 1) << 3;
}

/*-------------------------------------------------------------------*/
/*     Write from $0x4015                                            */
/*-------------------------------------------------------------------*/
void InfoNES_pAPUregwrite4015(unsigned char value)
{  
	// case APU_SMASK:

		dmc_t * dmc = &(APU->dmc);
		triangle_t * tri = &(APU->triangle);
		noise_t * noise = &(APU->noise);

		dmc->enabled = (value & 0x10) ? TRUE : FALSE;
		
		APU->enable_reg = value;
		
		if (value & 0x01)
			APU->rectangle[0].enabled = TRUE;
		else
		{
			APU->rectangle[0].enabled = FALSE;
			APU->rectangle[0].vbl_length = 0;
		}
		
		if (value & 0x02)
			APU->rectangle[1].enabled = TRUE;
		else
		{
			APU->rectangle[1].enabled = FALSE;
			APU->rectangle[1].vbl_length = 0;
		}
		
		if (value & 0x04)
			tri->enabled = TRUE;
		else
		{
			tri->enabled = FALSE;
			tri->vbl_length = 0;
			tri->linear_length = 0;
			tri->counter_started = FALSE;
			tri->write_latency = 0;
		}
			
		if (value & 0x08)
			noise->enabled = TRUE;
		else
		{
			noise->enabled = FALSE;
			noise->vbl_length = 0;
		}
		
		if (value & 0x10)
		{
			if (0 == dmc->dma_length)
				InfoNES_pAPUdmcreload(&APU->dmc);
		}
		else
		{
			dmc->dma_length = 0;	
		  dmc->irq_occurred = FALSE;
		}
}
#else

/*-------------------------------------------------------------------*/
/*    InfoNES_pAPUregwrite                                           */
/*-------------------------------------------------------------------*/
void InfoNES_pAPUregwrite(unsigned int address, unsigned char value)
{  
   int chan = (address & 4) ? 1 : 0;
   rectangle_t * rect = chan ? &(APU->rectangle[1]) : &(APU->rectangle[0]);

    
   switch (address)
   {
   /* rectangles */
   case APU_WRA0:
   case APU_WRB0:
	 {
      rect->regs[0] = value;
	  rect->volume = value & 0x0F;
      rect->env_delay = APU->table.decay_lut[value & 0x0F];
      rect->holdnote = (value & 0x20) ? TRUE : FALSE;
      rect->fixed_envelope = (value & 0x10) ? TRUE : FALSE;
      rect->duty_flip = duty_lut[value >> 6];
	 }
      break;

   case APU_WRA1:
   case APU_WRB1:
	 {
      rect->regs[1] = value;
      rect->sweep_on = (value & 0x80) ? TRUE : FALSE;
      rect->sweep_shifts = value & 7;
      rect->sweep_delay = APU->table.decay_lut[(value >> 4) & 7];
      
      rect->sweep_inc = (value & 0x08) ? TRUE : FALSE;
      rect->freq_limit = freq_limit[value & 7];
	 }
      break;

   case APU_WRA2:
   case APU_WRB2:
			{
      rect->regs[2] = value;
      rect->freq = (APU->rectangle[chan].freq & ~0xFF) | value;
			}
      break;

   case APU_WRA3:
   case APU_WRB3:
			{
      rect->regs[3] = value;

      rect->vbl_length = APU->table.vbl_lut[value >> 3];
      rect->env_vol = 0;
      rect->freq = ((value & 7) << 8) | (rect->freq & 0xFF);
      rect->adder = 0;
			}
      break;

   /* triangle */
   case APU_WRC0:
	   {
		   triangle_t * tri = &(APU->triangle);
		   tri->regs[0] = value;
		   tri->holdnote = (value & 0x80) ? TRUE : FALSE;
		   
		   if (FALSE == tri->counter_started && tri->vbl_length)
			 tri->linear_length = APU->table.trilength_lut[value & 0x7F];
	   }
      break;

   case APU_WRC2:
	   {
		   triangle_t * tri = &(APU->triangle);
		   tri->regs[1] = value;
           tri->freq = APU_TO_FIXED((((tri->regs[2] & 7) << 8) + value) + 1);
	   }
      break;

   case APU_WRC3:
	   {
		   triangle_t * tri = &(APU->triangle);
		   tri->regs[2] = value;
		   tri->write_latency = (int) (228 / APU_FROM_FIXED(APU->cycle_rate));
		   
		   tri->freq = APU_TO_FIXED((((value & 7) << 8) + tri->regs[1]) + 1);
		   tri->vbl_length = APU->table.vbl_lut[value >> 3];
		   tri->counter_started = FALSE;
		   tri->linear_length = APU->table.trilength_lut[tri->regs[0] & 0x7F];
	   }
	  break;

   /* noise */
   case APU_WRD0:
	   {
		   noise_t * noise = &(APU->noise);
		   noise->regs[0] = value;
		   noise->env_delay = APU->table.decay_lut[value & 0x0F];
		   noise->holdnote = (value & 0x20) ? TRUE : FALSE;
		   noise->fixed_envelope = (value & 0x10) ? TRUE : FALSE;
		   noise->volume = value & 0x0F;
	   }
      break;

   case APU_WRD2:
	  {
		  noise_t * noise = &(APU->noise);
		  noise->regs[1] = value;
		  noise->freq = APU_TO_FIXED(noise_freq[value & 0x0F]);
		  
		  noise->xor_tap = (value & 0x80) ? 0x40: 0x02;
	  }
      break;

   case APU_WRD3:
	  {
		  noise_t * noise = &(APU->noise);
		  noise->regs[2] = value;
		  
		  noise->vbl_length = APU->table.vbl_lut[value >> 3];
		  noise->env_vol = 0; /* reset envelope */
	  }
      break;

   /* DMC */
   case APU_WRE0:
	   {
		   dmc_t * dmc = &(APU->dmc);
		   
		   dmc->regs[0] = value;
		   
		   dmc->freq = APU_TO_FIXED(dmc_clocks[value & 0x0F]);
		   dmc->looping = (value & 0x40) ? TRUE : FALSE;
		   
		   if (value & 0x80)
			   dmc->irq_gen = TRUE;
		   else
		   {
			   dmc->irq_gen = FALSE;
			   dmc->irq_occurred = FALSE;
		   }
	   }
      break;

   case APU_WRE1: /* 7-bit DAC */
	  {
		  dmc_t * dmc = &(APU->dmc);
		  value &= 0x7F; /* bit 7 ignored */
		  dmc->output_vol += ((value - APU->dmc.regs[1]) << 8);
		  dmc->regs[1] = value;
	  }
      break;

   case APU_WRE2:
	   {
		   dmc_t * dmc = &(APU->dmc);
		   dmc->regs[2] = value;
		   dmc->cached_addr = 0xC000 + (unsigned short) (value << 6);
	   }
      break;

   case APU_WRE3:
	   {
		   dmc_t * dmc = &(APU->dmc);
		   dmc->regs[3] = value;
		   dmc->cached_dmalength = ((value << 4) + 1) << 3;
	   }
      break;

   case APU_SMASK:
	  {
		  dmc_t * dmc = &(APU->dmc);
		  triangle_t * tri = &(APU->triangle);
		  noise_t * noise = &(APU->noise);

		  dmc->enabled = (value & 0x10) ? TRUE : FALSE;
		  
		  APU->enable_reg = value;
		  
		  for (chan = 0; chan < 2; chan++)
		  {
			  if (value & (1 << chan))
				  APU->rectangle[chan].enabled = TRUE;
			  else
			  {
				  APU->rectangle[chan].enabled = FALSE;
				  APU->rectangle[chan].vbl_length = 0;
			  }
		  }
		  
		  if (value & 0x04)
			  tri->enabled = TRUE;
		  else
		  {
			  tri->enabled = FALSE;
			  tri->vbl_length = 0;
			  tri->linear_length = 0;
			  tri->counter_started = FALSE;
			  tri->write_latency = 0;
		  }
		  
		  if (value & 0x08)
			  noise->enabled = TRUE;
		  else
		  {
			  noise->enabled = FALSE;
			  noise->vbl_length = 0;
		  }
		  
		  if (value & 0x10)
		  {
			  if (0 == dmc->dma_length)
				  InfoNES_pAPUdmcreload(&APU->dmc);
		  }
		  else
			{
				dmc->dma_length = 0;	  
				dmc->irq_occurred = FALSE;
			}
	  }
      break;
   default: break;
   }
}

#endif



/*-------------------------------------------------------------------*/
/*     Simple queue routines                                         */
/*-------------------------------------------------------------------*/
#define  APU_QEMPTY()   (APU->q_head == APU->q_tail)

/*-------------------------------------------------------------------*/
/* emulation of the 15-bit shift register the												 */
/* NES uses to generate pseudo-random series												 */
/* for the white noise channel                                       */
/*-------------------------------------------------------------------*/
char shift_register15(unsigned char xor_tap)
{
   static int sreg = 0x4000;
   int bit0, tap, bit14;

   bit0 = sreg & 1;
   tap = (sreg & xor_tap) ? 1 : 0;
   bit14 = (bit0 ^ tap);
   sreg >>= 1;
   sreg |= (bit14 << 14);
   return (bit0 ^ 1);
}


/*-----------------------------------------------------------------------*/
/* RECTANGLE WAVE1                                                       */
/* ======================================================================*/
/* reg0: 0-3=volume, 4=envelope, 5=hold, 6-7=duty cycle                  */
/* reg1: 0-2=sweep shifts, 3=sweep inc/dec, 4-6=sweep length, 7=sweep on */
/* reg2: 8 bits of freq                                                  */
/* reg3: 0-2=high freq, 7-4=vbl length counter                           */
/*-----------------------------------------------------------------------*/
#define  APU_RECTANGLE_OUTPUT chan->output_vol
int InfoNES_pAPUrectangle(rectangle_t *chan)
{
   int output;
   int num_times;
   int total;

   APU_VOLUME_DECAY(chan->output_vol);

   if (FALSE == chan->enabled || chan->vbl_length <= 0)
      return APU_RECTANGLE_OUTPUT;

   /* vbl length counter */
   if (FALSE == chan->holdnote)
      chan->vbl_length --;

   // speed hacks. Rick.
   {
	   int env_phase = chan->env_phase;
	   int env_delay = chan->env_delay;
	   int holdnote = chan->holdnote;
	   int env_vol = chan->env_vol;
	   env_phase -= 4 ; /* 240/60 */
	   while (env_phase < 0)
	   {		 
		   env_phase += env_delay;
		   
		   if (holdnote)
			   env_vol = (env_vol + 1) & 0x0F;
		   else if (env_vol < 0x0F)
			   env_vol++;
	   }
	   chan->env_phase = env_phase;
	   chan->env_delay = env_delay;
	   chan->holdnote = holdnote;
	   chan->env_vol = env_vol;
   }

   /* TODO: using a table of max frequencies is not technically
   ** clean, but it is fast and (or should be) accurate 
   */
   if (chan->freq < 8 || (FALSE == chan->sweep_inc && chan->freq > chan->freq_limit))
      return APU_RECTANGLE_OUTPUT;

   /* frequency sweeping at a rate of (sweep_delay + 1) / 120 secs */
   if (chan->sweep_on && chan->sweep_shifts)
   {
      chan->sweep_phase -= 2 ; /* 120/60 */
      while (chan->sweep_phase < 0)
      {
         chan->sweep_phase += chan->sweep_delay;

         if (chan->sweep_inc) /* ramp up */
         {
            if (TRUE == chan->sweep_complement)
               chan->freq += ~(chan->freq >> chan->sweep_shifts);
            else
               chan->freq -= (chan->freq >> chan->sweep_shifts);
         }
         else /* ramp down */
         {
            chan->freq += (chan->freq >> chan->sweep_shifts);
         }
      }
   }

   chan->phaseacc -= APU->cycle_rate; /* # of cycles per sample */
   if (chan->phaseacc >= 0)
      return APU_RECTANGLE_OUTPUT;

   num_times = total = 0;

   if (chan->fixed_envelope)
      output = chan->volume << 8; /* fixed volume */
   else
      output = (chan->env_vol ^ 0x0F) << 8;

   while (chan->phaseacc < 0)
   {
      chan->phaseacc += APU_TO_FIXED(chan->freq + 1);
      chan->adder = (chan->adder + 1) & 0x0F;

      if (chan->adder < chan->duty_flip)
         total += output;
      else
         total -= output;

      num_times++;

   }
   chan->output_vol = total / num_times;

   return APU_RECTANGLE_OUTPUT;
}

/*-------------------------------------------------------------------*/
/* TRIANGLE WAVE2                                                    */
/* ==================================================================*/
/* reg0: 7=holdnote, 6-0=linear length counter                       */
/* reg2: low 8 bits of frequency                                     */
/* reg3: 7-3=length counter, 2-0=high 3 bits of frequency            */
/*-------------------------------------------------------------------*/
#define  InfoNES_pAPUTRIANGLE_OUTPUT  ((chan->output_vol*21)>>4)

int InfoNES_pAPUtriangle(triangle_t *chan)
{
   APU_VOLUME_DECAY(chan->output_vol);

   if (FALSE == chan->enabled || chan->vbl_length <= 0)
      return InfoNES_pAPUTRIANGLE_OUTPUT;

   if (chan->counter_started)
   {
      if (chan->linear_length > 0)
         chan->linear_length -= 4 ;  /* 240/60 */
      if (chan->vbl_length  && FALSE == chan->holdnote)
         chan->vbl_length --;
   }
   else if (FALSE == chan->holdnote && chan->write_latency)
   {
      if (--chan->write_latency == 0)
         chan->counter_started = TRUE;
   }

   if (chan->linear_length <= 0 || chan->freq < APU_TO_FIXED(4)) /* inaudible */
      return InfoNES_pAPUTRIANGLE_OUTPUT;

   chan->phaseacc -= APU->cycle_rate; /* # of cycles per sample */
   while (chan->phaseacc < 0)
   {
      chan->phaseacc += chan->freq;
      chan->adder = (chan->adder + 1) & 0x1F;

      if (chan->adder & 0x10)
         chan->output_vol -= (2 << 8);
      else
         chan->output_vol += (2 << 8);
   }

   return InfoNES_pAPUTRIANGLE_OUTPUT;
}

/*-------------------------------------------------------------------*/
/* WHITE NOISE CHANNEL                                               */
/* ==================================================================*/
/* reg0: 0-3=volume, 4=envelope, 5=hold                              */
/* reg2: 7=small(93 NBYTE) sample,3-0=freq lookup                     */
/* reg3: 7-4=vbl length counter                                      */
/*-------------------------------------------------------------------*/
#define  InfoNES_pAPUNOISE_OUTPUT  ((chan->output_vol*13)>>4)

int InfoNES_pAPUnoise(noise_t *chan)
{
   int outvol;
   int num_times;
   int total;

   APU_VOLUME_DECAY(chan->output_vol);

   if (FALSE == chan->enabled || chan->vbl_length <= 0)
      return InfoNES_pAPUNOISE_OUTPUT;

   /* vbl length counter */
   if (FALSE == chan->holdnote)
      chan->vbl_length --;

   /* envelope decay at a rate of (env_delay + 1) / 240 secs */
   {
	   int env_phase = chan->env_phase;
	   int env_delay = chan->env_delay;
	   int holdnote = chan->holdnote;
	   int env_vol = chan->env_vol;
	   env_phase -= 4; /* 240/60 */
	   while (env_phase < 0)
	   {
		   env_phase += env_delay;
		   
		   if (holdnote)
			   env_vol = (env_vol + 1) & 0x0F;
		   else if (env_vol < 0x0F)
			   env_vol++;
	   }
	   chan->env_phase = env_phase;
	   chan->env_delay = env_delay;
	   chan->holdnote = holdnote;
	   chan->env_vol = env_vol;
   }


   chan->phaseacc -= APU->cycle_rate; /* # of cycles per sample */
   if (chan->phaseacc >= 0)
      return InfoNES_pAPUNOISE_OUTPUT;
   
   num_times = total = 0;

   if (chan->fixed_envelope)
      outvol = chan->volume << 8; /* fixed volume */
   else
      outvol = (chan->env_vol ^ 0x0F) << 8;

   while (chan->phaseacc < 0)
   {
      chan->phaseacc += chan->freq;

      if (shift_register15(chan->xor_tap))
         total += outvol;
      else
         total -= outvol;

      num_times++;
   }

   chan->output_vol = total / num_times;

   return InfoNES_pAPUNOISE_OUTPUT;
}


void InfoNES_pAPUdmcreload(dmc_t *chan)
{
   chan->address = chan->cached_addr;
   chan->dma_length = chan->cached_dmalength;
   chan->irq_occurred = TRUE;//FALSE;
}

/*----------------------------------------------------------------------*/
/* DELTA MODULATION CHANNEL                                             */
/* =====================================================================*/
/* reg0: 7=irq gen, 6=looping, 3-0=pointer to clock table							  */
/* reg1: output dc level, 6 bits unsigned                               */
/* reg2: 8 bits of 64-NBYTE aligned address offset : $C000 + (value * 64)*/
/* reg3: length, (value * 16) + 1                                       */
/*----------------------------------------------------------------------*/
//#define  InfoNES_pAPUDMC_OUTPUT (chan->output_vol - (chan->output_vol >> 3))
#define  APU_DMC_OUTPUT ((chan->output_vol*13)>>4)

int InfoNES_pAPUdmc(dmc_t *chan)
{
   int delta_bit;

   APU_VOLUME_DECAY(chan->output_vol);

   /* only process when channel is alive */
   if (chan->dma_length)
   {
      chan->phaseacc -= APU->cycle_rate; /* # of cycles per sample */
      
      while (chan->phaseacc < 0)
      {
         chan->phaseacc += chan->freq;
         
         if (0 == (chan->dma_length & 7))
         {
          chan->cur_byte =K6502_Read(chan->address);//chan->cur_byte = nes6502_getbyte(chan->address);

            if (0xFFFF == chan->address)
               chan->address = 0x8000;
            else
               chan->address++;
         }

         if (--chan->dma_length == 0)
         {
            /* if loop bit set, we're cool to retrigger sample */
            if (chan->looping)
               InfoNES_pAPUdmcreload(chan);
            else
            {
               /* check to see if we should generate an irq */
               if (chan->irq_gen)
               {
                  chan->irq_occurred = TRUE;

               }

               /* bodge for timestamp queue */
               chan->enabled = FALSE;
               break;
            }
         }
         delta_bit = (chan->dma_length & 7) ^ 7;

         /* positive delta */
         if (chan->cur_byte & (1 << delta_bit))
         {
            if (chan->regs[1] < 0x7D)
            {
               chan->regs[1] += 2;
               chan->output_vol += (2 << 8);
            }
         }
         /* negative delta */
         else            
         {
            if (chan->regs[1] > 1)
            {
               chan->regs[1] -= 2;
               chan->output_vol -= (2 << 8);
            }
         }
      }
   }

   return APU_DMC_OUTPUT;
}

/*-------------------------------------------------------------------*/
/*     Read from $4015                                               */
/*-------------------------------------------------------------------*/
unsigned char InfoNES_pAPURead4015(void)
{
  unsigned char value;

	value = 0;
	/* Return 1 in 0-5 bit pos if a channel is playing */
	if (APU->rectangle[0].enabled && APU->rectangle[0].vbl_length>0)
		 value |= 0x01;
	if (APU->rectangle[1].enabled && APU->rectangle[1].vbl_length>0)
		 value |= 0x02;
	if (APU->triangle.enabled && APU->triangle.vbl_length>0)
		 value |= 0x04;
	if (APU->noise.enabled && APU->noise.vbl_length>0)
		 value |= 0x08;

	/* bodge for timestamp queue */
	if (APU->dmc.enabled)
		 value |= 0x10;

	if (APU->dmc.irq_occurred)
		 value |= 0x80;

   return value;
}

/*-------------------------------------------------------------------*/
/*    InfoNES_pAPUprocess                                          	 */
/*-------------------------------------------------------------------*/
void InfoNES_pAPUprocess(short *buffer, int num_samples)
{
   static int prev_sample = 0;
   int next_sample, accum;
	 
		while (num_samples--)
		{
				accum = 0;
			  accum += InfoNES_pAPUrectangle(&APU->rectangle[0]);
			  accum += InfoNES_pAPUrectangle(&APU->rectangle[1]);
			  accum += InfoNES_pAPUtriangle(&APU->triangle);
			  accum += InfoNES_pAPUnoise(&APU->noise);
			  accum += InfoNES_pAPUdmc(&APU->dmc);

			 /* do any filtering */
			 if (APU_FILTER_NONE != APU->filter_type)
			 {
					next_sample = accum;

					if (APU_FILTER_LOWPASS == APU->filter_type)
					{
						 accum += prev_sample;
						 accum >>= 1;
					}
					else
               next_sample =
						 accum = (accum + accum + accum + prev_sample) >> 2;

					prev_sample = next_sample;
			 }

			 /* prevent clipping */
			 if (accum > 0x7FFF)
					accum = 0x7FFF;
			 else if (accum < -0x8000)
					accum = -0x8000;
   
         /* signed 16-bit output,双声道 */
            *buffer++ = (short) accum;
						*buffer++ = (short) accum;
					//*buffer++ = (accum >> 8) ^ 0x80;
		}
}

/*-------------------------------------------------------------------*/
/*    set the filter type                                          	*/
/*-------------------------------------------------------------------*/
void InfoNES_pAPUsetfilter(int filter_type)
{
   APU->filter_type = filter_type;
}



/*-------------------------------------------------------------------*/
/*    InfoNES_pAPUregInit                                          	 */
/*-------------------------------------------------------------------*/
#ifdef APU_REG_QUICK_ACCESS	
void InfoNES_pAPUregInit(void)
{
	InfoNES_pAPUregwrite4000(0x00);/*apureg $0x4000*/
	InfoNES_pAPUregwrite4001(0x00);/*apureg $0x4001*/
	InfoNES_pAPUregwrite4002(0x00);/*apureg $0x4002*/
	InfoNES_pAPUregwrite4003(0x00);/*apureg $0x4003*/
	InfoNES_pAPUregwrite4004(0x00);/*apureg $0x4004*/
	InfoNES_pAPUregwrite4005(0x00);/*apureg $0x4005*/
	InfoNES_pAPUregwrite4006(0x00);/*apureg $0x4006*/
	InfoNES_pAPUregwrite4007(0x00);/*apureg $0x4007*/
	InfoNES_pAPUregwrite4008(0x00);/*apureg $0x4008*/
	InfoNES_pAPUregwrite400A(0x00);/*apureg $0x400A*/
	InfoNES_pAPUregwrite400B(0x00);/*apureg $0x400B*/
	InfoNES_pAPUregwrite400C(0x00);/*apureg $0x400C*/
	InfoNES_pAPUregwrite400E(0x00);/*apureg $0x400E*/
	InfoNES_pAPUregwrite400F(0x00);/*apureg $0x400F*/
	InfoNES_pAPUregwrite4010(0x00);/*apureg $0x4010*/
	InfoNES_pAPUregwrite4011(0x00);/*apureg $0x4011*/
	InfoNES_pAPUregwrite4012(0x00);/*apureg $0x4012*/
	InfoNES_pAPUregwrite4013(0x00);/*apureg $0x4013*/
	InfoNES_pAPUregwrite4015(0x00);/*apureg $0x4015*/		
}
#endif

/*-------------------------------------------------------------------*/
/*   InfoNES_pAPUreset                                           		 */
/*-------------------------------------------------------------------*/
void InfoNES_pAPUreset(void)
{
	#ifndef APU_REG_QUICK_ACCESS	
   unsigned int address;
	#endif

   APU->q_head = APU->q_tail = 0;

	#ifdef APU_REG_QUICK_ACCESS	
		InfoNES_pAPUregInit();   	
	#else
		for (address = 0x4000; address <= 0x4013; address++)
		InfoNES_pAPUregwrite(address,0);
		InfoNES_pAPUregwrite(0x4015, 0x00);
	#endif

}

/*-------------------------------------------------------------------*/
/*   InfoNES_pAPUbuild_luts                                          */
/*-------------------------------------------------------------------*/
void InfoNES_pAPUbuild_luts(int num_samples)
{
   int i;

   /* lut used for enveloping and frequency sweeps */
   for (i = 0; i < 16; i++)
      APU->table.decay_lut[i] = num_samples * (i + 1);

   /* used for note length, based on vblanks and size of audio buffer */
   for (i = 0; i < 32; i++)
      APU->table.vbl_lut[i] = vbl_length[i] * num_samples;

   /* triangle wave channel's linear length table */
   for (i = 0; i < 128; i++)
      APU->table.trilength_lut[i] = (int) (0.25 * (i * num_samples));
}

static void InfoNES_pAPUsetactive(apu_t *active)
{
   APU = active;
}

/*-------------------------------------------------------------------*/
/*   InfoNES_pAPUsetparams                                           				 */
/*-------------------------------------------------------------------*/
void InfoNES_pAPUsetparams(int sample_rate, int refresh_rate)
{
   APU->sample_rate = sample_rate;
   APU->refresh_rate = refresh_rate;
   //APU->sample_bits = sample_bits;

   APU->num_samples = sample_rate / refresh_rate;
   //frag_size = frag_size; /* quell warnings ????*/

   /* turn into fixed point! */
   APU->cycle_rate = (int) (APU_BASEFREQ * 65536.0 / (float) sample_rate);

   /* build various lookup tables for apu */
   InfoNES_pAPUbuild_luts(APU->num_samples);

}


/*-------------------------------------------------------------------*/
/*     InfoNES_pAPUinit                                         						 */
/*-------------------------------------------------------------------*/
void InfoNES_pAPUinit(void)	
{
   /* set the stupid flag to tell difference between two rectangles */
   APU->rectangle[0].sweep_complement = TRUE;
   APU->rectangle[1].sweep_complement = FALSE;

   /* set the update APU */

   InfoNES_pAPUsetactive(APU);

	 InfoNES_pAPUsetparams(22050, 60);
	
   InfoNES_pAPUreset(); 

   InfoNES_pAPUsetfilter(APU_FILTER_LOWPASS);
	 
/*----------------------------------------------------------------------
     初始化音频外设
---------------------------------------------------------------------------*/    

  InfoNES_SoundOpen(0, 44100);
}

/*===================================================================*/
/*                                                                   */
/*     InfoNES_pAPUVsync() : Callback Function per Vsync             */
/*                                                                   */
/*===================================================================*/

void InfoNES_pAPUVsync(void)
{
	
	InfoNES_SoundOutput();

}

/*===================================================================*/
/*                                                                   */
/*            InfoNES_pAPUDone() : Finalize pApu                     */
/*                                                                   */
/*===================================================================*/

void InfoNES_pAPUDone(void)
{
  InfoNES_SoundClose();
}

/*===================================================================*/
/*                                                                   */
/*            InfoNES_pAPUInit() : IInfoNES_pAPUInit                     */
/*                                                                   */
/*===================================================================*/
void InfoNES_pAPUInit(void)
{
	InfoNES_pAPUinit();
}

