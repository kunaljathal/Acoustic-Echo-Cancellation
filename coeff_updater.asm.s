/*******************************************************************************************
ECSE 494        Electrical Engineering Design Project

Authors:        Mr Kunal Jathal - kunal.jathal@mail.mcgill.ca,
                Mr Asrar Rangwala - asrar.rangwala@mail.mcgill.ca,
 

Copyright (C) 2006 Deep Thought
Version 1.0
Date Modified:  May 9th, 2006

File Name      : coeff_updater.asm
Function Name  : __coeff_updater

Description    : This function updates the adaptive filter coeffs.
Operands	   : R0- error signal, R1-step_size,
			   : Filter structure is on the stack.             

Prototype:
                void coeff_updater(fract16 error_sig, fract16 step_size, fir_state_fr16 *s);
            
                s->
                {
                    fract16 *h, // filter coefficients 
                    fract16 *d, // start of delay line 
                    fract16 *p, // read/write pointer 
                    int k; // number of coefficients 
                    int l; // interpolation/decimation index - not used in our system
                } fir_state_fr16;
 
******************************************************************************************/

.section program;
.global    __coeff_updater;
.align 8;
__coeff_updater :		
			
			P0=[SP+12];		    // Address of the filter structure
			nop;nop;nop;		// Instruction alignment
			P1=[P0++];			// Address of the filter coefficient array
			P2=[P0++];   		// Address of the delay line
			P3=[P0++];			// Address of the delay line current pointer
			R2=[P0++];			// Number of filter coefficients
			P4=R2;				// Loop Index
			
			B0=P1;				// Filter coeff. array initialized as a circular buffer
			I0=P1;				// Initialize I0 to the start of the filter coeff. array for read
			B1=P2;				// Delay line buffer is initialized as a circular buffer 	
			I1=P3;     			// start of the delay line read pointer
			B2=P1;				// Filter coeff. array initialized as a circular buffer
			I2=P1;				// Initialize I2 to the start of the filter coeff. array			

			R2=R2+R2;
					
			L0=R2;      		// Set the length of the delay line buffer
			L1=R2;      		// Initialize the filter coeff. length register
			L2=R2;				// Initialize the filter coeff. length register
			nop;nop;			// Instruction alignment
					
			I1+=4;				//Adjust the read pointer to the filter coeff.
			I2-=4;				//Adjust the write pointer to the filter coeff.
			R6=[I2];			//The first time the last two coefficients h(L-1) and h(L-2) are over-written
/************************************************************************************************/												
			
			R3.L=R0.L*R1.L;		//R3.L contains error_sig * step_size

			R4.L=W[I1++];		//Fetch x(n)
			LSETUP(E_FIR_START,E_FIR_END) LC0=P4>>1; //Loop 1 to Nc/2
			
E_FIR_START:	R2=[I0++] || R4.H=W[I1++];  //In the first Iteration: Fetch h0 and h1. Fetch x(n-1)
				R7.L=R3.L*R4.L, R7.H=R3.L*R4.H || [I2++]=R6; //In the first iteration:
															 //R7.L = error_sig*step_size*x(0), R7.H = error_sig*step_size*x(n-1)							
															 //Store the previous updated coefficients
E_FIR_END:		R6=R7+|+R2 (S) || R4.L=W[I1++]; //In the first Iteration:
												//h0 = h0+error_sig*step_size*x(0), h1 = h1+error_sig*step_size*x(n-1)
												//Fetch x(n-2)

			[I2++]=R6;			//Store the updated coefficients
/*************************************************************************************************/
		  	L0=0;              // Clear the circular buffer initialization
			L1=0;
			L2=0;
			L3=0;
			RTS;				

__coeff_updater.end:	
