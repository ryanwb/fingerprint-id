// L138_LCDK_aic3106_init.c
//

#include "L138_LCDK_aic3106_init.h"

int16_t *pingIN, *pingOUT, *pongIN, *pongOUT;

AIC31_data_type AIC31_data;

typedef struct BITVAL    // used in function prbs()
{
 unsigned int b0:1, b1:1, b2:1, b3:1, b4:1, b5:1, b6:1;
 unsigned int b7:1, b8:1, b9:1, b10:1, b11:1, b12:1,b13:1;
 unsigned int dweebie:2; //Fills the 2 bit hole - bits 14-15
} bitval;

typedef union SHIFT_REG
{
 unsigned int regval;
 bitval bt;
} shift_reg;
int fb = 1;                        // feedback variable
shift_reg sreg = {0xFFFF};         // shift register

uint32_t prand_seed = 1;       // used in function prand()

extern void vectors(void);

void output_sample(int32_t out_data)
{
  AIC31_data.uint = out_data;
  if (poll) while (!CHKBIT(MCASP->SRCTL13, XRDY));
  MCASP->XBUF13 = AIC31_data.uint;
}
   
void output_left_sample(int16_t out_data)
{
  AIC31_data.uint = 0;
  AIC31_data.channel[LEFT]=out_data;
  if (poll) while (!CHKBIT(MCASP->SRCTL13, XRDY));
  MCASP->XBUF13 = AIC31_data.uint;
}

void output_right_sample(int16_t out_data)
{
  AIC31_data.uint = 0;
  AIC31_data.channel[RIGHT]=out_data;
  if (poll) while (!CHKBIT(MCASP->SRCTL13, XRDY));
  MCASP->XBUF13 = AIC31_data.uint;
}

int32_t input_sample(void)
{
  if (poll) while (!CHKBIT(MCASP->SRCTL14, RRDY));
  AIC31_data.uint = MCASP->XBUF14;
  return (AIC31_data.uint);
}

int16_t input_left_sample(void)
{
  if (poll) while (!CHKBIT(MCASP->SRCTL14, RRDY));
  AIC31_data.uint = MCASP->XBUF14;
  return (AIC31_data.channel[LEFT]);
}

int16_t input_right_sample(void)
{
  if (poll) while (!CHKBIT(MCASP->SRCTL14, RRDY));
  AIC31_data.uint = MCASP->XBUF14;
  return (AIC31_data.channel[RIGHT]);
}

   

