#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <cr_section_macros.h>
#include <stdio.h>

#include "../Drivers/inc/lpc17xx_timer.h"
#include "../Drivers/inc/lpc17xx_dac.h"
#include "../Drivers/inc/lpc17xx_adc.h"
#include "../Drivers/inc/lpc17xx_pinsel.h"
#include "../Drivers/inc/lpc17xx_gpdma.h"

void conf_DAC(void);
void conf_DMA(void);
void build_lli(void);
void conf_TMR(void);
void conf_Pinsel(void); //suponemos esta implementado

GPDMA_LLI_T LLI0;
GPDMA_LLI_T LLI1;
GPDMA_LLI_T LLI2;
GPDMA_LLI_T LLI3;

static uint16_t DACPendPos[10];
static uint16_t DACPendNeg[10];
static uint16_t NegDACPendPos[10];
static uint16_t NegDACPendNeg[10];

static uint16_t valor_amplitud = 0;

int main(void) {
	conf_DAC();
	conf_TMR();
	conf_Pinsel();
	for(int i = 5; i < 10; i++){//empezamos en la mitad ya que debemos tener todo el rango dinamico
	    valor_amplitud = (1023 / 10) * i;
	    DACPendPos[i] = (valor_amplitud << 6);
	}
	for(int i = 10; i>5 ; i--){//desde el maximo hacia la mitad
		valor_amplitud = (1023 / 10) * i;
		DACPendNeg[i] = (valor_amplitud << 6);
	}
	for(int i = 5; i >= 1 ; i--){//desde la mitad hacia abajo
		valor_amplitud = (1023 / 10) * i;
		NegDACPendPos[i] = (valor_amplitud << 6);
	}
	for(int i = 0; i < 5; i++){//desde abajo hacia la mitad
		valor_amplitud = (1023 / 10) * i;
		NegDACPendPos[i] = (valor_amplitud << 6);
	}
	while(1){
	}
}

void conf_TMR(){
	 TIM_TIMERCFG_T timer0;
	 timer0.prescaleOpt = TIM_TICK;
	 timer0.prescaleValue = 5; //aumenta cada 0,2 uS
	 TIM_InitTimer(LPC_TIM0,&timer0);

	 TIM_MATCHCFG_T match01;
	 match01.channel = TIM_MATCH_1 ;
	 match01.matchValue = 4;//al llegar a 1uS matchea, osea estamos trabajando a 1MHz en el DAC
	 match01.extOpt = TIM_NOTHING;
	 match01.intEn = DISABLE;
	 match01.resetEn = ENABLE;
	 match01.stopEn = DISABLE;
	 TIM_ConfigMatch(LPC_TIM0,&match01);//DMA se dispara por match
	 TIM_Enable(LPC_TIM0);
}

void conf_DAC(){
	DAC_Init();
	DAC_CONVERTER_CFG_T dacCfg;
	dacCfg.dmaRequest = ENABLE;
	dacCfg.doubleBuffer = DISABLE;
	dacCfg.dmaCounter = DISABLE;
	DAC_SetBias(DAC_700uA);//para obtener el maxima frecuancia posible
	DAC_ConfigDAConverterControl(&dacCfg);
}

