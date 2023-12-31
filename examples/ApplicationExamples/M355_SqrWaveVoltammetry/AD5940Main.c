/*!
 *****************************************************************************
 @file:    AD5940Main.c
 @author:  $Author: nxu2 $
 @brief:   Used to control specific application and process data.
 @version: $Revision: 766 $
 @date:    $Date: 2017-08-21 14:09:35 +0100 (Mon, 21 Aug 2017) $
 -----------------------------------------------------------------------------

Copyright (c) 2017-2019 Analog Devices, Inc. All Rights Reserved.

This software is proprietary to Analog Devices, Inc. and its licensors.
By using this software you agree to the terms of the associated
Analog Devices Software License Agreement.

*****************************************************************************/
#include "SqrWaveVoltammetry.h"
#include "DioLib.h"
#include <stdio.h>
#include "ad5940.h"
/**
   User could configure following parameters
**/

#define APPBUFF_SIZE 1024
uint32_t AppBuff[APPBUFF_SIZE];
float LFOSCFreq;    /* Measured LFOSC frequency */
int step=1, wait=0;
void RunRecipe(int start)
{
	if(wait==0)
	{
		if(step==1)
		{
				AD5940_Delay10us(3000);
				printf("#P2_ON_99_10;");
			  wait=1;
		}
		if(step==2)
		{
				AD5940_Delay10us(3000);
				printf("#WAIT_30;");
			  wait=1;					
		}
		if(step==3)
		{
				AD5940_Delay10us(3000);
				printf("#C8_ON;");
				wait=1;			
		}
		if(step==4)
		{
			  AD5940_Delay10us(3000);
				printf("#P3_ON_99_12;");
				wait=1;			
		}
		if(step==5)
		{
				printf("#P1_ON_99_12;");
				wait=1;			
		}
		if(step==6)
		{
				AD5940_Delay10us(3000);
				printf("#P3_ON_99_12;");	
				wait=1;				
		}	
		if(step==7)
		{
				AD5940_Delay10us(3000);
				printf("#P1_ON_-99_12;");	
				wait=1;				
			step=8;
		}				
//		switch(step)
//		{
//			case 1:
//				//do someting 

//				printf("#P2_ON_99_10;");
//					AD5940_Delay10us(3000);
//			  wait=1;
//				break;
//			case 2:
//				//do something

//				printf("#WAIT_30;");
//					AD5940_Delay10us(3000);
//			  wait=1;			
//				break;			
//			case 3:
//				//do something

//				printf("#C8_ON;");
//	
//				wait=1;
//				break;			
//			case 4:
//				//do something

//				printf("#P3_ON_99_12;");
//					AD5940_Delay10us(3000);
//				wait=1;
//				break;			
//			case 5:
//		
//				printf("#WAIT_30;");
//					AD5940_Delay10us(3000);
//				wait=1;
//				break;			
//			case 6:
//				//do something

//				printf("#P1_ON_-99_12;");	
//					AD5940_Delay10us(3000);
//				wait=1;			
//				break;			
//			case 7:
//				//do something

//				printf("#P2_ON_99_12;");	
//					AD5940_Delay10us(3000);
//				wait=1;			
//				break;	
//			case 8:
//				//do something	
//			
//				wait=1;			
//				break;		
//			case 9:
//				//do something	
//			
//				wait=1;			
//				break;	
//			case 10:
//				//do something	
//			
//				wait=1;			
//				break;			
//			default:
//				start=0;
//				wait=1;
//			break;
//		}
	}	
}
/**
 * @brief An example to deal with data read back from AD5940. Here we just print data to UART
 * @note UART has limited speed, it has impact when sample rate is fast. Try to print some of the data not all of them.
 * @param pData: the buffer stored data for this application. The data from FIFO has been pre-processed.
 * @param DataCount: The available data count in buffer pData.
 * @return return 0.
*/
uint32_t index1=0,general_index,j=0;;
float SQW_DataToPrint[2500],SQW_DataToPrint_filt[2500];
static int32_t RampShowResult(float *pData, uint32_t DataCount)
{
 /* Print data*/
  for(int i=0;i<DataCount;i++)
  {
				index1++;

   // printf("index:%d, %.3f \n",index1 , pData[i]);
		SQW_DataToPrint[index1]=pData[i];
		
    //i += 10;  /* Print though UART consumes too much time. */
  }
	general_index=index1;
	
  return 0;
}

