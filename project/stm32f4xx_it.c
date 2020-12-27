/**
  ******************************************************************************
  * @file    Project/STM32F4xx_StdPeriph_Templates/stm32f4xx_it.c 
  * @author  MCD Application Team
  * @version V1.3.0
  * @date    13-November-2013
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_it.h"

/** @addtogroup Template_Project
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
//  TimingDelay_Decrement();
}

/******************************************************************************/
/*                 STM32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f4xx.s).                                               */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/
void DMA1_Stream1_IRQHandler(void){
	char checksum_Rx = 0;
	DMA_ClearITPendingBit(DMA1_Stream1, DMA_IT_TCIF1);
	GPIO_ToggleBits(GPIOD, GPIO_Pin_12);
	if(RXBuffer[0]=='$' && RXBuffer[1]=='S' && RXBuffer[2]=='P' && RXBuffer[3]=='E' && RXBuffer[4]=='E' && RXBuffer[5]=='D' && RXBuffer[6]==','){
		for(int k = 0; k < 4; k++){
			m_data.mybyte[3-k] = RXBuffer[7+k];
			checksum_Rx += RXBuffer[7+k];
		}
		if(checksum_Rx == RXBuffer[11])
			if(m_data.myfloat<=250 && m_data.myfloat>=-250)
				mainMotor.setpoint_RPM = m_data.myfloat;
				PI.setpoint						 = mainMotor.setpoint_RPM;
	}
	DMA_Cmd(DMA1_Stream1, ENABLE);
}

void TIM3_IRQHandler(void){
		if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET){
				TIM_Cmd(TIM3, DISABLE);
				TIM_SetCounter(TIM3, 0);
				GPIO_ToggleBits(GPIOD, GPIO_Pin_12);
				
				// Calculate the speed
				int16_t tmp_cnt = TIM5->CNT;
				TIM_SetCounter(TIM5, IC_StartPoint);
				mainMotor.measure_RPM = (tmp_cnt - IC_StartPoint)*60*40/(4*counterPerRound);
				checkSpeed();
			
				TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
		}
}






