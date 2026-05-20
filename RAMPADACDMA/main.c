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
static uint32_t lli_control(void);
void config_DAC();

static uint16_t vectorDAC[10];
static GPDMA_LLI_T LLI_0;
static GPDMA_LLI_T LLI_1;

int main(void) {

	config_DAC();

	for(int i = 0; i < 10; i++){
	    uint32_t valor_amplitud = (1023 / 10) * i;
	    vectorDAC[i] = (valor_amplitud << 6);
	}

	config_DMA();

	while(1);
}

void config_DMA(){
	GPDMA_Init();

	// ------------------------------------------------------------
	// 1. Definir la LLI (Linked List Item) con la configuración de la transferencia
	// ------------------------------------------------------------

	LLI_0.srcAddr   = (uint32_t)vectorDAC;           // Origen: array en memoria
	LLI_0.dstAddr   = (uint32_t)&(LPC_DAC->DACR);    // Destino: registro DACR
	LLI_0.nextLLI   = (uint32_t)&LLI_1;              // Apunta a LL1

	LLI_0.control = lli_control();

	LLI_1.srcAddr   = (uint32_t)vectorDAC;           // Origen: array en memoria
	LLI_1.dstAddr   = (uint32_t)&(LPC_DAC->DACR);    // Destino: registro DACR
	LLI_1.nextLLI   = (uint32_t)&LLI_0;              // Apunta a LL0 → bucle infinito

	LLI_1.control = lli_control();


	GPDMA_Channel_CFG_T conDMA;
	conDMA.channelNum = GPDMA_CH_0;
	conDMA.transferSize = 10;
	conDMA.type = GPDMA_M2P;
	conDMA.srcMemAddr = (uint32_t)(vectorDAC);
	conDMA.dstMemAddr = (uint32_t) &(LPC_DAC->DACR);
	conDMA.srcConn = 0;
	conDMA.dstConn = GPDMA_DAC;
	conDMA.src.width = GPDMA_WORD;;
	conDMA.src.burst = GPDMA_BSIZE_1;
	conDMA.src.increment = ENABLE;

	conDMA.dst.width = GPDMA_WORD;
	conDMA.dst.burst = GPDMA_BSIZE_1;
	conDMA.dst.increment = DISABLE;

	conDMA.intErr = ENABLE;
	conDMA.intTC = DISABLE;
	conDMA.linkedList =(uint32_t)&LLI_0;

	GPDMA_SetupChannel (&conDMA);
	GPDMA_ChannelStart(GPDMA_CH_0);
}
static uint32_t lli_control(void){
	return GPDMA_DMACCxControl_TransferSize(10) |
	           GPDMA_DMACCxControl_SBSize(GPDMA_BSIZE_1) |
	           GPDMA_DMACCxControl_DBSize(GPDMA_BSIZE_1) |
	           GPDMA_DMACCxControl_SWidth(GPDMA_HALFWORD) |
	           GPDMA_DMACCxControl_DWidth(GPDMA_HALFWORD) |
	           GPDMA_DMACCxControl_SI;
}

void config_DAC(){
	DAC_CONVERTER_CFG_T dacConfig;

	dacConfig.doubleBuffer = DISABLE;
	dacConfig.dmaRequest = ENABLE;
	dacConfig.dmaCounter = ENABLE;

	DAC_Init();//INICIALIZA EL PIN

	DAC_SetBias(DAC_350uA);
	DAC_UpdateValue(0);
	uint32_t tmp = 28 ;
	DAC_SetDMATimeOut(tmp);

	/*
	PINSEL_CFG_T pinDAC;
	pinDAC.port = PORT_0;
	pinDAC.pin = PIN_26;
	pinDAC.openDrain = DISABLE;
	pinDAC.func = PINSEL_FUNC_10;
	pinDAC.mode = PINSEL_TRISTATE;
	PINSEL_ConfigPin(&pinDAC);*/

	DAC_ConfigDAConverterControl(&dacConfig);
}