void PrintData(void)
{
	index1=0;
	j=0;
	for(int i=0;i<general_index;i++)
  {
//		printf("Data:%ld, %.3f \n", i, SQW_DataToPrint[i]);
//		printf("R:%f\n", SQW_DataToPrint[i]/SQW_DataToPrint[i+1]);
		SQW_DataToPrint_filt[j]=SQW_DataToPrint[i+2]-SQW_DataToPrint[i];
		SQW_DataToPrint_filt[j+1]=SQW_DataToPrint[i+3];
		//printf("Data:%ld, %.3f \n", i, SQW_DataToPrint_filt[j]);
		//printf("Data:%ld, %.3f \n", i, SQW_DataToPrint_filt[j+1]);
	
		j=j+2;
		i=i+3;
		  
  }
	for(int i=j;i>0;i--)
  {
		printf("%.3f,%.3f\n", SQW_DataToPrint_filt[i-1], SQW_DataToPrint_filt[i]);
		i--;
		AD5940_Delay10us(3000);
	}
	general_index=0;
}

/**
 * @brief The general configuration to AD5940 like FIFO/Sequencer/Clock. 
 * @note This function will firstly reset AD5940 using reset pin.
 * @return return 0.
*/
static int32_t AD5940PlatformCfg(void)
{
  CLKCfg_Type clk_cfg;
  SEQCfg_Type seq_cfg;  
  FIFOCfg_Type fifo_cfg;
  AGPIOCfg_Type gpio_cfg;
  LFOSCMeasure_Type LfoscMeasure;

  /* Use hardware reset */
  AD5940_HWReset();
  AD5940_Initialize();    /* Call this right after AFE reset */
	
  /* Platform configuration */
  /* Step1. Configure clock */
  clk_cfg.HFOSCEn = bTRUE;
  clk_cfg.HFXTALEn = bFALSE;
  clk_cfg.LFOSCEn = bTRUE;
  clk_cfg.HfOSC32MHzMode = bFALSE;
  clk_cfg.SysClkSrc = SYSCLKSRC_HFOSC;
  clk_cfg.SysClkDiv = SYSCLKDIV_1;
  clk_cfg.ADCCLkSrc = ADCCLKSRC_HFOSC;
  clk_cfg.ADCClkDiv = ADCCLKDIV_1;
  AD5940_CLKCfg(&clk_cfg);
  /* Step2. Configure FIFO and Sequencer*/
  fifo_cfg.FIFOEn = bTRUE;           /* We will enable FIFO after all parameters configured */
  fifo_cfg.FIFOMode = FIFOMODE_FIFO;
  fifo_cfg.FIFOSize = FIFOSIZE_4KB;   /* 2kB for FIFO, The reset 4kB for sequencer */
  fifo_cfg.FIFOSrc = FIFOSRC_SINC3;   /* */
  fifo_cfg.FIFOThresh = 4;            /*  Don't care, set it by application paramter */
  AD5940_FIFOCfg(&fifo_cfg);
  seq_cfg.SeqMemSize = SEQMEMSIZE_2KB;  /* 4kB SRAM is used for sequencer, others for data FIFO */
  seq_cfg.SeqBreakEn = bFALSE;
  seq_cfg.SeqIgnoreEn = bTRUE;
  seq_cfg.SeqCntCRCClr = bTRUE;
  seq_cfg.SeqEnable = bFALSE;
  seq_cfg.SeqWrTimer = 0;
  AD5940_SEQCfg(&seq_cfg);
  /* Step3. Interrupt controller */
  AD5940_INTCCfg(AFEINTC_1, AFEINTSRC_ALLINT, bTRUE);   /* Enable all interrupt in INTC1, so we can check INTC flags */
  AD5940_INTCClrFlag(AFEINTSRC_ALLINT);
  AD5940_INTCCfg(AFEINTC_0, AFEINTSRC_DATAFIFOTHRESH|AFEINTSRC_ENDSEQ|AFEINTSRC_CUSTOMINT0, bTRUE); 
  AD5940_INTCClrFlag(AFEINTSRC_ALLINT);
  /* Step4: Configure GPIOs */
  gpio_cfg.FuncSet = GP0_INT|GP1_SLEEP|GP2_SYNC;  /* GPIO1 indicates AFE is in sleep state. GPIO2 indicates ADC is sampling. */
  gpio_cfg.InputEnSet = 0;
  gpio_cfg.OutputEnSet = AGPIO_Pin0|AGPIO_Pin1|AGPIO_Pin2;
  gpio_cfg.OutVal = 0;
  gpio_cfg.PullEnSet = 0;
  AD5940_AGPIOCfg(&gpio_cfg);
  /* Measure LFOSC frequency */
  /**@note Calibrate LFOSC using system clock. The system clock accuracy decides measurement accuracy. Use XTAL to get better result. */
  LfoscMeasure.CalDuration = 1000.0;  /* 1000ms used for calibration. */
  LfoscMeasure.CalSeqAddr = 0;        /* Put sequence commands from start address of SRAM */
  LfoscMeasure.SystemClkFreq = 16000000.0f; /* 16MHz in this firmware. */
  AD5940_LFOSCMeasure(&LfoscMeasure, &LFOSCFreq);
 // printf("LFOSC Freq:%f\n", LFOSCFreq);
 // AD5940_SleepKeyCtrlS(SLPKEY_UNLOCK);         /*  */
  return 0;
}

