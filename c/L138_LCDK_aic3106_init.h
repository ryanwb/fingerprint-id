// L138_LCDK_aic3106_init.h
//

#include "stdio.h"
#include "stdlib.h"
#include "types.h"
#include "evmomapl138.h"
#include "evmomapl138_timer.h"
#include "evmomapl138_gpio.h"
#include "evmomapl138_i2c.h"
#include "evmomapl138_mcasp.h"
#include "evmomapl138_aic3106.h"

#define BUFCOUNT 128
#define PING 1
#define PONG 0
#define NOISELEVEL 8000

#define LCDK_LINE_INPUT 0
#define LCDK_MIC_INPUT 1

#define FS_8000_HZ 8000
#define FS_9600_HZ 9600
#define FS_11025_HZ 11025
#define FS_12000_HZ 12000
#define FS_16000_HZ 16000
#define FS_19200_HZ 19200
#define FS_22050_HZ 22050
#define FS_24000_HZ 24000
#define FS_32000_HZ 32000
#define FS_44100_HZ 44100
#define FS_48000_HZ 48000

#define ADC_GAIN_0DB 0
#define ADC_GAIN_3DB 6
#define ADC_GAIN_6DB 12
#define ADC_GAIN_9DB 18
#define ADC_GAIN_12DB 24
#define ADC_GAIN_15DB 30
#define ADC_GAIN_18DB 36
#define ADC_GAIN_21DB 42
#define ADC_GAIN_24DB 48
#define ADC_GAIN_27DB 54
#define ADC_GAIN_30DB 60
#define ADC_GAIN_33DB 66
#define ADC_GAIN_36DB 72
#define ADC_GAIN_39DB 78
#define ADC_GAIN_42DB 84
#define ADC_GAIN_45DB 90
#define ADC_GAIN_48DB 96
#define ADC_GAIN_51DB 102
 
#define DAC_ATTEN_0DB 0
#define DAC_ATTEN_3DB 6
#define DAC_ATTEN_6DB 12
#define DAC_ATTEN_9DB 18
#define DAC_ATTEN_12DB 24
#define DAC_ATTEN_15DB 30
#define DAC_ATTEN_18DB 36
#define DAC_ATTEN_21DB 42
#define DAC_ATTEN_24DB 48

#define EDMA_3CC_IECRH          *( unsigned int* )( 0x01c0105c )
#define EDMA_3CC_EECRH          *( unsigned int* )( 0x01c0102c )
#define EDMA_3CC_ICRH           *( unsigned int* )( 0x01c01074 )
#define EDMA_3CC_ECRH           *( unsigned int* )( 0x01c0100c )

#define EDMA_3CC_IECR           *( unsigned int* )( 0x01c01058 )
#define EDMA_3CC_EECR           *( unsigned int* )( 0x01c01028 )
#define EDMA_3CC_EESR           *( unsigned int* )( 0x01c01030 )
#define EDMA_3CC_ICR            *( unsigned int* )( 0x01c01070 )
#define EDMA_3CC_IPR            *( unsigned int* )( 0x01c01068 )
#define EDMA_3CC_ECR            *( unsigned int* )( 0x01c01008 )
#define EDMA3_IESR              *( unsigned int* )( 0x01c01060 )
#define EDMA3_DRAE1             *( unsigned int* )( 0x01c00348 )
#define INTC_INTMUX1            *( unsigned int* )( 0x01800104 )
#define EVTCLR0                 *( unsigned int* )( 0x01080040 )
#define EVTCLR1                 *( unsigned int* )( 0x01080044 )

// pinmux defines for LCDK
// these are different from those used for eXperimenter
// due to use of McASP serializers 13 and 14 and different user DIP
// switch interface
#define PINMUX_MCASP_REG_0       (0)
#define PINMUX_MCASP_MASK_0      (0x0FFFFFFF)
#define PINMUX_MCASP_VAL_0       (0x01111111)
#define PINMUX_MCASP_REG_1       (1)
#define PINMUX_MCASP_MASK_1      (0x0FFFFFF0)
#define PINMUX_MCASP_VAL_1       (0x08888110)

#define LEFT  1
#define RIGHT 0

static int poll;

void L138_initialise_edma(int32_t fs, int16_t adc_gain, int16_t dac_atten, int8_t input_type);
void L138_initialise_poll(int32_t fs, int16_t adc_gain, int16_t dac_atten, int8_t input_type);
void L138_initialise_intr(int32_t fs, int16_t adc_gain, int16_t dac_atten, int8_t input_type);

uint8_t read_LCDK_user_DIP();
int32_t input_sample(void);
int16_t input_right_sample(void);
int16_t input_left_sample(void);
void output_sample(int32_t out_data);
void output_right_sample(int16_t out_data);
void output_left_sample(int16_t out_data);
int16_t prand();
int prbs();

typedef union
{
  uint32_t uint;
  short channel[2];
} AIC31_data_type;


extern	cregister volatile unsigned int	CSR;	// control status register
extern	cregister volatile unsigned int	ICR;	// interrupt clear register
extern	cregister volatile unsigned int	IER;	// interrupt enable register
extern	cregister volatile unsigned int	ISTP;	// interrupt 

