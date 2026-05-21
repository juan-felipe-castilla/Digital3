#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <cr_section_macros.h>
#include "../Drivers/inc/lpc17xx_adc.h"
#include "../Drivers/inc/lpc17xx_dac.h"
#include "../Drivers/inc/lpc17xx_timer.h"
#include "../Drivers/inc/lpc17xx_systick.h"
#include "../Drivers/inc/lpc17xx_exti.h"
#include "../Drivers/inc/lpc17xx_gpdma.h"

#include <stdio.h>
typedef enum{
	PRIMER,
	SEGUNDA,
	TERCERA,
	ESPERA,
}Estado;
Estado estado = ESPERA;

GPDMA_LLI_T LLI_0;
GPDMA_LLI_T LLI_1;
GPDMA_LLI_T LLI_2;
GPDMA_LLI_T LLI_3;

void conf_DMA(void);
void conf_Pin(void);
void conf_EINT0(void);
void conf_DAC(void);
void build_lli(void);

int main(void){
	conf_Pin();//supongamos la funcion esta implementada
	conf_DAC();
	build_lli();
	conf_DMA();
	conf_EINT0();
	while(1){
	}
}

void conf_EINT0(){
	EXTI_CFG_T eint0;
	EXTI_Init();//inicializamos la int externa con valores default
	eint0.line = EXTI_EINT0;
	eint0.mode = EXTI_EDGE_SENSITIVE;
	eint0.polarity = EXTI_RISING_EDGE;
	EXTI_ConfigEnable(&eint0);//Applies the mode and polarity settings through EXTI_Config and
							  //subsequently enables the corresponding interrupt in the NVIC.
	NVIC_EnableIRQ(EINT0_IRQn);
}

void conf_DAC(){
	DAC_CONVERTER_CFG_T dacCfg;
	dacCfg.dmaCounter = ENABLE;
	dacCfg.dmaRequest = ENABLE;
	dacCfg.doubleBuffer = DISABLE;
	DAC_Init();
	DAC_SetBias(DAC_350uA);
	DAC_UpdateValue(0);
	DAC_ConfigDAConverterControl(&dacCfg);
}

void conf_DMA(){
	GPDMA_Channel_CFG_T dmaCfg0;
	GPDMA_Channel_CFG_T dmaCfg1;
	GPDMA_Channel_CFG_T dmaCfg2;

	GPDMA_Init();

	dmaCfg0.channelNum = GPDMA_CH_0;
	dmaCfg0.type = GPDMA_M2P;
	dmaCfg0.transferSize = 1024;
	dmaCfg0.srcMemAddr = DIRECCION_BLOQUE_0;
	dmaCfg0.dstMemAddr = (uint32_t) &(LPC_DAC->DACR);
	dmaCfg0.dstConn = GPDMA_DAC;
	dmaCfg0.srcConn = 0;
	dmaCfg0.intErr = ENABLE;
	dmaCfg0.intTC = DISABLE;
	dmaCfg0.linkedList = (uint32_t)&LLI_0 ;
	dmaCfg0.src.burst = GPDMA_BSIZE_1;
	dmaCfg0.src.increment = ENABLE;
	dmaCfg0.src.width = GPDMA_WORD;
	dmaCfg0.dst.burst = GPDMA_BSIZE_1;
	dmaCfg0.dst.increment = DISABLE;
	dmaCfg0.dst.width = GPDMA_WORD;

	dmaCfg1.channelNum = GPDMA_CH_1;
	dmaCfg1.type = GPDMA_M2P;
	dmaCfg1.transferSize = 1024;
	dmaCfg1.srcMemAddr = DIRECCION_BLOQUE_1;
	dmaCfg1.dstMemAddr = (uint32_t) &(LPC_DAC->DACR);
	dmaCfg1.dstConn = GPDMA_DAC;
	dmaCfg1.srcConn = 0;
	dmaCfg1.intErr = ENABLE;
	dmaCfg1.intTC = DISABLE;
	dmaCfg1.linkedList = (uint32_t)&LLI_1 ;
	dmaCfg1.src.burst = GPDMA_BSIZE_1;
	dmaCfg1.src.increment = ENABLE;
	dmaCfg1.src.width = GPDMA_WORD;
	dmaCfg1.dst.burst = GPDMA_BSIZE_1;
	dmaCfg1.dst.increment = DISABLE;
	dmaCfg1.dst.width = GPDMA_WORD;

	dmaCfg2.channelNum = GPDMA_CH_2;
	dmaCfg2.type = GPDMA_M2P;
	dmaCfg2.transferSize = 1024;
	dmaCfg2.srcMemAddr = DIRECCION_BLOQUE_0;
	dmaCfg2.dstMemAddr = (uint32_t) &(LPC_DAC->DACR);
	dmaCfg2.dstConn = GPDMA_DAC;
	dmaCfg2.srcConn = 0;
	dmaCfg2.intErr = ENABLE;
	dmaCfg2.intTC = DISABLE;
	dmaCfg2.linkedList = (uint32_t)&LLI_2 ;
	dmaCfg2.src.burst = GPDMA_BSIZE_1;
	dmaCfg2.src.increment = ENABLE;
	dmaCfg2.src.width = GPDMA_WORD;
	dmaCfg2.dst.burst = GPDMA_BSIZE_1;
	dmaCfg2.dst.increment = DISABLE;
	dmaCfg2.dst.width = GPDMA_WORD;

	GPDMA_SetupChannel(&dmaCfg0);
	GPDMA_SetupChannel(&dmaCfg1);
	GPDMA_SetupChannel(&dmaCfg2);

}

