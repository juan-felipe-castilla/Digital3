#ifdef __USE_CMSIS
#include "LPC17xx.h"
#include "../Drivers/inc/lpc17xx_timer.h"
#include "../Drivers/inc/lpc17xx_dac.h"
#include "../Drivers/inc/lpc17xx_adc.h"
#include "../Drivers/inc/lpc17xx_pinsel.h"
#include "../Drivers/inc/lpc17xx_gpdma.h"
#endif

#include <cr_section_macros.h>
#include <stdio.h>

#define BUFFER_LEN        4095U //cantidad de transferencias de un buffer
void config_DMA();
void config_channel_DMA();
void config_DAC();
void config_ADC();
void config_LLI();
static uint32_t tmp = 125 ;
static uint32_t lli_DAC_control(void);
static uint32_t lli_ADC_control(void);
volatile uint16_t Buffer0[BUFFER_LEN] ={0};
volatile uint16_t Buffer1[BUFFER_LEN] ={0};

GPDMA_Channel_CFG_T conDAC;
GPDMA_Channel_CFG_T conADC;

// flag para preguntar si dma ya hizo la primera transferencia, cuando termina, el dac comienza a convertir
static volatile uint8_t dacStarted = 0U;
static DAC_CONVERTER_CFG_T dacConfig;


static GPDMA_LLI_T LLI_0_ADC;
static GPDMA_LLI_T LLI_1_ADC;
static GPDMA_LLI_T LLI_0_DAC;
static GPDMA_LLI_T LLI_1_DAC;

int main(void){

	config_DAC();
	config_ADC();
	config_LLI();
	config_channel_DMA();
	config_DMA();

	GPDMA_ChannelStart(GPDMA_CH_0); // Arranca DMA del ADC
	// El arranque del DMA del DAC conviene hacerlo en la interrupción
	// cuando el primer buffer esté lleno, pero para probar podés ponerlo acá:

	ADC_StartCmd(ADC_START_CONTINUOUS); // Que el ADC empiece a convertir
    ADC_BurstEnable();

	while(1);
}

void DMA_IRQHandler(void) {
	// pregunta si hubo error en la transferencia -> limpia flag
    if (GPDMA_IntGetStatus(GPDMA_INTERR, GPDMA_CH_0) == SET) {
        GPDMA_ClearIntPending(GPDMA_CLR_INTERR, GPDMA_CH_1);
    }

    // limpia flag al finalizar transmision
    if (GPDMA_IntGetStatus(GPDMA_INTTC, GPDMA_CH_0) == SET) {
        GPDMA_ClearIntPending(GPDMA_CLR_INTTC, GPDMA_CH_1);

        // pregunta si termino primera tranferencia -> inicia dac
        if (dacStarted == 0U) {
            dacStarted = 1U;
            GPDMA_ChannelStart(GPDMA_CH_1);
        }
    }
}

void config_LLI(){
	// ------------------------------------------------------------
	// 1. Definir la LLI (Linked List Item) con la configuración de la transferencia
	// ------------------------------------------------------------

	LLI_0_ADC.srcAddr   = (uint32_t)&(LPC_ADC->ADGDR);          // Origen: array en memoria
	LLI_0_ADC.dstAddr   = (uint32_t)Buffer0;    // Destino: buffer de memoria
	LLI_0_ADC.nextLLI   = (uint32_t)&LLI_1_ADC;              // Apunta a LL1
	LLI_0_ADC.control = lli_ADC_control();

	LLI_1_ADC.srcAddr   = (uint32_t)&(LPC_ADC->ADGDR);            // Origen: array en memoria
	LLI_1_ADC.dstAddr   = (uint32_t)Buffer1;    // Destino: buffer 1, donde leera
	LLI_1_ADC.nextLLI   = (uint32_t)&LLI_0_ADC;              // Apunta a LL0 → bucle infinito
	LLI_1_ADC.control = lli_ADC_control();

	LLI_0_DAC.srcAddr   = (uint32_t)Buffer1;            // Origen: array en memoria
	LLI_0_DAC.dstAddr   = (uint32_t)&(LPC_DAC->DACR);    // Destino: registro del dac
	LLI_0_DAC.nextLLI   = (uint32_t)&LLI_1_DAC;
	LLI_0_DAC.control = lli_DAC_control();

	LLI_1_DAC.srcAddr   = (uint32_t)Buffer0;            // Origen: array en memoria
	LLI_1_DAC.dstAddr   = (uint32_t)&(LPC_DAC->DACR);    // Destino: registro del dac
	LLI_1_DAC.nextLLI   = (uint32_t)&LLI_1_DAC;
	LLI_1_DAC.control = lli_DAC_control();
}

