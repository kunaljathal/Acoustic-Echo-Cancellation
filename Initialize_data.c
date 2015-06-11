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
 --File name:       Initialize_Data.c
 --
 --Software:		VisualDSP++3.5
 --Hardware:		ADSP-BF533 EZ-KIT Board
 --
 --Purpose:         Initialize the arrays that contain the input samples and the lms filter
 --                 filter coefficients.
 ------------------------------------------------------------------------------------------------------*/

#include "Talkthrough.h"

//Initialize echo input delay buffer
void Init_Echo_Delayed_Input(void)
{
    int i;
    for(i=0; i<echo_TAP_NUM; i++)
    {
        echo_delayed_input[i] = 0;
    }
}

//Initialize adaptive lms input delay buffer
void Init_Lms_Delayed_Input(void)
{
    int i;
    for(i=0; i<lms_TAP_NUM; i++)
    {
        lms_delayed_input[i] = 0;
    }
}

//Initialize adaptive lms filter coeff buffer
void Init_Lms_Filter_Coeff(void)
{
    int i;
    for(i=0; i<lms_TAP_NUM; i++)
    {
        lms_filter_coeff[i] = 0;
    }
}