void build_lli(){
	LLI_0.srcAddr = DIRECCION_BLOQUE_0;
	LLI_0.dstAddr = (uint32_t)&(LPC_DAC->DACR);
	LLI_0.nextLLI = (uint32_t)&LLI_0;
	LLI_0.control= (DMA_SIZE | (2 << 18)                    // Source width: 32-bit
					| (2 << 21)                              // Destination width: 32-bit
					| (1 << 26));                            // Increment source address

	LLI_1.srcAddr = DIRECCION_BLOQUE_1;
	LLI_1.dstAddr = (uint32_t)&(LPC_DAC->DACR);
	LLI_1.nextLLI = (uint32_t)&LLI_1;
	LLI_1.control= (DMA_SIZE | (2 << 18)                    // Source width: 32-bit
					| (2 << 21)                              // Destination width: 32-bit
					| (1 << 26));                            // Increment source address

	LLI_2.srcAddr = DIRECCION_BLOQUE_0;
	LLI_2.dstAddr = (uint32_t)&(LPC_DAC->DACR);
	LLI_2.nextLLI = (uint32_t)&LLI_3;
	LLI_2.control= (DMA_SIZE | (2 << 18)                    // Source width: 32-bit
					| (2 << 21)                              // Destination width: 32-bit
					| (1 << 26));                            // Increment source address

	LLI_3.srcAddr = DIRECCION_BLOQUE_2;
	LLI_3.dstAddr = (uint32_t)&(LPC_DAC->DACR);
	LLI_3.nextLLI = (uint32_t)&LLI_2;
	LLI_3.control= (DMA_SIZE | (2 << 18)                    // Source width: 32-bit
						| (2 << 21)                              // Destination width: 32-bit
						| (1 << 26));                            // Increment source address
}

void EINT0_IRQHandler(){
	if(estado < 2){
		estado++;
	}
	else{
		estado = 0;
	}

	GPDMA_ChannelStop(GPDMA_CH_0);
	GPDMA_ChannelStop(GPDMA_CH_1);
	GPDMA_ChannelStop(GPDMA_CH_2);

	switch(estado){
		case PRIMER:
			DAC_SetBIAS(DAC_350uA);
			DAC_SetDMATimeOut(415);
			GPDMA_ChannelStart(GPDMA_CH_0);
			break;
		case SEGUNDA:
			DAC_SetBIAS(DAC_350uA);
			DAC_SetDMATimeOut(208);
			GPDMA_ChannelStart(GPDMA_CH_1);
			break;
		case TERCERA:
			DAC_SetBIAS(DAC_700uA);
			DAC_SetDMATimeOut(56);
			GPDMA_ChannelStart(GPDMA_CH_2);
			break;
		default:
			break;
	}
	EXTI_ClearFlag(EXTI_EINT0);
}










