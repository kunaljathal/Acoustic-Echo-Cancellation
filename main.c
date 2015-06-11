/*-------------------------------------------------------------------------------------------------------
 --ECSE 494         Electrical Engineering Design Project
 --
 --Project Name:    Acoustic Echo Cancellation
 --
 --Authors:         Mr Kunal Jathal - kunal.jathal@mail.mcgill.ca,
 --                 Mr Asrar Rangwala - asrar.rangwala@mail.mcgill.ca
 --
 --Copyright (C) 2006 Deep Thought
 --Version 1.0
 --Date Modified:   May 9th, 2006
 --
 --File name:       main.c
 --
 --Software:		VisualDSP++3.5
 --Hardware:		ADSP-BF533 EZ-KIT Board
 --
 --Purpose:         Create an acoustic echo-cancellation system where a simulated
 --                 or a real-world produced echoed signal is minimized.
 ------------------------------------------------------------------------------------------------------*/

//Library and Header declarations--------------------
#include "sysreg.h"
#include "ccblkfn.h"
#include "fract.h"
#include "filter.h"
#include "stdio.h"
#include "flt2fr.h"
#include "Talkthrough.h"
//---------------------------------------------------


//Global Variables-----------------------------------

// array for registers to configure the ad1836 audio codec
// names are defined in "Talkthrough.h"
volatile short sCodec1836TxRegs[CODEC_1836_REGS_LENGTH] =
{
    DAC_CONTROL_1	| 0x010,
    DAC_CONTROL_2	| 0x000,
    DAC_VOLUME_0	| 0x3ff,
    DAC_VOLUME_1	| 0x3ff,
    DAC_VOLUME_2	| 0x3ff,
    DAC_VOLUME_3	| 0x3ff,
    DAC_VOLUME_4	| 0x3ff,
    DAC_VOLUME_5	| 0x3ff,
    ADC_CONTROL_1	| 0x000,
    ADC_CONTROL_2	| 0x1a0,
    ADC_CONTROL_3	| 0x000
    
};


int iChannel0LeftIn, iChannel1LeftIn; // left input data from ad1836
int iChannel0RightIn, iChannel1RightIn; // right input data from ad1836
int iChannel0LeftOut, iChannel1LeftOut, iChannel2LeftOut; // left ouput data for ad1836
int iChannel0RightOut, iChannel1RightOut, iChannel2RightOut; // right ouput data for ad1836
volatile int iTxBuffer1[8]; // SPORT0 DMA transmit buffer
volatile int iRxBuffer1[8]; // SPORT0 DMA receive buffer

section("sdram0") volatile fract16 echo_delayed_input[echo_TAP_NUM]; //Array to store the input samples for the echo filter
fract16 lms_delayed_input[lms_TAP_NUM]; //Array to store the input samples for the adaptive lms filter
fract16 lms_filter_coeff[lms_TAP_NUM]; //Array to store the coefficients for the adaptive lms filter
volatile int freq_count; //Sets the sampling frequency of the input samples
volatile int x;		//Original input
volatile int y;		//Output from the echo simulator or the real-world environment
volatile int y_lms; //Output from the adaptive lms filter
volatile int output_error; //Error signal produced
volatile int echo_writer; //write Pointer for the echo generator delay array
volatile int echo_reader_first, echo_reader_second, echo_reader_third; //read Pointer for the echo generator delay array
volatile int ADC_flag;
fract16 in_fir, out_fir;

//Pointers for some of the variables.
fract16 *pointer;
fract16 *pointer_out;
fract16 *step_size_p;
short *sum_int_p;
fract16 *sum_fract_p;

fract16 echo_output;
fract16 step_size;
fract16 error_sig;

short sum_int;
fract16 sum_fract;
float sum;
float meu;

fir_state_fr16 state;	// declare filter state


//--------------------------------------------------------------------------//
// Function:	main														//
//																			//
// Description:	After calling a few initalization routines, main() just 	//
//				waits in a loop forever.  The code to process the incoming  //
//				data is placed in the function Process_Data() in the 	//
//				file "Process_Data.c".										//
//--------------------------------------------------------------------------//
void main(void)
{
    //Pointer Initialization
    pointer = &in_fir;
    pointer_out = &out_fir;
    step_size_p = &step_size;
    sum_int_p= &sum_int;
    sum_fract_p= &sum_fract;
    
    //Call system Initialization procedures
    Init_PLL();
    Init_EBIU();
    Init_Flash();
    Init1836();
    Init_Sport0();
    Init_DMA();
    Init_Sport_Interrupts();
    Init_Echo_Delayed_Input();
    Init_Lms_Delayed_Input();
    Init_Lms_Filter_Coeff();
    fir_init(state, lms_filter_coeff, lms_delayed_input, lms_TAP_NUM,1); // Initialize adaptive filter state
    Enable_DMA_Sport0();
    
    //Variable Initialization
    echo_writer = 0;			//Writer to the echo delay buffer
    echo_reader_first = 1;		//First echo reader - read from echo delay buffer
    echo_reader_second = 1000;	//Second echo reader - read from echo delay buffer
    echo_reader_third = 3000;	//Third echo reader - read from echo delay buffer
    freq_count = 0;
    ADC_flag = 0;
    step_size = 0x0a00;			//Initialize the step size. It can be any value
    //since the NLMS algorithm will update it.
    
    while(1)
    {
        if(ADC_flag == 1)		//If new input sample is ready, process it.
        {
            ADC_flag = 0;		//The request has been entertained.
            
            x = iChannel1LeftIn;		//Read the input
            echo_output = (iChannel0LeftIn >> 16); //Need to shift input sample -anomaly of BF533
            in_fir = x >> 16;	//Need to shift input sample -anomaly of BF533
            
            // Comment echo_generator(); if working with real world echos. Otherwise keep it.
            echo_generator();	//Call the inbuilt echo_generator
            adaptive_lms();		//Call adaptive lms
            
            y = echo_output << 16; //Need to shift input sample -anomaly of BF533
            y_lms = out_fir << 16; //Need to shift input sample -anomaly of BF533
            output_error = error_sig << 16; //Need to shift input sample -anomaly of BF533
            
            iChannel0LeftOut = y;		//Give the output
            iChannel1LeftOut = y_lms;   //Give the output
            iChannel2LeftOut = output_error; //Give the output
            
            *pSIC_IMASK = 0x00000200;	//Re-enable codec interrupts
            ssync();
        }
    }
}