void EDMA3_PaRAM_setup() // sets up PaRAM and enables EDMA events
{
uint32_t *EDMA3_PaRAM_ptr;
// PaRAM sets (channels) 0 and 1, triggered by McASP0
// receive and transmit events respectively, are used
// with linked parameters set up in PaRAM sets (channels) 64, 65, 67, and 68
// the linked parameters differ only in the ping or pong buffers used

// set up PaRAM set 1 (MCASP0 XEVT)

EDMA3_PaRAM_ptr = (unsigned int *)(0x01C04020);

*EDMA3_PaRAM_ptr++ = 0x00000000; // no EDMA3 interrupts on transfer complete
*EDMA3_PaRAM_ptr++ = (unsigned int)pingOUT; // source is one of two output buffers
*EDMA3_PaRAM_ptr++ = (int32_t)((((BUFCOUNT/2)<<16) & 0xFFFF0000) | 0x00000004);
 // BCNT = BUFCOUNT/2(arrays per frame), ACNT = 4(bytes per array) writing to a (32-bit) McASP0 serialiser
*EDMA3_PaRAM_ptr++ = 0x01D02000; // destination is DMA port of McASP0
*EDMA3_PaRAM_ptr++ = 0x00000004; // dest address is static, increment source address by ACNT bytes
*EDMA3_PaRAM_ptr++ = 0x00000800; // BCNT reload value is redundant - link to PaRAM set 64
*EDMA3_PaRAM_ptr++ = 0x00000000; // CCNTIDX
*EDMA3_PaRAM_ptr++ = 0x00000001; // rsvd, CCNT = 1(frame per block)

// set up PaRAM set 64

EDMA3_PaRAM_ptr = (unsigned int *)(0x01C04800);

*EDMA3_PaRAM_ptr++ = 0x00000000; // same as PaRAM set 1
*EDMA3_PaRAM_ptr++ = (unsigned int)pongOUT; // apart from source buffer used
*EDMA3_PaRAM_ptr++ = (int32_t)((((BUFCOUNT/2)<<16) & 0xFFFF0000) | 0x00000004);
*EDMA3_PaRAM_ptr++ = 0x01D02000; 
*EDMA3_PaRAM_ptr++ = 0x00000004; 
*EDMA3_PaRAM_ptr++ = 0x00000820; // BCNT reload value is redundant - link to PaRAM set 65
*EDMA3_PaRAM_ptr++ = 0x00000000;  
*EDMA3_PaRAM_ptr++ = 0x00000001; 

// set up PaRAM set 65

EDMA3_PaRAM_ptr = (unsigned int *)(0x01C04820);

*EDMA3_PaRAM_ptr++ = 0x00000000; // exactly the same as PaRAM set 1
*EDMA3_PaRAM_ptr++ = (unsigned int)pingOUT; 
*EDMA3_PaRAM_ptr++ = (int32_t)((((BUFCOUNT/2)<<16) & 0xFFFF0000) | 0x00000004);
*EDMA3_PaRAM_ptr++ = 0x01D02000; 
*EDMA3_PaRAM_ptr++ = 0x00000004; 
*EDMA3_PaRAM_ptr++ = 0x00000800; // BCNT reload value is redundant - link to PaRAM set 64
*EDMA3_PaRAM_ptr++ = 0x00000000;  
*EDMA3_PaRAM_ptr++ = 0x00000001; 


// set up PaRAM set 0 (MCASP0 REVT)

EDMA3_PaRAM_ptr = (unsigned int *)(0x01C04000);

*EDMA3_PaRAM_ptr++ = 0x00100000; // OPT interrupt on transfer complete with TCC = 0
*EDMA3_PaRAM_ptr++ = 0x01D02000; // SRC DMA port of McASP0
*EDMA3_PaRAM_ptr++ = (int32_t)((((BUFCOUNT/2)<<16) & 0xFFFF0000) | 0x00000004);
*EDMA3_PaRAM_ptr++ = (unsigned int)pingIN; // DST one of two output buffers
*EDMA3_PaRAM_ptr++ = 0x00040000; // DSTBIDX, SRCBIDX increment dest by 4 bytes, src is static
*EDMA3_PaRAM_ptr++ = 0x00000860; // BCNT reload value is redundant - link to PaRAM set 67
*EDMA3_PaRAM_ptr++ = 0x00000000; // 
*EDMA3_PaRAM_ptr++ = 0x00000001; // rsvd, CCNT 1 frame per block

// set up parameter RAM set 67

EDMA3_PaRAM_ptr = (unsigned int *)(0x01C04860);

*EDMA3_PaRAM_ptr++ = 0x00101000; // OPT similar to PaRAM set 1 but TCC = 1 and different buffer used
*EDMA3_PaRAM_ptr++ = 0x01D02000;    // SRC
*EDMA3_PaRAM_ptr++ = (int32_t)((((BUFCOUNT/2)<<16) & 0xFFFF0000) | 0x00000004);
*EDMA3_PaRAM_ptr++ = (unsigned int)pongIN; // DST - single address used for XBUFn, RBUFn if RBSEL = 0
*EDMA3_PaRAM_ptr++ = 0x00040000; // DSTBIDX, SRCBIDX
*EDMA3_PaRAM_ptr++ = 0x00000880; // BCNT reload value is redundant - link to PaRAM set 68
*EDMA3_PaRAM_ptr++ = 0x00000000; // 
*EDMA3_PaRAM_ptr++ = 0x00000001; // rsvd, CCNT

// set up parameter RAM set 68

EDMA3_PaRAM_ptr = (unsigned int *)(0x01C04880); // exactly the same as PaRAM set 1

*EDMA3_PaRAM_ptr++ = 0x00100000; // OPT
*EDMA3_PaRAM_ptr++ = 0x01D02000;    // SRC
*EDMA3_PaRAM_ptr++ = (int32_t)((((BUFCOUNT/2)<<16) & 0xFFFF0000) | 0x00000004);
*EDMA3_PaRAM_ptr++ = (unsigned int)pingIN; // DST - single address used for XBUFn, RBUFn if RBSEL = 0
*EDMA3_PaRAM_ptr++ = 0x00040000; // DSTBIDX, SRCBIDX
*EDMA3_PaRAM_ptr++ = 0x00000860; // BCNT reload value is redundant - link to PaRAM set 67
*EDMA3_PaRAM_ptr++ = 0x00000000; // 
*EDMA3_PaRAM_ptr++ = 0x00000001; // rsvd, CCNT


  EDMA_3CC_IECRH  = 0xffffffff;   // IERH - Disable high interrupts
  EDMA_3CC_EECRH  = 0xffffffff;   // EERH - Disable high events
  EDMA_3CC_ICRH   = 0xffffffff;   // ICRH - Clear high interrupts
  EDMA_3CC_ECRH   = 0xffffffff;   // ICRH - Clear high events

  EDMA_3CC_IECR   = 0xffffffff;   // IER  - Disable low interrupts
  EDMA_3CC_EECR   = 0xffffffff;   // EER  - Disable low events
  EDMA_3CC_ICR    = 0xffffffff;   // ICR  - Clear low interrupts
  EDMA_3CC_ECR    = 0xffffffff;   // ICRH - Clear low events


EDMA_3CC_EESR = 0x00000003; // enable EDMA3 events 0 and 1, i.e. McASP REVT and XEVT
}