/*
void conf_ADC();
void conf_TIMER();
void conf_SysTick();

void promedio();
static int cont=0, prom = 0,DUTY = 25;
static uint16_t muestras[4]={0};
static volatile uint32_t ticks_30s = 0;
static volatile uint8_t flag_30s = 0;

typedef enum{
	PROMEDIO,
	SET1,
	PWM,
	CLEAR,
	ESPERA
}Estado;

Estado out = ESPERA;


int main(void) {
	conf_ADC();
	conf_SysTick();
	ADC_BurstEnable();
	while(1){
		switch(out){
		case PROMEDIO:
			promedio();
			break;
		case SET1:
			printf("salida en 1");//no me voy a poner a hacer las configs de pines
			out = ESPERA;
			break;
		case CLEAR:
			printf("salida en 0");//no me voy a poner a hacer las configs de pines
			out = ESPERA;
			break;
		case PWM:
			conf_TIMER();
			out = ESPERA;
			break;
		default:
			break;
		}
	}
}
void conf_ADC(){
	ADC_Init(200000);					//enables the ADC clock in the PCONP register, calculates the required clock divider
	ADC_PowerUp();				//based on the peripheral clock (PCLK) to achieve the target rate, and powers up the converter
	ADC_BurstDisable();			//desactivamos el burst
	ADC_PinConfig(ADC_CHANNEL_0);
	ADC_ChannelEnable(ADC_CHANNEL_0);
}

void promedio(){
	for(int i = 0; i<4;i++){
		prom += muestras[i];
	}
	prom = prom/4;//en prom esta el valor promedio del las 4 muestras en BINARIO
	if(prom<1241){//1 Volt
		out = CLEAR;
	}else if(prom>=1241 && prom<=2481){//1<prom<2
		out = PWM;
	}else{
		out = SET1;
	}
	prom = 0;
}

void conf_TIMER(){

	DUTY = 25 + (((prom - 1241) * 20)/1240); //formula para el DUTYCYCLE

	TIM_TIMERCFG_T timer0;
	TIM_MATCHCFG_T match01,match00;

	timer0.prescaleOpt = TIM_US; //timer en microsegundos
	timer0.prescaleValue = 1; //aumenta cada 1 uS

	match01.matchValue = 50; //a los 50 uS da el match 1, periodo
	match01.channel =  TIM_MATCH_1;
	match01.stopEn = DISABLE;
	match01.resetEn = ENABLE;
	match01.intEn = ENABLE;
	match01.extOpt = TIM_NOTHING;

	match00.matchValue = DUTY; //El match se da segun el valor de tension
	match00.channel =  TIM_MATCH_0;
	match00.stopEn = DISABLE;
	match00.resetEn = DISABLE;
	match00.intEn = ENABLE;
	match00.extOpt = TIM_NOTHING;

	TIM_InitTimer(LPC_TIM0,&timer0);
	TIM_ConfigMatch(LPC_TIM0,&match01);
	TIM_ConfigMatch(LPC_TIM0,&match00);

	NVIC_EnableIRQ(TIMER0_IRQn);
	// Start Timer0
	TIM_Enable(LPC_TIM0);
}

void conf_SysTick(){
	SYSTICK_InternalInit(100);
	SYSTICK_IntCmd(ENABLE);
	SYSTICK_Cmd(ENABLE);
	NVIC_EnableIRQ(SysTick_IRQn);
}

void SysTick_Handler(){
	ticks_30s++;
	if (ticks_30s >= 300) {
	   flag_30s = 1;  // Pasaron 30 segundos
	   ticks_30s = 0; // Reiniciamos el contador
		if(cont<4){
			muestras[cont] =  ADC_GlobalGetData();//obtenemos el valor mas reciente de la conversion
			cont++;
		}else{
			cont = 0;
			out = PROMEDIO;
		}
	}
	SYSTICK_ClearCounterFlag();
}

void TIMER0_IRQHandler(){
	if(TIM_GetIntStatus(LPC_TIM0, TIM_MR0_INT)){
		TIM_ClearIntPending(LPC_TIM0, TIM_MR0_INT);
		printf("PWM EN BAJO/n");
	}
	else if(TIM_GetIntStatus(LPC_TIM0, TIM_MR1_INT)){
			TIM_ClearIntPending(LPC_TIM0, TIM_MR1_INT);
			printf("PWM EN ALTO/n");
	}
}*/

