

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <cr_section_macros.h>
#include "../Drivers/inc/lpc17xx_timer.h"
#include "../Drivers/inc/lpc17xx_dac.h"
#include "../Drivers/inc/lpc17xx_exti.h"
#include "../Drivers/inc/lpc17xx_adc.h"
#include "../Drivers/inc/lpc17xx_systick.h"
#include "../Drivers/inc/lpc17xx_gpdma.h"

#include <stdio.h>

void configDMA(void);
void configEXTI(void);
void configTMR(void);
void configADC(void);
void configDAC(void);
//void configSystick(void);

#define AHBSRAMbank0 0x2007C000
#define AHBSRAMbank1 0x2007E000

static DAC_CONVERTER_CFG_T dacCfg;
static GPDMA_Channel_CFG_T adcconfig;
static GPDMA_Channel_CFG_T dacconfig;
static GPDMA_Channel_CFG_T adc2dacconfig;

static GPDMA_LLI_T LLI_DAC;
static GPDMA_LLI_T LLI_ADC;
static GPDMA_LLI_T LLI_ADC_DAC;

uint32_t *wave_form = (uint32_t *) AHBSRAMbank1;
uint16_t valor_amplitud = 0;
//static uint16_t wave_form[4096]={0}; supuestamente esto no debe ir
int cont=0;

static TIM_MATCHCFG_T match01;
static TIM_TIMERCFG_T timer0;

//static TIM_CAPTURECFG_T cap01;
//static TIM_TIMERCFG_T timer0;

int main(void) {
	configDMA();
	configTMR();
	configDAC();
	configEXTI();
	configADC();

	GPDMA_ChannelStart(GPDMA_CH_0);//habilitamos las transferencias del dac y el adc
	GPDMA_ChannelStart(GPDMA_CH_1);

	//aca generamos la funcion y la mandamos a la 2da mitad del banco
	for(int i = 307;i<614;i++){
		valor_amplitud = (512*(i-307))/307;
		wave_form[i]= (valor_amplitud << 6);
	}
	valor_amplitud = 0;
	for(int j = 0;j<307;j++){
		valor_amplitud = 512 +(512* j)/307;
		wave_form[j]= valor_amplitud;
	}

	while (1){
	}
}

void configEXTI(){
	EXTI_Init();
	EXTI_CFG_T eint0;
	eint0.line = EXTI_EINT0;
	eint0.mode = EXTI_EDGE_SENSITIVE;
	eint0.polarity = EXTI_RISING_EDGE;
	EXTI_PinConfig(&eint0,EXTI_NOPULL);
	EXTI_Config(&eint0);
	EXTI_EnableIRQ(EXTI_EINT0);
}

void configTMR(){
	timer0.prescaleOpt = TIM_US;
	timer0.prescaleValue = 1;
	TIM_InitTimer(LPC_TIM0,&timer0);
	match01.channel = TIM_MATCH_1;
	match01.extOpt = TIM_NOTHING;
	match01.matchValue = 31;
	match01.resetEn = ENABLE;
	match01.stopEn = FALSE;
	match01.intEn = FALSE;
	TIM_ConfigMatch(LPC_TIM0,&match01);
	TIM_Enable(LPC_TIM0);
}

