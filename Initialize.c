/*-------------------------------------------------------------------------------------------------------
 --ECSE 494         Electrical Engineering Design Project
 --
 --Project Name:    Acoustic Echo Cancellation
 --
 --Authors:         Analog Devices.
 --Modified by:     Mr Kunal Jathal - kunal.jathal@mail.mcgill.ca,
 --                 Mr Asrar Rangwala - asrar.rangwala@mail.mcgill.ca
 --
 --Copyright (C) 2006 Deep Thought
 --Version 1.0
 --Date Modified:May 9th, 2006
 --
 --File name:       Initialize.c
 --
 --Software:		VisualDSP++3.5
 --Hardware:		ADSP-BF533 EZ-KIT Board
 --
 --Purpose:         Initialize system parameters.
 ------------------------------------------------------------------------------------------------------*/


#include "Talkthrough.h"
#include "cdefBF533.h"

void Init_PLL(void)
{
    volatile int test=0;
    
    sysreg_write(reg_SYSCFG, 0x32);		//Initialize System Configuration Register
    
    *pPLL_CTL = 0x2C00;
    ssync();
    idle();
}

//--------------------------------------------------------------------------//
// Function:	Init_EBIU													//
//																			//
// Description:	This function initializes and enables asynchronous memory 	//
//				banks in External Bus Interface Unit so that Flash A can be //
//				accessed.													//
//--------------------------------------------------------------------------//
void Init_EBIU(void)
{
    
    *pEBIU_AMBCTL0	= 0x7bb07bb0;
    *pEBIU_AMBCTL1	= 0x7bb07bb0;
    *pEBIU_AMGCTL	= 0x000f;
    
}


//--------------------------------------------------------------------------//
// Function:	Init_Flash													//
//																			//
// Description:	This function initializes pin direction of Port A in Flash A//
//				to output.  The AD1836_RESET on the ADSP-BF533 EZ-KIT board //
//				is connected to Port A.										//
//--------------------------------------------------------------------------//
void Init_Flash(void)
{
    *pFlashA_PortA_Dir = 0x1;
}


//--------------------------------------------------------------------------//
// Function:	Init1836()													//
//																			//
// Description:	This function sets up the SPI port to configure the AD1836. //
//				The content of the array sCodec1836TxRegs is sent to the 	//
//				codec.														//
//--------------------------------------------------------------------------//
void Init1836(void)
{
    int i;
    int j;
    static unsigned char ucActive_LED = 0x01;
    
    // write to Port A to reset AD1836
    *pFlashA_PortA_Data = 0x00;
    
    // write to Port A to enable AD1836
    *pFlashA_PortA_Data = ucActive_LED;
    
    // wait to recover from reset
    for (i=0; i<0xf000; i++);
    
    // Enable PF4
    *pSPI_FLG = FLS4;
    // Set baud rate SCK = HCLK/(2*SPIBAUD) SCK = 2MHz
    *pSPI_BAUD = 16;
    // configure spi port
    // SPI DMA write, 16-bit data, MSB first, SPI Master
    *pSPI_CTL = TIMOD_DMA_TX | SIZE | MSTR;
    
    // Set up DMA5 to transmit
    // Map DMA5 to SPI
    *pDMA5_PERIPHERAL_MAP	= 0x5000;
    
    // Configure DMA5
    // 16-bit transfers
    *pDMA5_CONFIG = WDSIZE_16;
    // Start address of data buffer
    *pDMA5_START_ADDR = sCodec1836TxRegs;
    // DMA inner loop count
    *pDMA5_X_COUNT = CODEC_1836_REGS_LENGTH;
    // Inner loop address increment
    *pDMA5_X_MODIFY = 2;
    
    // enable DMAs
    *pDMA5_CONFIG = (*pDMA5_CONFIG | DMAEN);
    // enable spi
    *pSPI_CTL = (*pSPI_CTL | SPE);
    
    // wait until dma transfers for spi are finished
    for (j=0; j<0xaff; j++);
    
    // disable spi
    *pSPI_CTL = 0x0000;
}


