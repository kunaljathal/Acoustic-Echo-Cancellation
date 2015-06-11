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
 --File name:       Process_data.c
 --
 --Software:		VisualDSP++3.5
 --Hardware:		ADSP-BF533 EZ-KIT Board
 --
 --Purpose:         Process the input samples and produce the echo generator and the adaptive lms output.
 ------------------------------------------------------------------------------------------------------*/

#include "Talkthrough.h"

void echo_generator(void)
{
    echo_output = in_fir;
    
    // Produce First echo
    echo_output = add_fr1x16(echo_output, multr_fr1x16(0x1000, echo_delayed_input[echo_reader_first]));
    // Produce Second echo
    echo_output = add_fr1x16(echo_output, multr_fr1x16(0x2000, echo_delayed_input[echo_reader_second]));
    // Produce Third echo
    echo_output = add_fr1x16(echo_output, multr_fr1x16(0x4000, echo_delayed_input[echo_reader_third]));
    
    
    echo_delayed_input[echo_writer] = in_fir; //Store the current input in the echo delay buffer
    
    //Increment the echo buffer readers
    echo_writer++;
    echo_reader_first++;
    echo_reader_second++;
    echo_reader_third++;
    
    if(echo_writer == echo_TAP_NUM)
    {
        echo_writer = 0;
    }
    if(echo_reader_first == echo_TAP_NUM)
    {
        echo_reader_first = 0;
    }
    if(echo_reader_second == echo_TAP_NUM)
    {
        echo_reader_second = 0;
    }
    if(echo_reader_third == echo_TAP_NUM)
    {
        echo_reader_third = 0;
    }
}

void adaptive_lms(void)
{
    // First, produce y_lms.
    fir_fr16(pointer, pointer_out, 1, &state);	//Call the in-built efficient FIR filter.
    
    // Find the error signal
    adder_unit();
    
    // Calculate the step size------------------------------------
    // If you do not want to update the step size automatically,
    // comment this section of step size calculation between the
    // top and bottom comments.
    _NLMS_step_updater_sum(sum_int_p, sum_fract_p, &state);
    
    sum = sum_int + fr16_to_float(sum_fract);
    
    meu = 1 / (0.5 + sum);	//alpha = 1 and gamma = 0.5.
    
    if(meu>=1)
    {
        step_size=0x7fff;
    }
    else
    {
        step_size = float_to_fr16(meu);
    }
    //Step size calculation ends----------------------------------
    
    // Lastly, update the filter coefficients
    _coeff_updater(error_sig, step_size, &state);
    
}

void adder_unit(void)
{
    error_sig = sub_fr1x16(echo_output, out_fir);
}

