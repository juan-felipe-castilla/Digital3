#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <cr_section_macros.h>
#include "../Drivers/inc/lpc17xx_timer.h"
#include "../Drivers/inc/lpc17xx_dac.h"
#include "../Drivers/inc/lpc17xx_gpio.h"
#include "../Drivers/inc/lpc17xx_gpdma.h"

#include <stdio.h>

void configDMA(void);
static GPDMA_Channel_CFG_T dmacfg;

int main(void){
	configDMA();
	while (1){
	}
}

void configDMA(){
 GPDMA_Init();
 dmacfg.channelNum = GPDMA_CH_0;
 dmacfg.type = GPDMA_M2M;
 dmacfg.transferSize = 4095;
 dmacfg.srcMemAddr = (uint32_t) 0x20080000;
 dmacfg.dstMemAddr= (uint32_t)0x2007E000;
 dmacfg.srcConn = 0;
 dmacfg.dstConn = 0;
 dmacfg.intErr = ENABLE;
 dmacfg.intTC = ENABLE;
 dmacfg.src.width = GPDMA_HALFWORD;
 dmacfg.dst.width = GPDMA_HALFWORD;
 dmacfg.src.increment = ENABLE;
 dmacfg.dst.increment = ENABLE;
 dmacfg.src.burst = GPDMA_BSIZE_1;
 dmacfg.dst.burst = GPDMA_BSIZE_1;
 GPDMA_SetupChannel(&dmacfg);
 GPDMA_ChannelStart(GPDMA_CH_0);

 NVIC_EnableIRQ(DMA_IRQn);
}

void GPDMA_IRQHandler(){
	GPDMA_ClearIntPending(GPDMA_CLR_INTTC, GPDMA_CH_0);
	GPIO_SetPinState(PORT_0,PIN_22,SET);
}
