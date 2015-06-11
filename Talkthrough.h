#ifndef  __Talkthrough_DEFINED
#define __Talkthrough_DEFINED

//--------------------------------------------------------------------------//
// Header files																//
//--------------------------------------------------------------------------//
#include <sys\exception.h>
#include <cdefBF533.h>
#include <ccblkfn.h>
#include <sysreg.h>
#include <fract.h>
#include <filter.h>

//--------------------------------------------------------------------------//
// Symbolic constants														//
//--------------------------------------------------------------------------//
// addresses for Port B in Flash A
#define pFlashA_PortA_Dir	(volatile unsigned char *)0x20270006
#define pFlashA_PortA_Data	(volatile unsigned char *)0x20270004

// names for codec registers, used for iCodec1836TxRegs[]
#define DAC_CONTROL_1		0x0000
#define DAC_CONTROL_2		0x1000
#define DAC_VOLUME_0		0x2000
#define DAC_VOLUME_1		0x3000
#define DAC_VOLUME_2		0x4000
#define DAC_VOLUME_3		0x5000
#define DAC_VOLUME_4		0x6000
#define DAC_VOLUME_5		0x7000
#define ADC_0_PEAK_LEVEL	0x8000
#define ADC_1_PEAK_LEVEL	0x9000
#define ADC_2_PEAK_LEVEL	0xA000
#define ADC_3_PEAK_LEVEL	0xB000
#define ADC_CONTROL_1		0xC000
#define ADC_CONTROL_2		0xD000
#define ADC_CONTROL_3		0xE000

// names for slots in ad1836 audio frame
#define INTERNAL_ADC_L0			0
#define INTERNAL_ADC_L1			1
#define INTERNAL_ADC_R0			4
#define INTERNAL_ADC_R1			5
#define INTERNAL_DAC_L0			0
#define INTERNAL_DAC_L1			1
#define INTERNAL_DAC_L2			2
#define INTERNAL_DAC_R0			4
#define INTERNAL_DAC_R1			5
#define INTERNAL_DAC_R2			6

// size of array iCodec1836TxRegs and iCodec1836RxRegs
#define CODEC_1836_REGS_LENGTH	11

// SPI transfer mode
#define TIMOD_DMA_TX 0x0003

// SPORT0 word length
#define SLEN_32	0x001f

// DMA flow mode
#define FLOW_1	0x1000

//User Defined TAP lengths
#define echo_TAP_NUM 3500	//Sets the length of the simulated echo
#define lms_TAP_NUM 4000	//Sets the length of the adaptive lms filter - DO NOT EXCEED 4000

//--------------------------------------------------------------------------//
// Global variables															//
//--------------------------------------------------------------------------//
extern int iChannel0LeftIn;
extern int iChannel0RightIn;
extern int iChannel0LeftOut;
extern int iChannel0RightOut;
extern int iChannel1LeftIn;
extern int iChannel1RightIn;
extern int iChannel1LeftOut;
extern int iChannel1RightOut;
extern int iChannel2LeftOut;
extern int iChannel2RightOut;
extern volatile short sCodec1836TxRegs[];
extern volatile int iRxBuffer1[];
extern volatile int iTxBuffer1[];

extern volatile int ADC_flag;
extern volatile int freq_count;
extern volatile int x;
extern volatile int y;
extern volatile int y_lms;
extern volatile int output_error;
extern fract16 in_fir, out_fir;

extern volatile int echo_writer;
extern volatile int echo_reader_first, echo_reader_second, echo_reader_third;


extern fract16 *pointer;
extern fract16 *pointer_out;
extern fract16 *step_size_p;

extern short *sum_int_p;
extern fract16 *sum_fract_p;
extern short sum_int;
extern fract16 sum_fract;
extern float sum;
extern float meu;

extern fir_state_fr16 state;	// declare filter state

extern volatile fract16 echo_delayed_input[];
extern fract16 lms_delayed_input[];
extern fract16 lms_filter_coeff[];
extern fract16 echo_output;
extern fract16 step_size;
extern fract16 error_sig;
//--------------------------------------------------------------------------//
// Prototypes																//
//--------------------------------------------------------------------------//
// in file Initialisation.c
void Init_PLL(void);
void Init_EBIU(void);
void Init_Flash(void);
void Init1836(void);
void Init_Sport0(void);
void Init_DMA(void);
void Init_Sport_Interrupts(void);
void Enable_DMA_Sport(void);


// in file Process_data.c
void adder_unit(void);
void echo_generator(void);
void adaptive_lms(void);
void Init_Echo_Delayed_Input(void);
void Init_Lms_Delayed_Input(void);
void Init_Lms_Filter_Coeff(void);

// in file ISRs.c
EX_INTERRUPT_HANDLER(Sport0_RX_ISR);

#endif //__Talkthrough_DEFINED

