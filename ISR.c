/*-------------------------------------------------------------------------------------------------------
 --ECSE 494         Electrical Engineering Design Project
 --
 --Project Name:    Acoustic Echo Cancellation
 --
 --Authors:         Analog Devices
 --Modified by:     Mr Kunal Jathal - kunal.jathal@mail.mcgill.ca,
 --                 Mr Asrar Rangwala - asrar.rangwala@mail.mcgill.ca
 --
 --Copyright (C) 2006 Deep Thought
 --Version 1.0
 --Date Modified:   May 9th, 2006
 --
 --File name:       ISR.c
 --
 --Software:		VisualDSP++3.5
 --Hardware:		ADSP-BF533 EZ-KIT Board
 --
 --Purpose:         Handle the interrupts caused by the audio codec AD 1836
 ------------------------------------------------------------------------------------------------------*/



//--------------------------------------------------------------------------//
// Function:	Sport0_RX_ISR												//
//																			//
// Description: This ISR is executed after a complete frame of input data 	//
//				has been received. The new samples are stored in 			//
//				iChannel0LeftIn, iChannel0RightIn, iChannel1LeftIn and 		//
//				iChannel1RightIn respectively.  A flag is set for the 		//
//				main procedure which processes the data.					//
//				After that the processed values are copied from the 		//
//				variables iChannel0LeftOut, iChannel0RightOut, 				//
//				iChannel1LeftOut and iChannel1RightOut into the dma 		//
//				transmit buffer.											//
//--------------------------------------------------------------------------//

#include "Talkthrough.h"

EX_INTERRUPT_HANDLER(Sport0_RX_ISR)
{
    // confirm interrupt handling
    *pDMA1_IRQ_STATUS = 0x0001;
    
    if(freq_count == 5) //change freq_count in order to change the sampling freq.
    {
        *pSIC_IMASK = 0x00000000; // Disable interrupts
        ssync();
        
        freq_count = 0;
        // copy input data from dma input buffer into variables
        iChannel0LeftIn = iRxBuffer1[INTERNAL_ADC_L0]; //Echoed input
        iChannel0RightIn = iRxBuffer1[INTERNAL_ADC_R0];
        iChannel1LeftIn = iRxBuffer1[INTERNAL_ADC_L1]; //Clean input
        iChannel1RightIn = iRxBuffer1[INTERNAL_ADC_R1];
        
        ADC_flag = 1;	//Flag for main indicating that the new input sample is ready
        
        // copy processed data from variables into dma output buffer
        iTxBuffer1[INTERNAL_DAC_L0] = iChannel0LeftOut;
        iTxBuffer1[INTERNAL_DAC_R0] = 0;
        iTxBuffer1[INTERNAL_DAC_L1] = iChannel1LeftOut;
        iTxBuffer1[INTERNAL_DAC_R1] = 0;
        iTxBuffer1[INTERNAL_DAC_L2] = iChannel2LeftOut;	
        iTxBuffer1[INTERNAL_DAC_R2] = 0;
    }
    else
    {
        freq_count++;
    }
}
