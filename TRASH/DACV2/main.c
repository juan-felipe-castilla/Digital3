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

static uint32_t vectorDAC[10];

int main(void) {

	config_DAC();

	for(int i = 0; i < 10; i++){
	    uint32_t valor_amplitud = (1023 / 10) * i;
	    vectorDAC[i] = (valor_amplitud) | (1 << 16);   // Añadir BIAS
	}

	config_DMA();

	while(1);
}

void config_DMA(){
	GPDMA_Init();

	// ------------------------------------------------------------
	// 1. Definir la LLI (Linked List Item) con la configuración de la transferencia
	// ------------------------------------------------------------
	static GPDMA_LLI_T LLI_0;
	LLI_0.srcAddr   = (uint32_t)vectorDAC;           // Origen: array en memoria
	LLI_0.dstAddr   = (uint32_t)&(LPC_DAC->DACR);    // Destino: registro DACR
	LLI_0.nextLLI   = (uint32_t)&LLI_0;              // Apunta a sí misma → bucle infinito

	    // Palabra de control:
	    // bits 11:0   = transfer size = 10 (diez valores)
	    // bits 14:12  = source burst size = 0 → 1 transferencia por petición
	    // bits 17:15  = source width = 1 → halfword (16 bits)
	    // bits 20:18  = dest burst size = 0 → 1 transferencia
	    // bits 23:21  = dest width = 2 → word (32 bits)
	    // bit 24      = source increment = 1 (avanza por el array)
	    // bit 25      = dest increment = 0 (registro fijo)
	    // bit 26      = TC interrupt = 0 (no usamos interrupción)
	LLI_0.control = 10
	                    | (0 << 12)
	                    | (1 << 15)
	                    | (0 << 18)
	                    | (2 << 21)
	                    | (1 << 24)
	                    | (0 << 25)
	                    | (0 << 26);


	//ENUMS para configurar src y dst
	GPDMA_Endpoint_T src_en;
	GPDMA_Endpoint_T dst_en;

	src_en.width = 0;
	src_en.burst = GPDMA_BSIZE_1;
	src_en.increment = ENABLE;

	dst_en.width = 0;
	dst_en.burst = GPDMA_BSIZE_1;
	dst_en.increment = DISABLE;



	GPDMA_Channel_CFG_T conDMA;
	conDMA.channelNum = 0;
	conDMA.transferSize = 20;
	conDMA.type = GPDMA_M2P;
	conDMA.srcMemAddr = (uint32_t)(vectorDAC);
	conDMA.dstMemAddr = (uint32_t) &(LPC_DAC->DACR);
	conDMA.srcConn = 0;
	conDMA.dstConn = GPDMA_DAC;
	conDMA.src = src_en;
	conDMA.dst = dst_en;
	conDMA.intErr = DISABLE;
	conDMA.intTC = DISABLE;
	conDMA.linkedList =(uint32_t)&LLI_0;

	GPDMA_SetupChannel (&conDMA);
	GPDMA_ChannelStart(GPDMA_CH_0);
}

void config_DAC(){
	DAC_CONVERTER_CFG_T dacConfig;

	dacConfig.doubleBuffer = DISABLE;
	dacConfig.dmaRequest = ENABLE;
	dacConfig.dmaCounter = ENABLE;


	//VALUE = 28d

	uint32_t tmp = 2500 ;
	DAC_SetDMATimeOut(tmp);

	PINSEL_CFG_T pinDAC;
	pinDAC.port = PORT_0;
	pinDAC.pin = PIN_26;
	pinDAC.openDrain = DISABLE;
	pinDAC.func = PINSEL_FUNC_10;
	pinDAC.mode = PINSEL_TRISTATE;
	PINSEL_ConfigPin(&pinDAC);

	DAC_Init();

	DAC_ConfigDAConverterControl(&dacConfig);
}
