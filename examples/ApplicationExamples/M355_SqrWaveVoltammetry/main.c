/******************************************************************************
Copyright (c) 2017-2019 Analog Devices, Inc. All Rights Reserved.

This software is proprietary to Analog Devices, Inc. and its licensors.
By using this software you agree to the terms of the associated
Analog Devices Software License Agreement.

*****************************************************************************/

#include "UrtLib.h"
#include "ClkLib.h"
#include "DioLib.h"
#include <stdio.h>
#include "ad5940.h"

void UartInit(void);
void ClockInit(void);

int main(void)
{
  void AD5940_Main(void);
  ClockInit();
  UartInit();
  DioOenPin(pADI_GPIO2,PIN4,1);               // Enable P2.4 as Output to toggle DS2 LED
  DioClrPin(pADI_GPIO2,PIN4);           // Flash LED
  AD5940_MCUResourceInit(0);    /* Inialize all peripherals etc. used for AD5940/AFE. */
	
	DioCfgPin(pADI_GPIO0,PIN3,0);    //GPIO CSO - enable - P0.3 - H61 configuration 
	DioOenPin(pADI_GPIO0,PIN3,1); 
	DioSetPin(pADI_GPIO0,PIN3); 
	
	DioCfgPin(pADI_GPIO0,PIN0,0);    //GPIO A0 - SCLK0 - P0.0 - H9 configuration 
	DioOenPin(pADI_GPIO0,PIN0,1); 
	
	DioCfgPin(pADI_GPIO0,PIN1,0);    //GPIO A1 - Mosi - P0.1 - H8 configuration 
	DioOenPin(pADI_GPIO0,PIN1,1); 

  AD5940_Main();
}

void ClockInit(void)
{
   DigClkSel(DIGCLK_SOURCE_HFOSC);
   ClkDivCfg(1,1);   //HCLK = PCLK = 26MHz
}

// Initialize UART for 230400-8-N-1
void UartInit(void)
{
   DioCfgPin(pADI_GPIO0,PIN10|PIN11,1);               // Setup P0.10, P0.11 as UART pin
   pADI_UART0->COMLCR2 = 0x3;                   // Set PCLk oversampling rate 32. (PCLK to UART baudrate generator is /32) 
	 UrtCfg(pADI_UART0,115200,
          (BITM_UART_COMLCR_WLS|3),0);         // Configure UART for 57600 baud rate
   UrtFifoCfg(pADI_UART0, RX_FIFO_1BYTE,      // Configure the UART FIFOs for 8 bytes deep
              BITM_UART_COMFCR_FIFOEN);
   UrtFifoClr(pADI_UART0, BITM_UART_COMFCR_RFCLR// Clear the Rx/TX FIFOs
              |BITM_UART_COMFCR_TFCLR);
	 UrtIntCfg(pADI_UART0,BITM_UART_COMIEN_ERBFI|BITM_UART_COMIEN_ETBEI|BITM_UART_COMIEN_ELSI);   // Enable Rx, Tx and Rx buffer full Interrupts
	 NVIC_EnableIRQ(UART_EVT_IRQn);               // Enable UART interrupt source in NVIC

}