void config_DMA(){
	//configuramos los canales
	config_channel_DMA();
	//Iniciamos y seteamos los canales
	GPDMA_Init();

	GPDMA_SetupChannel(&conADC);
	GPDMA_SetupChannel(&conDAC);

    // limpia isr y habilia nvic
    NVIC_ClearPendingIRQ(DMA_IRQn);
    NVIC_EnableIRQ(DMA_IRQn);
}

 void config_channel_DMA(){
	conADC.channelNum = GPDMA_CH_0;
	conADC.transferSize = BUFFER_LEN;
	conADC.type = GPDMA_P2M;
	conADC.srcMemAddr = 0;//ignoramos porque es periferico
	conADC.dstMemAddr = (uint32_t)Buffer0; // destino = direccion primer elemento del array
	conADC.srcConn = GPDMA_ADC;
	conADC.dstConn = 0;//ojito que igna puso el del adc
	conADC.linkedList = (uint32_t)&LLI_0_ADC;
	conADC.intErr = ENABLE;
	conADC.intTC = ENABLE;

	conADC.src.width = GPDMA_HALFWORD;
	conADC.src.burst = GPDMA_BSIZE_1;
	conADC.src.increment = DISABLE;

	conADC.dst.width = GPDMA_HALFWORD;
	conADC.dst.burst = GPDMA_BSIZE_1;
    // incrementa dst para escribir en siguiente posicion del array
	conADC.dst.increment = ENABLE;


	conDAC.channelNum = GPDMA_CH_1;
	conDAC.transferSize = BUFFER_LEN;
	conDAC.type = GPDMA_M2P;
	conDAC.srcMemAddr = (uint32_t)Buffer1;//buffer donde el dac va a leer
	conDAC.dstMemAddr = (uint32_t) &(LPC_DAC->DACR);
	conDAC.srcConn = 0;
	conDAC.dstConn = GPDMA_DAC;
	conDAC.intErr = ENABLE;
	conDAC.intTC = DISABLE;
	conDAC.linkedList =(uint32_t)&LLI_0_DAC;
	conDAC.src.width = GPDMA_HALFWORD;
	conDAC.src.burst = GPDMA_BSIZE_1;
	conDAC.src.increment = ENABLE;	// incrementa source para escribir en siguiente posicion del array

	conDAC.dst.width = GPDMA_HALFWORD;
	conDAC.dst.burst = GPDMA_BSIZE_1;
	conDAC.dst.increment = DISABLE;

}

void config_DAC(){

	dacConfig.doubleBuffer = DISABLE;
	dacConfig.dmaRequest = ENABLE;
	dacConfig.dmaCounter = ENABLE;

	DAC_Init();//INICIALIZA EL PIN

	DAC_SetBias(DAC_350uA);
	DAC_UpdateValue(0);

	DAC_SetDMATimeOut(tmp);

	DAC_ConfigDAConverterControl(&dacConfig);
}

void config_ADC(){
	//Energizacion y ratio de frecuencia de muestreo
	ADC_Init(200000);
	//Enciende el ADC
	ADC_PowerUp();
	// Deshabilita burst para configurar
	ADC_BurstDisable();
	//Pines del canal adc
	ADC_PinConfig(ADC_CHANNEL_0);
	//Habilitamos el canal
	ADC_ChannelEnable(ADC_CHANNEL_0);
}

static uint32_t lli_DAC_control(void){
	return GPDMA_DMACCxControl_TransferSize(BUFFER_LEN) |
	           GPDMA_DMACCxControl_SBSize(GPDMA_BSIZE_1) |
	           GPDMA_DMACCxControl_DBSize(GPDMA_BSIZE_1) |
	           GPDMA_DMACCxControl_SWidth(GPDMA_HALFWORD) |
	           GPDMA_DMACCxControl_DWidth(GPDMA_HALFWORD) |
	           GPDMA_DMACCxControl_SI;
}

static uint32_t lli_ADC_control(void){
	return GPDMA_DMACCxControl_TransferSize(BUFFER_LEN) |
	           GPDMA_DMACCxControl_SBSize(GPDMA_BSIZE_1) |
	           GPDMA_DMACCxControl_DBSize(GPDMA_BSIZE_1) |
	           GPDMA_DMACCxControl_SWidth(GPDMA_HALFWORD) |
	           GPDMA_DMACCxControl_DWidth(GPDMA_HALFWORD) |
	           GPDMA_DMACCxControl_DI					|
			   GPDMA_DMACCxControl_I;
}
