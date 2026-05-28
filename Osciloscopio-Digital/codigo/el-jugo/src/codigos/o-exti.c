#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <cr_section_macros.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include "../Drivers/inc/debug_frmwrk.h"
#include "../Drivers/inc/lpc17xx_adc.h"
#include "../Drivers/inc/lpc17xx_can.h"
#include "../Drivers/inc/lpc17xx_clkpwr.h"
#include "../Drivers/inc/lpc17xx_common.h"
#include "../Drivers/inc/lpc17xx_dac.h"
#include "../Drivers/inc/lpc17xx_emac.h"
#include "../Drivers/inc/lpc17xx_exti.h"
#include "../Drivers/inc/lpc17xx_gpdma.h"
#include "../Drivers/inc/lpc17xx_gpio.h"
#include "../Drivers/inc/lpc17xx_i2c.h"
#include "../Drivers/inc/lpc17xx_i2s.h"
#include "../Drivers/inc/lpc17xx_libcfg_default.h"
#include "../Drivers/inc/lpc17xx_mcpwm.h"
#include "../Drivers/inc/lpc17xx_nvic.h"
#include "../Drivers/inc/lpc17xx_pinsel.h"
#include "../Drivers/inc/lpc17xx_pwm.h"
#include "../Drivers/inc/lpc17xx_qei.h"
#include "../Drivers/inc/lpc17xx_rit.h"
#include "../Drivers/inc/lpc17xx_rtc.h"
#include "../Drivers/inc/lpc17xx_spi.h"
#include "../Drivers/inc/lpc17xx_ssp.h"
#include "../Drivers/inc/lpc17xx_systick.h"
#include "../Drivers/inc/lpc17xx_timer.h"
#include "../Drivers/inc/lpc17xx_uart.h"
#include "../Drivers/inc/lpc17xx_wdt.h"
#include "../Drivers/inc/lpc_types.h"


#include"../headers/o-exti.h"
#include "../headers/variables.h"



void conf_EXTI(){
	EXTI_CFG_T eint0;
	EXTI_Init();
	EXTI_PinConfig(EXTI_EINT0, EXTI_PULLDOWN);
	eint0.line = EXTI_EINT0;
	eint0.mode = EXTI_EDGE_SENSITIVE;
	eint0.polarity = EXTI_RISING_EDGE;
	EXTI_Config(&eint0);

	EXTI_CFG_T eint1;
	EXTI_PinConfig(EXTI_EINT1, EXTI_PULLDOWN);
	eint1.line = EXTI_EINT1;
	eint1.mode = EXTI_EDGE_SENSITIVE;
	eint1.polarity = EXTI_RISING_EDGE;
	EXTI_Config(&eint1);

	EXTI_EnableIRQ(EXTI_EINT1);
	EXTI_EnableIRQ(EXTI_EINT0);
}

void EINT0_IRQHandler(){
	//Apagamos canales DMA-DAC (1,2,3)
	NVIC_ClearPendingIRQ(EINT0_IRQn);
	contDACen++;
	printf("Prendido");

	if (contDACen == 1){
	GPDMA_ChannelResume(GPDMA_CH_1);
	GPDMA_ChannelPause(GPDMA_CH_2);
	GPDMA_ChannelPause(GPDMA_CH_3);
	}
	else
	{
		contDACen = 0;
		GPDMA_ChannelPause(GPDMA_CH_1);
		GPDMA_ChannelPause(GPDMA_CH_2);
		GPDMA_ChannelPause(GPDMA_CH_3);
		printf("Apagado");
	}
}

void EINT1_IRQHandler(){
	//Cambiamos entre sin-square-trian (DAC-DMA)
	NVIC_ClearPendingIRQ(EINT1_IRQn);

	contDAC++;
	if (contDAC == 1)
	{
		GPDMA_ChannelPause(GPDMA_CH_1);
		GPDMA_ChannelResume(GPDMA_CH_2);
		GPDMA_ChannelPause(GPDMA_CH_3);
	}
	else if(contDAC == 2){
		GPDMA_ChannelPause(GPDMA_CH_1);
		GPDMA_ChannelPause(GPDMA_CH_2);
		GPDMA_ChannelResume(GPDMA_CH_3);
	}
	else{
		contDAC = 0;

		GPDMA_ChannelResume(GPDMA_CH_1);
		GPDMA_ChannelPause(GPDMA_CH_2);
		GPDMA_ChannelPause(GPDMA_CH_3);
	}


}