void conf_DMA(){
	GPDMA_Init();
	GPDMA_Channel_CFG_T dmaCfg;
	dmaCfg.channelNum = GPDMA_CH_0;
	dmaCfg.transferSize = 10;
	dmaCfg.type = GPDMA_M2P;
	dmaCfg.dstConn = GPDMA_DAC;
	dmaCfg.srcConn = 0;
	dmaCfg.intErr= ENABLE;
	dmaCfg.intTC= DISABLE;
	dmaCfg.dstMemAddr = (uint32_t) &(LPC_DAC->DACR);
	dmaCfg.srcMemAddr = (uint32_t) (DACPendPos);
	dmaCfg.src.burst = GPDMA_BSIZE_1;
	dmaCfg.src.width = GPDMA_HALFWORD;
	dmaCfg.src.increment = DISABLE;
	dmaCfg.dst.burst = GPDMA_BSIZE_1;
	dmaCfg.dst.width = GPDMA_HALFWORD;
	dmaCfg.dst.increment= DISABLE;
	dmaCfg.linkedList = (uint32_t) &LLI0;

	GPDMA_SetupChannel (&dmaCfg);
	GPDMA_ChannelStart(GPDMA_CH_0);

	LLI0.dstAddr = (uint32_t)&(LPC_DAC->DACR);
	LLI0.srcAddr = (uint32_t) (DACPendPos);
	LLI0.nextLLI = (uint32_t)&LLI1;
	LLI0.control =(10 | (1 << 18)                 // Source width: 16-bit
            		|(2 << 21));                // Destination width: 16-bit


	LLI1.dstAddr = (uint32_t)&(LPC_DAC->DACR);
	LLI1.srcAddr = (uint32_t) (DACPendNeg);
	LLI1.nextLLI =(uint32_t)&LLI2;
	LLI1.control =(10 | (1 << 18)                 // Source width: 16-bit
					|(2 << 21));

	LLI2.dstAddr =(uint32_t)&(LPC_DAC->DACR);
	LLI2.srcAddr = (uint32_t)(NegDACPendNeg);
	LLI2.nextLLI = (uint32_t)&LLI3;
	LLI2.control = (10 | (1 << 18)                 // Source width: 16-bit
					|(2 << 21));

	LLI3.dstAddr = (uint32_t)&(LPC_DAC->DACR);
	LLI3.srcAddr =(uint32_t)(NegDACPendPos);
	LLI3.nextLLI = (uint32_t)&LLI0;
	LLI3.control = (10 | (1 << 18)                 // Source width: 16-bit
					|(2 << 21));

}


/*
void conf_ADC(void);
void conf_TMR(void);
void conf_Pinsel(void); //suponemos esta implementado
volatile uint8_t idx = 0; // Índice compartido para ambos canales
static uint16_t buffer_ch2[20]={0};
static uint16_t buffer_ch4[20]={0};


uint16_t r1 = 0,r0 = 0;
int main(void) {
	conf_ADC();
	conf_TMR();
	conf_Pinsel();
	while(1){
	}
}
void conf_ADC(){
	ADC_Init(50000);
	ADC_PowerUp();
	ADC_BurstDisable();
	ADC_PinConfig(ADC_CHANNEL_2);
	ADC_PinConfig(ADC_CHANNEL_4);
	ADC_ChannelEnable(ADC_CHANNEL_2);
	ADC_ChannelEnable(ADC_CHANNEL_4);
	ADC_IntEnable(ADC_INT_CH2);
	ADC_IntEnable(ADC_INT_CH4);
	ADC_StartCmd(ADC_START_ON_MAT01);
	ADC_EdgeStartConfig(ADC_START_ON_RISING);
	NVIC_EnableIRQ(ADC_IRQn);
}

void conf_TMR(){
	 TIM_TIMERCFG_T timer0;
	 timer0.prescaleOpt = TIM_US;
	 timer0.prescaleValue = 1; //aumenta cada 1 uS
	 TIM_InitTimer(LPC_TIM0,&timer0);

	 TIM_MATCHCFG_T match01;
	 match01.channel = TIM_MATCH_1;
	 match01.matchValue = 20;//a los 20uS matchea para cumplir el teorema del muestreo
	 match01.extOpt = TIM_NOTHING;
	 match01.intEn = DISABLE;
	 match01.resetEn = ENABLE;
	 match01.stopEn = DISABLE;
	 TIM_ConfigMatch(LPC_TIM0,&match01);//adc se dispara por macth
	 TIM_Enable(LPC_TIM0);
}

void ADC_IRQHandler(){
	if(	ADC_ChannelGetStatus(ADC_CHANNEL_2, DONE)){
		buffer_ch2[idx] = ADC_ChannelGetData(ADC_CHANNEL_2);
	}
	if(	ADC_ChannelGetStatus(ADC_CHANNEL_4, DONE)){
		buffer_ch4[idx] = ADC_ChannelGetData(ADC_CHANNEL_4);
	}
	idx++;
	if (idx >= 20) idx = 0;
	NVIC_ClearPendingIRQ(ADC_IRQn);
}
*/