//--------------------------------------------------------------------------//
// Function:	Init_Sport0													//
//																			//
// Description:	Configure Sport0 for TDM mode, to transmit/receive data 	//
//				to/from the AD1836. Configure Sport for external clocks and //
//				frame syncs.												//
//--------------------------------------------------------------------------//
void Init_Sport0(void)
{
    // Sport0 receive configuration
    // External CLK, External Frame sync, MSB first
    // 32-bit data
    *pSPORT0_RCR1 = RFSR;
    *pSPORT0_RCR2 = SLEN_32;
    
    // Sport0 transmit configuration
    // External CLK, External Frame sync, MSB first
    // 24-bit data
    *pSPORT0_TCR1 = TFSR;
    *pSPORT0_TCR2 = SLEN_32;
    
    // Enable MCM 8 transmit & receive channels
    *pSPORT0_MTCS0 = 0x000000FF;
    *pSPORT0_MRCS0 = 0x000000FF;
    
    // Set MCM configuration register and enable MCM mode
    *pSPORT0_MCMC1 = 0x0000;
    *pSPORT0_MCMC2 = 0x101c;
}


//--------------------------------------------------------------------------//
// Function:	Init_DMA													//
//																			//
// Description:	Initialize DMA1 in autobuffer mode to receive and DMA2 in	//
//				autobuffer mode to transmit									//
//--------------------------------------------------------------------------//
void Init_DMA(void)
{
    // Set up DMA1 to receive
    // Map DMA1 to Sport0 RX
    *pDMA1_PERIPHERAL_MAP = 0x1000;
    
    // Configure DMA1
    // 32-bit transfers, Interrupt on completion, Autobuffer mode
    *pDMA1_CONFIG = WNR | WDSIZE_32 | DI_EN | FLOW_1;
    // Start address of data buffer
    *pDMA1_START_ADDR = iRxBuffer1;
    // DMA inner loop count
    *pDMA1_X_COUNT = 8;
    // Inner loop address increment
    *pDMA1_X_MODIFY	= 4;
    
    
    // Set up DMA2 to transmit
    // Map DMA2 to Sport0 TX
    *pDMA2_PERIPHERAL_MAP = 0x2000;
    
    // Configure DMA2
    // 32-bit transfers, Autobuffer mode
    *pDMA2_CONFIG = WDSIZE_32 | FLOW_1;
    // Start address of data buffer
    *pDMA2_START_ADDR = iTxBuffer1;
    // DMA inner loop count
    *pDMA2_X_COUNT = 8;
    // Inner loop address increment
    *pDMA2_X_MODIFY	= 4;
}


//--------------------------------------------------------------------------//
// Function:	Init_Interrupts												//
//																			//
// Description:	Initialize Interrupt for Sport0 RX							//
//--------------------------------------------------------------------------//
void Init_Sport_Interrupts(void)
{
    // Set Sport0 RX (DMA1) interrupt priority to 2 = IVG9
    *pSIC_IAR0 = 0xffffffff;
    *pSIC_IAR1 = 0xffffff2f;
    *pSIC_IAR2 = 0xffffffff;
    
    // assign ISRs to interrupt vectors
    // Sport0 RX ISR -> IVG 9
    register_handler(ik_ivg9, Sport0_RX_ISR);
    
    // enable Sport0 RX interrupt
    *pSIC_IMASK = 0x00000200;
    ssync();
}


//--------------------------------------------------------------------------//
// Function:	Enable_DMA_Sport											//
//																			//
// Description:	Enable DMA1, DMA2, Sport0 TX and Sport0 RX					//
//--------------------------------------------------------------------------//
void Enable_DMA_Sport0(void)
{
    // enable DMAs
    *pDMA2_CONFIG	= (*pDMA2_CONFIG | DMAEN);
    *pDMA1_CONFIG	= (*pDMA1_CONFIG | DMAEN);
    
    // enable Sport0 TX and RX
    *pSPORT0_TCR1 	= (*pSPORT0_TCR1 | TSPEN);
    *pSPORT0_RCR1 	= (*pSPORT0_RCR1 | RSPEN);
}