/**
 * @brief The interface for user to change application paramters.
 * @return return 0.
*/
void AD5940RampStructInit(void)
{
  AppSWVCfg_Type *pRampCfg;
  
  AppSWVGetCfg(&pRampCfg);
  /* Step1: configure general parmaters */
  pRampCfg->SeqStartAddr = 0x10;                /* leave 16 commands for LFOSC calibration.  */
  pRampCfg->MaxSeqLen = 512-0x10;              /* 4kB/4 = 1024  */
  pRampCfg->RcalVal = 200.0;                  /* 200 Ohm RCAL on EVAL-ADuCM355QSPZ */
  pRampCfg->ADCRefVolt = 1820.0f;  
  pRampCfg->FifoThresh = 1023;                   /* Maximum value is 2kB/4-1 = 512-1. Set it to higher value to save power. */
  pRampCfg->SysClkFreq = 16000000.0f;           /* System clock is 16MHz by default */
  pRampCfg->LFOSCClkFreq = LFOSCFreq;           /* LFOSC frequency */
	pRampCfg->AdcPgaGain = ADCPGA_1;
	pRampCfg->ADCSinc3Osr = ADCSINC3OSR_4;
  
	/* Step 2:Configure square wave signal parameters */
  pRampCfg->RampStartVolt = 200.0f;     /* Measurement starts at 0V*/
  pRampCfg->RampPeakVolt = 1400.0f;     		 /* Measurement finishes at -0.4V */
  pRampCfg->VzeroStart = 600.0f;           /* Vzero is voltage on SE0 pin: 1.3V */
  pRampCfg->VzeroPeak = 600.0f;          /* Vzero is voltage on SE0 pin: 1.3V */
  pRampCfg->Frequency = 50;                 /* Frequency of square wave in Hz */
  pRampCfg->SqrWvAmplitude = 60;       /* Amplitude of square wave in mV */
  pRampCfg->SqrWvRampIncrement = 5; /* Increment in mV*/
  pRampCfg->SampleDelay = 2.0f;             /* Time between update DAC and ADC sample. Unit is ms and must be < (1/Frequency)/2 - 0.2*/
  pRampCfg->LPTIARtiaSel = LPTIARTIA_1K;      /* Maximum current decides RTIA value */
	pRampCfg->bRampOneDir = bTRUE;//bTRUE;			/* Only measure ramp in one direction */
}

void AD5940_Main(void)
{
  uint32_t temp;  
//  AD5940PlatformCfg();
//  AD5940RampStructInit();

//  AppSWVInit(AppBuff, APPBUFF_SIZE);    /* Initialize RAMP application. Provide a buffer, which is used to store sequencer commands */
//  AppSWVCtrl(APPCTRL_START, 0);          /* Control IMP measurment to start. Second parameter has no meaning with this command. */

  while(1)
  {
		//RunRecipe(1);
		if(start_measurment==1)
		{		
			DioClrPin(pADI_GPIO2,PIN4);           // Flash LED
			AD5940PlatformCfg();
			AD5940RampStructInit();	
			//AD5940_McuSetLow();
			AppSWVInit(AppBuff, APPBUFF_SIZE);    /* Initialize RAMP application. Provide a buffer, which is used to store sequencer commands */				
			AD5940_Delay10us(100000);		/* Add a delay to allow sensor reach equilibrium befor starting the measurement */
			AppSWVCtrl(APPCTRL_START, 0);          /* Control IMP measurement to start. Second parameter has no meaning with this command. */
			start_measurment=2;
		}
		if(start_measurment==2)
		{
			if(AD5940_GetMCUIntFlag())
			{
				AD5940_ClrMCUIntFlag();
				temp = APPBUFF_SIZE;
				AppSWVISR(AppBuff, &temp);
				RampShowResult((float*)AppBuff, temp);
			
					if(totalDataReceivedSQW1==1)
					{
						PrintData();
						totalDataReceivedSQW1=0;
						start_measurment=0;
					}
				}
			}else
			{
				DioTglPin(pADI_GPIO2,PIN4);           // Flash LED
				AD5940_Delay10us(50000);
			}
		}

}
