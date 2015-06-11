/*******************************************************************************************
ECSE 494        Electrical Engineering Design Project

Authors:        Mr Kunal Jathal - kunal.jathal@mail.mcgill.ca,
                Mr Asrar Rangwala - asrar.rangwala@mail.mcgill.ca
 

Copyright (C) 2006 Deep Thought
Version 1.0
Date Modified:May 9th, 2006

File Name      : NLMS_step_updater_sum.asm
Module Name    : 
Function Name  : __NLMS_step_updater_sum

Description    : This function performs summation of the delayed input signal squared.
                 Returns the mantissa as int and fractional as fract.
Operands	   : R0- sum_int, R1-sum_fract,
			   : Filter structure is on stack.             

Prototype:
                void NLMS_step_updater_sum(short *sum_int, fract16 *sum_fract, fir_state_fr16 *s);
            
                s->
                {
                    fract16 *h, // filter coefficients 
                    fract16 *d, // start of delay line 
                    fract16 *p, // read/write pointer 
                    int k; // number of coefficients 
                    int l; // interpolation/decimation index 
                } fir_state_fr16;
 
Registers used   :   

            R0 = sum_int,
            R1 = sum_fract, 
******************************************************************************************/

.section program;
.global    __NLMS_step_updater_sum;
.align 8;
__NLMS_step_updater_sum :		
			
			P0=[SP+12];		    // Address of the filter structure
			nop;nop;nop;
			P1=[P0++];			// Address of the filter coefficient array
			P2=[P0++];   		// Address of the delay line
			P3=[P0++];			// Address of the delay line current pointer
			R3=[P0++];			// Number of filter coefficients
			P1=R0;				// Address of sum_int
			P4=R1;				// Address of sum_fract
						
			B0=P2;				// Delay line buffer is initialized as a circular buffer 	
			I0=P3;				// Initialize I0 to the last element in the delay buffer
			B1=P2;				// Delay line buffer is initialized as a circular buffer 	
			I1=P3;     			// start of the delay line read pointer

			R3=R3+R3;
		
			R7.H=0xffff;
			R7.L=0;
			
			L0=R3;      		// Initialize delay line buffer length register						
			L1=R3;      		// Initialize delay line buffer length register
			P2=8; 				// Outer Loop index - need to change for different adaptive TAP length
								// For a TAP length of 4000 make it (4000/250)/2 = 8
								// For a TAP length of 1000 make it (1000/250/2 = 2
								// For a TAP length of 500 make it (500/250)/2 = 1
								
			P3=250; 			// Inner Loop index - DO NOT CHANGE
			nop;nop;nop;
					
			I1+=4;				// Initialize the second read buffer to the current element
			I0+=2;				// Initialize the first read buffer to the last element
			R3=0;
			R6=0;
/************************************************************************************************/												
			A0=R3.L*R3.L,A1=R3.L*R3.L || R4.L=W[I1++] || R4.H=W[I0--];		//Initialize A0 and A1 to 0. Fetch x(0) and x(n-L-1).

			LSETUP(E_OUTER_START, E_OUTER_END)LC0=P2;//Loop P2 times.
			
E_OUTER_START:	LSETUP(E_FIR_START,E_FIR_END) LC1=P3; //Loop P3 which is always 250 times.
E_FIR_START:		R5.L=R4.L*R4.L,R5.H=R4.H*R4.H;	  //In the first iteratrion: Perform x(0)*x(0) and x(n-L-1)*x(n-L-1).
					R5=R5<<1;						  //Make the value unsigned
E_FIR_END:			A0+=R5.L*R7.H, A1+=R5.H*R7.H (FU) || R4.L=W[I1++] || R4.H=W[I0--]; //Add the terms to the running sum in A0 and A1. In the first iteration: Fetch x(1) and x(n-L-2) 
				
				R2.L = A0.X;		// Extract the int part
				R5.L = A1.X;		// Extract the int part
				R2.L = R2.L + R5.L (S);
				R6.H = R2.L + R6.H (S); //Store it in R6.H
				
				A0.X=R7.L;	//Initialize A0.x and A1.x to 0.
E_OUTER_END:	A1.X=R7.L;

			R5.H=A1;
			A0+= R5.H*R7.H (FU);

			R7.L = A0.X;
			R6.H= R7.L + R6.H (NS);
			R7.L=0;
			A0.X=R7.L;
			A0=A0>>>1;
			R6.L=A0;		// Extract the fractional part of the sum
								
			W[P1] = R6.H;	   // Store the mantissa as int
			W[P4] = R6.L;	   // Store the fractional as fract
									
/*************************************************************************************************/
		  	L0=0;              // Clear the circular buffer initialization
			L1=0;
			L2=0;
			L3=0;
			RTS;				

__NLMS_step_updater_sum.end:			

