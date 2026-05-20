#ifdef __USE_CMSIS
#include "LPC17xx.h"
#include "../Drivers/inc/lpc17xx_timer.h"
#include "../Drivers/inc/lpc17xx_dac.h"
#include "../Drivers/inc/lpc17xx_pinsel.h"
#include "../Drivers/inc/lpc17xx_gpdma.h"
#endif

#include <cr_section_macros.h>

#include <stdio.h>
void config_DMA();
void config_DAC();

static int vectorDAC[10];

int main(void) {

	config_DAC();

	for(int i = 0; i < 10; i++){
	    uint32_t valor_amplitud = (1023 / 10) * i;
	    vectorDAC[i] = (valor_amplitud << 6); // Corremos 6 lugares a la izquierda
	}

	config_DMA();

	while(1){

	}
}

void config_DMA(){
	GPDMA_Init();


	//LLI
	GPDMA_LLI_T LLI_0;
	LLI_0.srcAddr = (uint32_t)vectorDAC;
	LLI_0.dstAddr = (uint32_t)&(LPC_DAC->DACR);
	LLI_0.nextLLI = (uint32_t)&LLI_0;
	LLI_0.control = 10
					|(1 << 18)
					|(2 << 21)
					|(1 << 26);



	//ENUMS para configurar src y dst
	GPDMA_Endpoint_T src_en;
	GPDMA_Endpoint_T dst_en;

	src_en.width = GPDMA_HALFWORD;
	src_en.burst = GPDMA_BSIZE_1;
	src_en.increment = !DISABLE;

	dst_en.width = GPDMA_WORD;
	dst_en.burst = GPDMA_BSIZE_1;
	dst_en.increment = DISABLE;



	GPDMA_Channel_CFG_T conDMA;
	conDMA.channelNum = 0;
	conDMA.transferSize = 10;
	conDMA.type = GPDMA_M2P;
	conDMA.srcMemAddr = (uint32_t)(vectorDAC);
	conDMA.dstMemAddr = (uint32_t) &(LPC_DAC->DACR);
	conDMA.srcConn = 0;
	conDMA.dstConn = GPDMA_DAC;
	conDMA.src = src_en;
	conDMA.src = dst_en;
	conDMA.intErr = DISABLE;
	conDMA.intTC = DISABLE;
	conDMA.linkedList = (uint32_t)&LLI_0;

	GPDMA_SetupChannel (&conDMA);
	GPDMA_ChannelStart(GPDMA_CH_0);
}

void config_DAC(){
	DAC_CONVERTER_CFG_T dacConfig;

	dacConfig.doubleBuffer = DISABLE;
	dacConfig.dmaRequest = ENABLE;
	dacConfig.dmaCounter = ENABLE;


	//VALUE = 28d

	uint16_t tmp = 28;
	DAC_SetDMATimeOut(tmp);

	PINSEL_CFG_T pinDAC;
	pinDAC.port = PORT_0;
	pinDAC.pin = PIN_26;
	pinDAC.func = PINSEL_FUNC_10;
	pinDAC.mode = PINSEL_TRISTATE;
	PINSEL_ConfigPin(&pinDAC);

	DAC_Init();

	DAC_ConfigDAConverterControl(&dacConfig);
}