void configDMA(){
	GPDMA_Init();

	adcconfig.channelNum = GPDMA_CH_0;
	adcconfig.transferSize = 4095;
	adcconfig.type = GPDMA_P2M;
	adcconfig.intErr = DISABLE;
	adcconfig.intTC = DISABLE;
	adcconfig.srcMemAddr = 0;
	adcconfig.dstMemAddr =(uint32_t) AHBSRAMbank0;
	adcconfig.srcConn = GPDMA_ADC;
	adcconfig.dstConn = 0;
	adcconfig.src.width = GPDMA_HALFWORD;
	adcconfig.src.burst = GPDMA_BSIZE_1;
	adcconfig.src.increment = DISABLE;
	adcconfig.dst.width = GPDMA_HALFWORD;
	adcconfig.dst.burst = GPDMA_BSIZE_1;
	adcconfig.dst.increment = ENABLE;
	adcconfig.linkedList = (uint32_t)&LLI_ADC;

	dacconfig.channelNum = GPDMA_CH_1;
	dacconfig.transferSize = 614;
	dacconfig.type=GPDMA_M2P;
	dacconfig.intErr=DISABLE;
	dacconfig.intTC=DISABLE;
	dacconfig.srcMemAddr=(uint32_t) AHBSRAMbank1;
	dacconfig.dstMemAddr=(uint32_t)&(LPC_DAC->DACR);
	dacconfig.srcConn = 0;
	dacconfig.dstConn = GPDMA_DAC;
	dacconfig.src.width = GPDMA_HALFWORD;
	dacconfig.src.burst = GPDMA_BSIZE_1;
	dacconfig.src.increment = ENABLE;
	dacconfig.dst.width = GPDMA_HALFWORD;
	dacconfig.dst.burst = GPDMA_BSIZE_1;
	dacconfig.dst.increment = DISABLE;
	dacconfig.linkedList=(uint32_t)&LLI_DAC;

	adc2dacconfig.channelNum = GPDMA_CH_2;
	adc2dacconfig.transferSize = 4095;
	adc2dacconfig.type=GPDMA_M2P;
	adc2dacconfig.intErr = DISABLE;
	adc2dacconfig.intTC = DISABLE;
	adc2dacconfig.srcMemAddr = (uint32_t) AHBSRAMbank0;
	adc2dacconfig.dstMemAddr = (uint32_t)&(LPC_DAC->DACR);
	adc2dacconfig.srcConn = 0;
	adc2dacconfig.dstConn = GPDMA_DAC;
	adc2dacconfig.src.width = GPDMA_HALFWORD;
	adc2dacconfig.src.burst = GPDMA_BSIZE_1;
	adc2dacconfig.src.increment = ENABLE;
	adc2dacconfig.dst.width = GPDMA_HALFWORD;
	adc2dacconfig.dst.burst = GPDMA_BSIZE_1;
	adc2dacconfig.dst.increment = DISABLE;
	adc2dacconfig.linkedList = (uint32_t)&LLI_ADC_DAC;

	GPDMA_SetupChannel(&adcconfig);
	GPDMA_SetupChannel(&dacconfig);
	GPDMA_SetupChannel(&adc2dacconfig);

	LLI_ADC.srcAddr = (uint32_t)&(LPC_ADC->ADGDR);          // Origen: ADC
	LLI_ADC.dstAddr = (uint32_t) AHBSRAMbank0;    // Destino: buffer de memoria
	LLI_ADC.nextLLI = (uint32_t)&LLI_ADC;              // Apunta a LL1
	LLI_ADC.control = (4095 | (1 << 18)                 // Source width: 16-bit
                    		|(1 << 21)
							|(1<<27));					//Dst increment

	LLI_DAC.srcAddr = (uint32_t) AHBSRAMbank1;            // Origen: array en memoria
	LLI_DAC.dstAddr = (uint32_t)&(LPC_DAC->DACR);    // Destino: registro del dac
	LLI_DAC.nextLLI = (uint32_t)&LLI_DAC;
	LLI_DAC.control = (614 | (1 << 18)                 // Source width: 16-bit
						|(1 << 21)
						|(1<<26));						//src increment

	LLI_ADC_DAC.srcAddr = (uint32_t)AHBSRAMbank0;            // Origen: ADC
	LLI_ADC_DAC.dstAddr = (uint32_t)&(LPC_DAC->DACR);    // Destino: registro del dac
	LLI_ADC_DAC.nextLLI = (uint32_t)&LLI_ADC_DAC;
	LLI_ADC_DAC.control = (4095 | (1 << 18)                 // Source width: 16-bit
						|(1 << 21)
						|(1<<26));
}

void configDAC(){
	DAC_Init();
	dacCfg.dmaCounter = ENABLE;
	dacCfg.dmaRequest = ENABLE;
	dacCfg.doubleBuffer = DISABLE;
	DAC_SetBias(DAC_350uA);
	DAC_ConfigDAConverterControl(&dacCfg);
	DAC_SetDMATimeOut(20);//se necesitan 20 ciclos de 50ns para q el dac actualize cada 1us
}

