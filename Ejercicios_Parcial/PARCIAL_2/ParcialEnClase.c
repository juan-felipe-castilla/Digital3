#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <cr_section_macros.h>
#include "../Drivers/inc/lpc17xx_timer.h"
#include "../Drivers/inc/lpc17xx_dac.h"
#include "../Drivers/inc/lpc17xx_pinsel.h"
#include "../Drivers/inc/lpc17xx_gpdma.h"

#include <stdio.h>

void condigDMA();


int main(void) {



	while (1)
	{

	}
}


configDMA(){
	GPDMA_Init();

	GPDMA_Channel_CFG_T dma;

	dma.channelNum = GPDMA_CH_0;
	dma.transferSize = 4095;
	dma.type = GPDMA_M2M;
	dma.srcMemAddr = (uint32_t)0x20080000;
	dma.dstMemAddr = (uint32_t)0x2007E000;
	dma.srcConn = 0;
	dma.dstConn = 0;
	dma.src.width = GPDMA_HALFWORD;
	dma.src.burst = GPDMA_BSIZE_1;
	dma.src.increment = ENABLE;
	dma.dst.width = GPDMA_HALFWORD;
	dma.src.burst = GPDMA_BSIZE_1;
	dma.src.increment = ENABLE;
	dma.intTC = ENABLE;
	dma.intErr = DISABLE;
	dma.linkedList = (uint32_t) &LLI0;


	GPDMA_LLI_T LLI0;
	LLI0.srcAddr = (uint32_t)0x20080000 + 8191;
	LLI0.dstAddr = (uint32_t)0x2007E000 + 8191;
	LLI0.nextLLI = 0;
	LLI0.control = 0;
	LLI0.control = 5;    //esto hay que determinarlo. tiene puesto 5 porque si.


}
