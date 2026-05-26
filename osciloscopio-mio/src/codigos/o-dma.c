/*
 * o-dma.c
 *
 *  Created on: 25 may. 2026
 *      Author: benja
 */

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <cr_section_macros.h>
#include <string.h>
#include <stdio.h>

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

#include "../headers/o-dma.h"

// Definiciones para la señal
#define ADC_BUFFER_SIZE 4095

extern uint32_t adc_buffer[ADC_BUFFER_SIZE];

// Bandera para indicar fin de transferencia DMA
volatile uint8_t dma_done = 0;

/**
 * @brief Función de captura de ADC mediante DMA.
 * Configura el GPDMA para transferir 2000 muestras desde el ADC a la memoria.
 */
void capture_adc_dma(void) {
    static GPDMA_Channel_CFG_T GPDMACfg;

    // Reinicia la bandera de fin
    dma_done = 0;

    // Inicializa el controlador GPDMA
    GPDMA_Init();

    // Configura los parámetros del canal DMA
    GPDMACfg.channelNum = GPDMA_CH_0;
    GPDMACfg.transferSize = ADC_BUFFER_SIZE;
    GPDMACfg.type = GPDMA_P2M; // De periférico (ADC) a memoria (RAM)

    // Dirección de origen: Registro de Datos Global del ADC (ADGDR)
    GPDMACfg.srcMemAddr = (uint32_t)&(LPC_ADC->ADGDR);
    GPDMACfg.srcConn = GPDMA_ADC;

    // Dirección de destino: Nuestro arreglo en RAM
    GPDMACfg.dstMemAddr = (uint32_t)adc_buffer;
    GPDMACfg.dstConn = 0; // Ignorado en P2M

    // Configuración del origen (ADC)
    GPDMACfg.src.width = GPDMA_WORD;      // Lectura de 32 bits (ADGDR)
    GPDMACfg.src.burst = GPDMA_BSIZE_1;    // Una conversión a la vez
    GPDMACfg.src.increment = DISABLE;      // El registro ADC es fijo

    // Configuración del destino (Memoria)
    GPDMACfg.dst.width = GPDMA_WORD;      // Escritura de 32 bits
    GPDMACfg.dst.burst = GPDMA_BSIZE_1;
    GPDMACfg.dst.increment = ENABLE;       // Incrementar puntero del arreglo

    // Habilita interrupción de Terminal Count (fin de transferencia)
    GPDMACfg.intTC = ENABLE;
    GPDMACfg.intErr = ENABLE;

    // Aplicar configuración al canal 0
    GPDMA_SetupChannel(&GPDMACfg);

    // Habilita el ADC para generar solicitudes de DMA (vía interrupción global)
    ADC_IntEnable(ADC_INT_GLOBAL);

    // Inicia el modo ráfaga (Burst) para conversiones continuas a 200kHz
    ADC_BurstEnable();

    // Inicia el canal DMA
    GPDMA_ChannelStart(GPDMA_CH_0);

    // Espera hasta que la transferencia se complete (polling sobre el estado del canal)
    // En una implementación real se usaría la interrupción de GPDMA.
    while (GPDMA_IntGetStatus(GPDMA_INTTC, GPDMA_CH_0) == RESET);

    // Limpia las banderas de interrupción del DMA
    GPDMA_ClearIntPending(GPDMA_CLR_INTTC, GPDMA_CH_0);

    // Detiene el modo ráfaga para ahorrar energía
    ADC_BurstDisable();
}
