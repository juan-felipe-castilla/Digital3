/*
 * adc.c
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

#include "../headers/o-adc.h"
#include "../headers/variables.h"

void ADC0_Init(void) {
    ADC_Init(200000);
    ADC_PinConfig(ADC_CHANNEL_0);
    ADC_ChannelEnable(ADC_CHANNEL_0);
}

/**
 * @brief Función auxiliar para ordenar 5 elementos y devolver el central.
 */
uint8_t obtener_mediana_5(uint8_t *ventana) {
    uint8_t aux;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4 - i; j++) {
            if (ventana[j] > ventana[j + 1]) {
                aux = ventana[j];
                ventana[j] = ventana[j + 1];
                ventana[j + 1] = aux;
            }
        }
    }
    return ventana[2];
}

/**
 * @brief Recorre el buffer del ADC en saltos de 5, extrae los valores,
 * calcula la mediana de cada bloque y la guarda en el buffer filtrado.
 */
void filtrar_ruido_adc(uint32_t *buffer_crudo) {
    uint8_t ventana[5];
    uint16_t adc_val;
    uint32_t f_idx = 0;

    for (uint32_t i = 0; i < ADC_BUFFER_SIZE; i += 5) {
        // Se cambió el j < 4 por j < 5 para tomar las 5 muestras correctas
        for (int j = 0; j < 5; j++) {
            adc_val = ADC_GDR_RESULT(buffer_crudo[i + j]);
            ventana[j] = (uint8_t)(adc_val >> 4);
        }
        filtered_buffer[f_idx] = obtener_mediana_5(ventana);
        f_idx++;
    }
}

/**
 * @brief Filtra los datos capturados y los envía por UART.
 */