void configADC(){
	ADC_Init(32000);//el doble de la freecuencua de entrada para cumplir el teorema del muestreo
	ADC_BurstDisable();
	ADC_PowerUp();
	ADC_StartCmd(ADC_START_ON_MAT01);
	ADC_EdgeStartConfig(ADC_START_ON_RISING);
	ADC_PinConfig(ADC_CHANNEL_0);
	ADC_ChannelEnable(ADC_CHANNEL_0);
}

void EINT0_IRQHandler(){
	cont++;
	if(cont == 1){
		GPDMA_ChannelPause(GPDMA_CH_0);//pausamos las transferencias de adc y dac
		GPDMA_ChannelPause(GPDMA_CH_1);
		GPDMA_ChannelStart(GPDMA_CH_2);//hacemos el cambio para mandar por dac
	}
	else if(cont==2){
		GPDMA_ChannelPause(GPDMA_CH_2);//pausamos las transferencias de bank0 a dac
		GPDMA_ChannelResume(GPDMA_CH_0);
		GPDMA_ChannelResume(GPDMA_CH_1);//hacemos el cambio para mandar por dac y por adc como se estipulo
		cont=0;
	}
	 EXTI_ClearFlag(EXTI_EINT0);
}

/*
void configTMR(void);
void configDAC(void);
void configSystick(void);

static int cont=0,i=0;;
static uint16_t DC[10]={0};
static uint32_t t0,t1,t2;
static uint16_t valor_capturado = 0;
static uint16_t Periodo = 0;
static uint16_t prom = 0;


static DAC_CONVERTER_CFG_T dacCfg;

static TIM_CAPTURECFG_T cap01;
static TIM_TIMERCFG_T timer0;

int main(void) {
	configTMR();
	configDAC();
	configSystick();
	while (1){

	}
}

void configDAC(){
	DAC_Init();
	DAC_SetBias(DAC_350uA);
	dacCfg.dmaCounter = DISABLE;
	dacCfg.dmaRequest =DISABLE;
	dacCfg.doubleBuffer = DISABLE;
	DAC_ConfigDAConverterControl(&dacCfg);
}

void configTMR(){
	timer0.prescaleOpt = TIM_TICK;
	timer0.prescaleValue = 1;

	cap01.channel = TIM_CAPTURE_0;
	cap01.fallingEn = ENABLE;
	cap01.risingEn = ENABLE;
	cap01.intEn = ENABLE;

	TIM_PinConfig(TIM_CAP0_0_P1_26);
	TIM_ConfigCapture(LPC_TIM0,&cap01);
	TIM_InitTimer(LPC_TIM0,&timer0);

	NVIC_EnableIRQ(TIMER0_IRQn);

	TIM_Enable(LPC_TIM0);
}

void configSystick(void){
	SYSTICK_InternalInit(500);//se ingresa el tiempo en mS
	SYSTICK_IntCmd(ENABLE);
	NVIC_EnableIRQ(SysTick_IRQn);
	SYSTICK_Cmd(ENABLE);
}

void SysTick_IRQHandler(){
		uint32_t acumulador_prom = 0; // Variable local para evitar acumular errores
		// Sumamos todo primero para no perder decimales por truncamiento entero
		for(int j = 0; j < 10; j++) {
			acumulador_prom += DC[j];
		}
		prom = acumulador_prom / 10;

		DAC_UpdateValue(prom);
		prom = 0;
		NVIC_ClearPendingIRQ(SysTick_IRQn);
}

void TIMER0_IRQHandler(){
	if (TIM_GetIntStatus(LPC_TIM0, TIM_CR0_INT)){
		valor_capturado = TIM_GetCaptureValue(LPC_TIM0, TIM_CAPTURE_0);
		cont++;
		if (cont == 1) {
			t0 = valor_capturado; // Bajada 1
		} else if (cont == 2) {
			t1 = valor_capturado; // Subida
		} else if (cont == 3) {
			t2 = valor_capturado; // Bajada 2

			Periodo = t2 - t0;
			if (Periodo > 0){
				//Guardamos en la posición 'i' actual ANTES de incrementarla
				DC[i] = ((t2 - t1) * 620) / Periodo;
				//Incremento circular  (0 a 9) sin desbordar
				i = (i + 1) % 10;
			}
			//Sincronización de flancos. La bajada actual es el t0 del próximo período.
			t0 = t2;
			cont = 1;
		}
		TIM_ClearIntPending(LPC_TIM0, TIM_CR0_INT);
	}
}