//configure and initialise MCASP0 for edma3-based i/o
void L138_init_mcasp_edma()
{

   // reset mcasp.
   MCASP->GBLCTL  = 0;

   // configure receive registers.
   MCASP->RMASK      = 0xFFFFFFFF;
   MCASP->RFMT       = 0x000080F0;
   MCASP->AFSRCTL    = 0x00000000;
   MCASP->ACLKRCTL   = 0x00000000;
   MCASP->AHCLKRCTL  = 0x00000000;
   MCASP->RTDM       = 0x00000001; // TDM timeslot 0
   MCASP->RINTCTL    = 0x00000000;
   MCASP->RCLKCHK    = 0x00000000;

   // configure transmit registers.
   MCASP->XMASK      = 0xFFFFFFFF;
   MCASP->XFMT       = 0x000080F0;
   MCASP->AFSXCTL    = 0x00000000;
   MCASP->ACLKXCTL   = 0x00000000;
   MCASP->AHCLKXCTL  = 0x00000000;
   MCASP->XTDM       = 0x00000001; // TDM timeslot 0
   MCASP->XINTCTL    = 0x00000000;
   MCASP->XCLKCHK    = 0x00000000;
   // config serializers (13 = xmit, 14 = rcv).
   MCASP->SRCTL13    = 0x000D;
   MCASP->SRCTL14    = 0x000E;

   // config pin function and direction.
   MCASP->PFUNC      = 0;
   MCASP->PDIR       = 0x00002000;

   //
   MCASP->DITCTL     = 0x00000000;
   MCASP->DLBCTL     = 0x00000000;
   MCASP->AMUTE      = 0x00000000;


   // enable the audio clocks, verifying each bit is properly set.
   SETBIT(MCASP->XGBLCTL, XHCLKRST);
   while (!CHKBIT(MCASP->XGBLCTL, XHCLKRST)) {}
   SETBIT(MCASP->RGBLCTL, RHCLKRST);
   while (!CHKBIT(MCASP->RGBLCTL, RHCLKRST)) {}


     EDMA3_PaRAM_setup(); //this is a key difference to L138_init_mcasp_intr()

   MCASP->XSTAT = 0x0000FFFF;        // Clear all
   MCASP->RSTAT = 0x0000FFFF;        // Clear all

// if DMA is being used, verify that the XDATA bit (5) in XSTAT
// is cleared to 0. (it should have been cleared y writing 1 to it)
   while ( ( MCASP->XSTAT & 0x00000020 ) != 0x00000000 );

   SETBIT(MCASP->XGBLCTL, XSRCLR);
   while (!CHKBIT(MCASP->XGBLCTL, XSRCLR)) {}
   SETBIT(MCASP->RGBLCTL, RSRCLR);
   while (!CHKBIT(MCASP->RGBLCTL, RSRCLR)) {}

   /* Write a 0, so that no underrun occurs after releasing the state machine */
   MCASP->XBUF13 = 0;
   MCASP->RBUF14 = 0;

   SETBIT(MCASP->XGBLCTL, XSMRST);
   while (!CHKBIT(MCASP->XGBLCTL, XSMRST)) {}
   SETBIT(MCASP->RGBLCTL, RSMRST);
   while (!CHKBIT(MCASP->RGBLCTL, RSMRST)) {}

   SETBIT(MCASP->XGBLCTL, XFRST);
   while (!CHKBIT(MCASP->XGBLCTL, XFRST)) {}
   SETBIT(MCASP->RGBLCTL, RFRST);
   while (!CHKBIT(MCASP->RGBLCTL, RFRST)) {}

   // wait for transmit ready and send a dummy byte.
   while(!CHKBIT(MCASP->SRCTL13, XRDY)) {}
   MCASP->XBUF13 = 0;
}


// configure and initialise McASP0 for interrupt-based i/o

