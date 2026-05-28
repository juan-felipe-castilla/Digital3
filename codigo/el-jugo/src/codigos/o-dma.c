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
#include "../headers/variables.h"



void capture_adc_dma_start(void) {
    // --- FIX ABSOLUTO: Inicializar en 0 ---
    // Esto asegura que campos críticos (como el puntero de DMALLI)
    // no arranquen con basura de la RAM y crasheen el micro.
    GPDMA_Channel_CFG_T GPDMACfg = {0};

    // El destapacañerías para evitar el Overrun del ADC
    volatile uint32_t dummy_read = LPC_ADC->ADGDR;

    // Configura los parámetros del canal DMA
    GPDMACfg.channelNum = GPDMA_CH_0;
    GPDMACfg.transferSize = ADC_BUFFER_SIZE;
    GPDMACfg.type = GPDMA_P2M;

    GPDMACfg.srcMemAddr = (uint32_t)&(LPC_ADC->ADGDR);
    GPDMACfg.srcConn = GPDMA_ADC;

    // Selecciona el destino según el buffer activo
    if (active_dma_buffer == 0) {
        GPDMACfg.dstMemAddr = (uint32_t)adc_buffer_A;
    } else {
        GPDMACfg.dstMemAddr = (uint32_t)adc_buffer_B;
    }

    GPDMACfg.dstConn = 0;

    GPDMACfg.src.width = GPDMA_WORD;
    GPDMACfg.src.burst = GPDMA_BSIZE_1;
    GPDMACfg.src.increment = DISABLE;

    GPDMACfg.dst.width = GPDMA_WORD;
    GPDMACfg.dst.burst = GPDMA_BSIZE_1;
    GPDMACfg.dst.increment = ENABLE;

    GPDMACfg.intTC = ENABLE;
    GPDMACfg.intErr = ENABLE;

    // Aplica y arranca
    GPDMA_SetupChannel(&GPDMACfg);
    GPDMA_ChannelStart(GPDMA_CH_0);
}

void DMA_IRQHandler(void) {
    // 1. Verificar y LIMPIAR bandera de ERROR (Esto evita que se cuelgue)
    if (GPDMA_IntGetStatus(GPDMA_INTERR, GPDMA_CH_0)) {
        GPDMA_ClearIntPending(GPDMA_CLR_INTERR, GPDMA_CH_0);

        // --- FIX VITAL: REVIVIR EL DMA ---
        // Si hubo error, el DMA se detuvo. Tenemos que volver a arrancarlo
        // forzosamente o no transmite nunca más.
        capture_adc_dma_start();
        return; // Salimos para no ejecutar lo de abajo
    }

    // 2. Verificar y limpiar bandera de Fin de Transferencia (Terminal Count)
    if (GPDMA_IntGetStatus(GPDMA_INTTC, GPDMA_CH_0)) {
        GPDMA_ClearIntPending(GPDMA_CLR_INTTC, GPDMA_CH_0);

        // Lógica de Ping-Pong
        if (active_dma_buffer == 0) {
            buffer_ready = 1;
            active_dma_buffer = 1;
        } else {
            buffer_ready = 2;
            active_dma_buffer = 0;
        }

        // Vuelve a arrancar el DMA al nuevo buffer
        capture_adc_dma_start();
    }
}
