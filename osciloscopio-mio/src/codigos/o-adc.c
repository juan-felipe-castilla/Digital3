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

// Definiciones para la señal
#define ADC_BUFFER_SIZE 4095

// Nuevo tamaño: 2000 / 5 = 400 muestras filtradas
#define FILTERED_BUFFER_SIZE (ADC_BUFFER_SIZE / 5)

// Buffer global para la señal ya filtrada
uint8_t filtered_buffer[FILTERED_BUFFER_SIZE];

// Búfer para almacenar las muestras capturadas del ADC
// Se usa uint32_t para capturar el registro ADGDR completo (incluye datos y flags)
uint32_t adc_buffer[ADC_BUFFER_SIZE];

/**
 * @brief Inicializa el ADC a una frecuencia de muestreo de 200kHz.
 * Configura el canal AD0.0 en el pin P0.23.
 */
void ADC0_Init(void) {
    // Inicializa el periférico ADC con una tasa de muestreo de 200,000 Hz (200kHz)
    // Nota: 200kHz es el límite máximo para el LPC1769.
    ADC_Init(200000);

    // Configura el pin P0.23 como entrada del canal AD0.0
    ADC_PinConfig(ADC_CHANNEL_0);

    // Habilita el canal 0 para las conversiones
    ADC_ChannelEnable(ADC_CHANNEL_0);
}

/**
 * @brief Función auxiliar para ordenar 5 elementos y devolver el central.
 */
uint8_t obtener_mediana_5(uint8_t *ventana) {
    uint8_t aux;
    // Bubble sort rápido para 5 elementos
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
void filtrar_ruido_adc(void) {
    uint8_t ventana[5];
    uint16_t adc_val;
    uint32_t f_idx = 0; // Índice para el nuevo buffer

    // Recorremos los 2000 datos en bloques de a 5
    for (uint32_t i = 0; i < ADC_BUFFER_SIZE; i += 5) {

        // Extraemos 5 muestras consecutivas
        for (int j = 0; j < 5; j++) {
            adc_val = ADC_GDR_RESULT(adc_buffer[i + j]);
            ventana[j] = (uint8_t)(adc_val >> 4);
        }

        // Sacamos la mediana de esas 5 y la guardamos en el nuevo buffer
        filtered_buffer[f_idx] = obtener_mediana_5(ventana);
        f_idx++;
    }
}

/**
 * @brief Filtra los datos capturados y los envía por UART.
 */
void send_adc_data_ascii(void) {
    char msg[16];

    char header[] = "--- Señal Filtrada (400 muestras) ---\r\n";
    UART_Send(UART0, (uint8_t *)header, strlen(header), BLOCKING);

    // 1. Aplicamos el tratamiento de señal para cargar el nuevo buffer
    filtrar_ruido_adc();

    // 2. Transmitimos el buffer ya limpio
    for (uint32_t i = 0; i < FILTERED_BUFFER_SIZE; i++) {
        sprintf(msg, "%u\r\n", filtered_buffer[i]);
        UART_Send(UART0, (uint8_t *)msg, strlen(msg), BLOCKING);
    }
}