void L138_init_mcasp_intr()
{

   // globally reset McASP0
   MCASP->GBLCTL  = 0;

   // configure McASP0 receive registers
   MCASP->RMASK      = 0xFFFFFFFF;
   MCASP->RFMT       = 0x000080F8; 
   MCASP->AFSRCTL    = 0x00000000;
   MCASP->ACLKRCTL   = 0x00000000;
   MCASP->AHCLKRCTL  = 0x00000000;
   MCASP->RTDM       = 0x00000001; // TDM timeslot 0
   MCASP->RINTCTL    = 0x00000000;
   MCASP->RCLKCHK    = 0x00000000;

   // configure McASP0 transmit registers
   MCASP->XMASK      = 0xFFFFFFFF;
   MCASP->XFMT       = 0x000080F8;
   MCASP->AFSXCTL    = 0x00000000;
   MCASP->ACLKXCTL   = 0x00000000;
   MCASP->AHCLKXCTL  = 0x00000000;
   MCASP->XTDM       = 0x00000001; // TDM timeslot 0
   MCASP->XINTCTL    = 0x00000020; // interrupt on transmit
   MCASP->XCLKCHK    = 0x00000000;
   // configure serialisers (13 for xmit, 14 for rcv)
   MCASP->SRCTL13    = 0x000D;
   MCASP->SRCTL14    = 0x000E;

   // config pin function and direction.
   MCASP->PFUNC      = 0;
   MCASP->PDIR       = 0x00002000;

   //
   MCASP->DITCTL     = 0x00000000;
   MCASP->DLBCTL     = 0x00000000;
   MCASP->AMUTE      = 0x00000000;


   // enable the audio clocks, verifying each bit is properly set.
   SETBIT(MCASP->XGBLCTL, XHCLKRST);
   while (!CHKBIT(MCASP->XGBLCTL, XHCLKRST)) {}
   SETBIT(MCASP->RGBLCTL, RHCLKRST);
   while (!CHKBIT(MCASP->RGBLCTL, RHCLKRST)) {}

   MCASP->XSTAT = 0x0000FFFF;        // Clear all
   MCASP->RSTAT = 0x0000FFFF;        // Clear all


   SETBIT(MCASP->XGBLCTL, XSRCLR);
   while (!CHKBIT(MCASP->XGBLCTL, XSRCLR)) {}
   SETBIT(MCASP->RGBLCTL, RSRCLR);
   while (!CHKBIT(MCASP->RGBLCTL, RSRCLR)) {}

   // Write zero to serialiser buffers so that no underrun occurs after releasing the state machine
   MCASP->XBUF13 = 0;
   MCASP->RBUF14 = 0;

   SETBIT(MCASP->XGBLCTL, XSMRST);
   while (!CHKBIT(MCASP->XGBLCTL, XSMRST)) {}
   SETBIT(MCASP->RGBLCTL, RSMRST);
   while (!CHKBIT(MCASP->RGBLCTL, RSMRST)) {}

   SETBIT(MCASP->XGBLCTL, XFRST);
   while (!CHKBIT(MCASP->XGBLCTL, XFRST)) {}
   SETBIT(MCASP->RGBLCTL, RFRST);
   while (!CHKBIT(MCASP->RGBLCTL, RFRST)) {}

   // wait for transmit ready and send a dummy byte
//   while(!CHKBIT(MCASP->SRCTL13, XRDY)) {}
//   MCASP->XBUF13 = 0;

}

// configure and initialise McASP0 for polling-based i/o

