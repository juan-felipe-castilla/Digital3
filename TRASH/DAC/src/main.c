/*
 * Copyright 2022 NXP
 * NXP confidential.
 * This software is owned or controlled by NXP and may only be used strictly
 * in accordance with the applicable license terms.  By expressly accepting
 * such terms or by downloading, installing, activating and/or otherwise using
 * the software, you are agreeing that you have read, and that you agree to
 * comply with and are bound by, such license terms.  If you do not agree to
 * be bound by the applicable license terms, then you may not retain, install,
 * activate or otherwise use the software.
 */

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#include "../Drivers/inc/lpc17xx_timer.h"
#include "../Drivers/inc/lpc17xx_dac.h"
#include "../Drivers/inc/lpc17xx_pinsel.h"
#endif

#include <cr_section_macros.h>

#include <stdio.h>
void config_TMR();
void config_DAC();

static int cont =0;
static int vectorDAC[10];

int main(void) {
	config_TMR();
	config_DAC();
	for(int i = 0;i<10;i++){
		vectorDAC[i]= (1023/10)*i;
	}
	while(1){
		DAC_UpdateValue(LPC_DAC,vectorDAC[cont]);
	}
}

void config_TMR(){
	TIM_TIMERCFG_Type timerConfig;
	TIM_MATCHCFG_Type matchConfig;

	// Timer configuration: counter mode
	timerConfig.PrescaleOption = TIM_PRESCALE_USVAL;  // Prescaler in microseconds
	timerConfig.PrescaleValue = 10;  //

	// Initialize Timer2 with the specified configuration
	TIM_Init(LPC_TIM0, TIM_TIMER_MODE, &timerConfig);

	// Match configuration to achieve the 30-second interval
	matchConfig.MatchChannel = 0;
	matchConfig.IntOnMatch = ENABLE;  // Enable interrupt on match
	matchConfig.ResetOnMatch = ENABLE;  // Reset timer on match
	matchConfig.StopOnMatch = DISABLE;  // Do not stop timer on match
	matchConfig.ExtMatchOutputType = TIM_EXTMATCH_NOTHING;  // No external match output
	matchConfig.MatchValue = 10;  // 100  us

	// Configure the match for Timer0
	TIM_ConfigMatch(LPC_TIM0, &matchConfig);

	// Enable the interrupt in the NVIC for Timer2
	NVIC_EnableIRQ(TIMER0_IRQn);

	// Start Timer0
	TIM_Cmd(LPC_TIM0, ENABLE);
}

void config_DAC(){
	DAC_CONVERTER_CFG_Type dacConfig;

	dacConfig.DBLBUF_ENA = DISABLE;
	dacConfig.DMA_ENA = DISABLE;
	dacConfig.CNT_ENA = DISABLE;
	dacConfig.DMA_ENA = DISABLE;

	PINSEL_CFG_Type pinDAC;
	pinDAC.Portnum = PINSEL_PORT_0;
	pinDAC.Pinnum = PINSEL_PIN_26;
	pinDAC.Funcnum = PINSEL_FUNC_2;
	pinDAC.Pinmode = PINSEL_PINMODE_TRISTATE;
	PINSEL_ConfigPin(&pinDAC);

	DAC_Init(LPC_DAC);

	DAC_ConfigDAConverterControl(LPC_DAC, &dacConfig);
}
void TIMER0_IRQHandler(){
	cont++;
	if(cont>=10){
		cont=0;
	}
	TIM_ClearIntPending(LPC_TIM0, TIM_MR0_INT);  // Clear the interrupt flag
}