void L138_init_mcasp_poll()
{

   // globally reset McASP0
   MCASP->GBLCTL  = 0;

   // configure receive registers.
   MCASP->RMASK      = 0xFFFFFFFF;
   MCASP->RFMT       = 0x000080F8; 
   MCASP->AFSRCTL    = 0x00000000;
   MCASP->ACLKRCTL   = 0x00000000;
   MCASP->AHCLKRCTL  = 0x00000000;
   MCASP->RTDM       = 0x00000001;
   MCASP->RINTCTL    = 0x00000000;
   MCASP->RCLKCHK    = 0x00000000;

   // configure transmit registers.
   MCASP->XMASK      = 0xFFFFFFFF;
   MCASP->XFMT       = 0x000080F8;
   MCASP->AFSXCTL    = 0x00000000;
   MCASP->ACLKXCTL   = 0x00000000;
   MCASP->AHCLKXCTL  = 0x00000000;
   MCASP->XTDM       = 0x00000001;
   MCASP->XINTCTL    = 0x00000000;
   MCASP->XCLKCHK    = 0x00000000;
   // config serializers (13 = xmit, 14 = rcv).
   MCASP->SRCTL13    = 0x000D;
   MCASP->SRCTL14    = 0x000E;

   // config pin function and direction.
   MCASP->PFUNC      = 0;
   MCASP->PDIR       = 0x00002000;

   //
   MCASP->DITCTL     = 0x00000000;
   MCASP->DLBCTL     = 0x00000000;
   MCASP->AMUTE      = 0x00000000;


   // enable the audio clocks, verifying each bit is properly set.
   SETBIT(MCASP->XGBLCTL, XHCLKRST);
   while (!CHKBIT(MCASP->XGBLCTL, XHCLKRST)) {}
   SETBIT(MCASP->RGBLCTL, RHCLKRST);
   while (!CHKBIT(MCASP->RGBLCTL, RHCLKRST)) {}

   MCASP->XSTAT = 0x0000FFFF;        // Clear all
   MCASP->RSTAT = 0x0000FFFF;        // Clear all


   SETBIT(MCASP->XGBLCTL, XSRCLR);
   while (!CHKBIT(MCASP->XGBLCTL, XSRCLR)) {}
   SETBIT(MCASP->RGBLCTL, RSRCLR);
   while (!CHKBIT(MCASP->RGBLCTL, RSRCLR)) {}

   /* Write a 0, so that no underrun occurs after releasing the state machine */
   MCASP->XBUF13 = 0;
   MCASP->RBUF14 = 0;

   SETBIT(MCASP->XGBLCTL, XSMRST);
   while (!CHKBIT(MCASP->XGBLCTL, XSMRST)) {}
   SETBIT(MCASP->RGBLCTL, RSMRST);
   while (!CHKBIT(MCASP->RGBLCTL, RSMRST)) {}

   SETBIT(MCASP->XGBLCTL, XFRST);
   while (!CHKBIT(MCASP->XGBLCTL, XFRST)) {}
   SETBIT(MCASP->RGBLCTL, RFRST);
   while (!CHKBIT(MCASP->RGBLCTL, RFRST)) {}

   // wait for transmit ready and send a dummy byte.
   while(!CHKBIT(MCASP->SRCTL13, XRDY)) {}
   MCASP->XBUF13 = 0;

}


// initialise AIC3106 codec by writing to its control registers
// sampling frequency, ADC gain, and DAC attenuation are set according to parameters passed


void L138_init_aic3106_registers(int32_t fs, int16_t adc_gain, int16_t dac_atten, int8_t input_type)
{
// not every register is set here - some are left with default(reset) values
// each AIC3106 register is 8 bits wide

  AIC3106_writeRegister(  0, 0x00 );  // AIC3106_PAGESELECT is PAGE 0 REG 0
  AIC3106_writeRegister(  1, 0x80 );  // reset AIC3106 using REG 1

switch(fs) // set up sampling rate - default 8kHz
{
	case FS_8000_HZ:
	  AIC3106_writeRegister(  2, 0xAA );
      AIC3106_writeRegister(  7, 0x0A );  // Codec Datapath Setup [LeftDAC=LEFT][RightDAC=RIGHT]
	  break;
	case FS_9600_HZ:
	  AIC3106_writeRegister(  2, 0x88 );
      AIC3106_writeRegister(  7, 0x0A );  // Codec Datapath Setup [LeftDAC=LEFT][RightDAC=RIGHT]
	  break;
	case FS_11025_HZ:
	  AIC3106_writeRegister(  2, 0x66 );
      AIC3106_writeRegister(  7, 0x8A );  // Codec Datapath Setup [LeftDAC=LEFT][RightDAC=RIGHT]
	  break;
	case FS_12000_HZ:
	  AIC3106_writeRegister(  2, 0x66 );
      AIC3106_writeRegister(  7, 0x0A );  // Codec Datapath Setup [LeftDAC=LEFT][RightDAC=RIGHT]
	  break;
	case FS_16000_HZ:
	  AIC3106_writeRegister(  2, 0x44 );
      AIC3106_writeRegister(  7, 0x0A );  // Codec Datapath Setup [LeftDAC=LEFT][RightDAC=RIGHT]
	  break;
	case FS_19200_HZ:
	  AIC3106_writeRegister(  2, 0x33 );
      AIC3106_writeRegister(  7, 0x0A );  // Codec Datapath Setup [LeftDAC=LEFT][RightDAC=RIGHT]
	  break;
	case FS_22050_HZ:
	  AIC3106_writeRegister(  2, 0x22 );
      AIC3106_writeRegister(  7, 0x8A );  // Codec Datapath Setup [LeftDAC=LEFT][RightDAC=RIGHT]
	  break;
	case FS_24000_HZ:
	  AIC3106_writeRegister(  2, 0x22 );
      AIC3106_writeRegister(  7, 0x0A );  // Codec Datapath Setup [LeftDAC=LEFT][RightDAC=RIGHT]
	  break;
	case FS_32000_HZ:
	  AIC3106_writeRegister(  2, 0x11 );
      AIC3106_writeRegister(  7, 0x0A );  // Codec Datapath Setup [LeftDAC=LEFT][RightDAC=RIGHT]
	  break;
	case FS_44100_HZ:
	  AIC3106_writeRegister(  2, 0x00 );
      AIC3106_writeRegister(  7, 0x8A );  // Codec Datapath Setup [LeftDAC=LEFT][RightDAC=RIGHT]
	  break;
	case FS_48000_HZ:
	  AIC3106_writeRegister(  2, 0x00 );
      AIC3106_writeRegister(  7, 0x0A );  // Codec Datapath Setup [LeftDAC=LEFT][RightDAC=RIGHT]
	  break;
	default:
	  AIC3106_writeRegister(  2, 0xAA );
      AIC3106_writeRegister(  7, 0x0A );  // Codec Datapath Setup [LeftDAC=LEFT][RightDAC=RIGHT]
	  break;
}
	  
  AIC3106_writeRegister(  3, 0x22 );  // disable PLL and set Q=4(relevant) and P=2(irrelevant)
                                      // fsref will be MCLK/(128*Q) = 48000
                                      // MCLK on eXperimenter is 24.576MHz from on-board Citizen oscillator  
  AIC3106_writeRegister(  4, 0x20 );  // J=8 (irrelevant since PLL disabled)
  AIC3106_writeRegister(  5, 0x6E );  // D=7075 MS bits (irrelevant since PLL disabled)
  AIC3106_writeRegister(  6, 0x23 );  // D=7075 LS bits (irrelevant since PLL disabled)
  AIC3106_writeRegister(  8, 0xC0 );  // Audio Interface Control A BCLK=Slave(input), WCLK=Slave(input)
  AIC3106_writeRegister(  9, 0x40 );  // Audio Interface Control B DSP mode (1 slot), 32 bit slot width

  AIC3106_writeRegister( 12, 0x00 );  // Audio Interface Control C Data offset=0


  AIC3106_writeRegister( 10, 0x00 );  // Audio Interface Control C Data offset=0
// WARNING - this assumes that adc_gain is a sensible value
  AIC3106_writeRegister( 15, adc_gain );  // Left ADC Programmable Gain Amplifier Mute=OFF Gain=0dB
  AIC3106_writeRegister( 16, adc_gain );  // Right ADC Programmable Gain Amplifier Mute=OFF Gain=0dB

  if (input_type == LCDK_LINE_INPUT)
  {
    AIC3106_writeRegister( 19, 0x04 );  // power up ADCs
    AIC3106_writeRegister( 22, 0x04 );  // and connect LINE1L/R to ADC
  }
  else
  {
    AIC3106_writeRegister( 19, 0x7C );  // power up ADCs
    AIC3106_writeRegister( 22, 0x7C );  // and do not connect LINE1L/R to ADC
    AIC3106_writeRegister( 17, 0x0F );  // 17 MIC3L to L ADC (default 0xFF, NC) 0x00 0dB
    AIC3106_writeRegister( 18, 0xF0 );  // 18 MIC3R to R ADC (default 0xFF, NC) 0x00 0dB
    AIC3106_writeRegister( 25, 0x40 );  // 25 MICBIAS 0x40 2V, 0x00 OFF
  }

  AIC3106_writeRegister( 37, 0xE0 );  // DAC Power & Output Dvr LeftDAC=ON, RightDAC=ON, HPLCOM=SingleEnd]
// WARNING - this assumes that dac_atten is a sensible value
  AIC3106_writeRegister( 43, dac_atten );  // Left DAC Digital Volume Mute=OFF, Gain=0dB
  AIC3106_writeRegister( 44, dac_atten );  // Right DAC Digital Volume Mute=OFF, Gain=0dB
  AIC3106_writeRegister( 82, 0x80 );  // was 0x80 DAC_L1 to LEFT_LOP/M Volume Routed, Gain=0dB
  AIC3106_writeRegister( 86, 0x09 );  // LEFT_LOP/M Output Mute=OFF bit3 bit0 is REad only ?! must be set!
  AIC3106_writeRegister( 92, 0x80 );  // was 0x80 92 DAC_R1 to RIGHT_LOP/M Volume    <- [Routed]
  AIC3106_writeRegister( 93, 0x09 );  // 93           RIGHT_LOP/M Output    <- [Mute=OFF][Power=ON]
  AIC3106_writeRegister( 101, 0x01 ); // 101 GPIO Control Register B        <- [CODEC_CLKIN = CLKDIV_OUT]
  AIC3106_writeRegister( 102, 0 );
}

void L138_init_LCDK_DIP()
{
  GPIO_setDir(GPIO_BANK0, GPIO_PIN1, GPIO_INPUT);
  GPIO_setDir(GPIO_BANK0, GPIO_PIN2, GPIO_INPUT);
  GPIO_setDir(GPIO_BANK0, GPIO_PIN3, GPIO_INPUT);
  GPIO_setDir(GPIO_BANK0, GPIO_PIN4, GPIO_INPUT);
  return;
}

uint8_t read_LCDK_user_DIP()
{
  uint8_t DIP_1, DIP_2, DIP_3, DIP_4;

  GPIO_getInput(GPIO_BANK0, GPIO_PIN4, &DIP_4);
  GPIO_getInput(GPIO_BANK0, GPIO_PIN3, &DIP_3);
  GPIO_getInput(GPIO_BANK0, GPIO_PIN2, &DIP_2);
  GPIO_getInput(GPIO_BANK0, GPIO_PIN1, &DIP_1);
  return((uint8_t)(DIP_1 + 2*DIP_2 + 4*DIP_3 + 8*DIP_4));
}

// initialisation routine called by example program for edma3-based i/o

void L138_initialise_edma(int32_t fs, int16_t adc_gain, int16_t dac_atten, int8_t input_type)
{

  poll = 0;     // used in input_sample() and output_sample() functions


  // allocate memory for ping pong buffers
  pingOUT = (int16_t *)malloc(BUFCOUNT*sizeof(int16_t));
  pongOUT = (int16_t *)malloc(BUFCOUNT*sizeof(int16_t));
  pingIN = (int16_t *)malloc(BUFCOUNT*sizeof(int16_t));
  pongIN = (int16_t *)malloc(BUFCOUNT*sizeof(int16_t));

  USTIMER_init();

  EVMOMAPL138_lpscTransition(PSC1, DOMAIN0, LPSC_MCASP0, PSC_ENABLE);
  EVMOMAPL138_pinmuxConfig(PINMUX_MCASP_REG_0, PINMUX_MCASP_MASK_0, PINMUX_MCASP_VAL_0);
  EVMOMAPL138_pinmuxConfig(PINMUX_MCASP_REG_1, PINMUX_MCASP_MASK_1, PINMUX_MCASP_VAL_1);

  // replaces I2C_init();
  *( volatile uint32_t* )(0x01C22024) = 0;
  *( volatile uint32_t* )(0x01C22030) = 2;
  *( volatile uint32_t* )(0x01C2200C) = 5;
  *( volatile uint32_t* )(0x01C22010) = 5;
  *( volatile uint32_t* )(0x01C22024) |= 0x20;
 
  CSR = 0x0000; // disable interrupts globally while initialising
                // GIE is bit 0 of CSR register

  L138_init_aic3106_registers(fs, adc_gain, dac_atten,input_type);

  L138_init_mcasp_edma();

  // clear any pending interrupts within EDMA3CC
  EDMA_3CC_ICR = 0x0007; // writing to ICR affects IPR
  // associate event n with interrupt 4 by writing n into LSBs of INTMUX1
  // LSBs of INTMUX1 are literally associated with INT4
  // in this case n=8 (EDMA3CC   event)
  INTC_INTMUX1 = 0x00000008;

  ISTP = (unsigned int)vectors;
  // enable TCC = 0 and TCC = 1 EDMA3 interrupts by setting bits 0 and 1 in IESR
  EDMA3_IESR = 0x0003;
  EDMA3_DRAE1 = 0x0003;

  // clear all pending interrupt flags
  // interrupt clear register ICR is used to clear bits in interrupt flag register IFR
  ICR = 0xFFF0;  // ICR bits 3, 2, 1, and 0 are reserved, read as 0, write has no effect
  IER |= 0x12;  //enable NMI (bit 1) and INT4 (bit 4)
  
  CSR |= 0x01;  // enable interrupts globally
  L138_init_LCDK_DIP();

}

// initialisation routine called by example program for polling-based i/o

void L138_initialise_poll(int32_t fs, int16_t adc_gain, int16_t dac_atten, int8_t input_type)
{
  
  poll = 1;     // used in input_sample() and output_sample() functions

  USTIMER_init();

  EVMOMAPL138_lpscTransition(PSC1, DOMAIN0, LPSC_MCASP0, PSC_ENABLE);
  EVMOMAPL138_pinmuxConfig(PINMUX_MCASP_REG_0, PINMUX_MCASP_MASK_0, PINMUX_MCASP_VAL_0);
  EVMOMAPL138_pinmuxConfig(PINMUX_MCASP_REG_1, PINMUX_MCASP_MASK_1, PINMUX_MCASP_VAL_1);

  *( volatile uint32_t* )(0x01C22024) = 0;
  *( volatile uint32_t* )(0x01C22030) = 2;
  *( volatile uint32_t* )(0x01C2200C) = 5;
  *( volatile uint32_t* )(0x01C22010) = 5;
  *( volatile uint32_t* )(0x01C22024) |= 0x20;

  CSR = 0x0000; // disable interrupts globally while initialising
                // GIE is bit 0 of CSR register 

  L138_init_aic3106_registers(fs, adc_gain, dac_atten, input_type);

  L138_init_mcasp_poll();

  ICR = 0xFFF0; 
  L138_init_LCDK_DIP();
}

// initialisation routine called by example program for interrupt-based i/o

void L138_initialise_intr(int32_t fs, int16_t adc_gain, int16_t dac_atten, int8_t input_type)
{

  poll = 0;     // used in input_sample() and output_sample() functions

  USTIMER_init();

  EVMOMAPL138_lpscTransition(PSC1, DOMAIN0, LPSC_MCASP0, PSC_ENABLE);
  EVMOMAPL138_pinmuxConfig(PINMUX_MCASP_REG_0, PINMUX_MCASP_MASK_0, PINMUX_MCASP_VAL_0);
  EVMOMAPL138_pinmuxConfig(PINMUX_MCASP_REG_1, PINMUX_MCASP_MASK_1, PINMUX_MCASP_VAL_1);

  *( volatile uint32_t* )(0x01C22024) = 0;
  *( volatile uint32_t* )(0x01C22030) = 2;
  *( volatile uint32_t* )(0x01C2200C) = 5;
  *( volatile uint32_t* )(0x01C22010) = 5;
  *( volatile uint32_t* )(0x01C22024) |= 0x20;
 
  CSR = 0x0000; // disable interrupts globally while initialising
                // GIE is bit 0 of CSR register (CSR is special CCS
                // C compiler type cregister and given value in c6x.h)

  L138_init_aic3106_registers(fs, adc_gain, dac_atten, input_type);

  L138_init_mcasp_intr();

  // associate event n with interrupt 4 by writing n into LSBs of INTMUX1
  // LSBs of INTMUX1 are literally associated with INT4
  // in this case n=61 (MCASP0 event)
  INTC_INTMUX1 = 0x3d;

  ISTP = (unsigned int)vectors;

  // clear all pending interrupt flags
  // interrupt clear register ICR is used to clear bits in interrupt flag register IFR
  ICR = 0xFFF0;  // ICR bits 3, 2, 1, and 0 are reserved, read as 0, write has no effect
  IER |= 0x12;  //enable NMI (bit 1) and INT4 (bit 4)
  
  CSR |= 0x01;  // enable interrupts globally

  L138_init_LCDK_DIP();
}




int prbs(void) 			       //gen pseudo-random sequence {-1,1}
{
  int prnseq;			 
  if(sreg.bt.b0)
	prnseq = -NOISELEVEL;		           //scaled negative noise level
  else
	prnseq = NOISELEVEL;		           //scaled positive noise level
  fb =(sreg.bt.b0)^(sreg.bt.b1);   //XOR bits 0,1
  fb^=(sreg.bt.b11)^(sreg.bt.b13); //with bits 11,13 -> fb
  sreg.regval<<=1;
  sreg.bt.b0=fb;			       //close feedback path
  return prnseq;			       //return noise sequence value 
}



uint32_t rand31_next()
{
  uint32_t hi, lo;

  lo = 16807 * (prand_seed & 0xFFFF);
  hi = 16807 * (prand_seed >> 16);

  lo += (hi & 0x7FFF) << 16;
  lo += hi >> 15;

  if (lo > 0x7FFFFFFF) lo -= 0x7FFFFFFF;

  return(prand_seed = (uint32_t)lo);
}

int16_t prand()
{
return ((int16_t)(rand31_next()>>18)-4096);
}
