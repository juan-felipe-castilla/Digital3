/*
  -- Limpiar Ruido (filtrado por software) ///CUMPLIDO
  -- AGRANDAR EL BUFFER Y COORDINAR CON DMA
  -- Aumentar el Baudeaje
  -- Implementar Single
  -- Cambiar en python las unidades de tiempo y gilada de como se muestra
  -- Corte señal Sinusoidal
  -- Generador de señales y Calibracion(DAC)
  -- Divisor de frecuencia
  -- Tratamiento de señales, calculo de Periodo, Frecuencia, Vpp, etc...
  -- Cambio de Escalas mediante EINT
  -- Atenuador de Señales (admitir señales de mayor frecuencia)
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
#include "../Drivers/inc/lpc17xx_uart.h"
#include "../Drivers/inc/lpc17xx_pinsel.h"
#include "../Drivers/inc/lpc17xx_pwm.h"
#include "../Drivers/inc/lpc17xx_qei.h"
#include "../Drivers/inc/lpc17xx_rit.h"
#include "../Drivers/inc/lpc17xx_rtc.h"
#include "../Drivers/inc/lpc17xx_spi.h"
#include "../Drivers/inc/lpc17xx_ssp.h"
#include "../Drivers/inc/lpc17xx_systick.h"
#include "../Drivers/inc/lpc17xx_timer.h"
#include "../Drivers/inc/lpc17xx_wdt.h"
#include "../Drivers/inc/lpc_types.h"

#include "../headers/o-uart.h"

// Definiciones para la señal
#define RES_BITS 8
#define VAL_MAX 255
#define SAMPLES_PER_CYCLE 510

// Búfer en memoria para almacenar un ciclo de la señal triangular
uint8_t triangle_buffer[SAMPLES_PER_CYCLE];

/**
 * @brief Genera un ciclo completo de una señal triangular en el búfer de memoria.
 * Resolución de 8 bits (valores de 0 a 255).
 */
void generate_triangle_in_memory(void) {
    uint32_t i;

    // Parte ascendente: de 0 a 255
    for (i = 0; i < 256; i++) {
        triangle_buffer[i] = (uint8_t)i;
    }

    // Parte descendente: de 254 a 1 (para completar los 510 puntos del ciclo)
    for (i = 256; i < SAMPLES_PER_CYCLE; i++) {
        triangle_buffer[i] = (uint8_t)(510 - i);
    }
}

/**
 * @brief Inicializa el UART0 a 115200 baudios para una transmisión más rápida de los datos.
 */
void UART0_Init(void) {
    UART_CFG_T UARTConfigStruct;

    // 115200 baudios, 8 bits, sin paridad, 1 bit de parada
    UARTConfigStruct.baudRate = 115200;
    UARTConfigStruct.parity = UART_PARITY_NONE;
    UARTConfigStruct.dataBits = UART_DBITS_8;
    UARTConfigStruct.stopBits = UART_STOPBIT_1;

    UART_Init(UART0, &UARTConfigStruct);

    // Pines P0.2 y P0.3
    UART_PinConfig(UART_TX0_P0_2);
    UART_PinConfig(UART_RX0_P0_3);

    UART_TxEnable(UART0);
}

/**
 * @brief Envía el búfer de la señal a través de UART en formato de texto (ASCII)
 * para que sea legible en una terminal serie.
 */
void send_signal_ascii(void) {
    char msg[10];
    for (int i = 0; i < SAMPLES_PER_CYCLE; i++) {
        sprintf(msg, "%u\r\n", triangle_buffer[i]);
        UART_Send(UART0, (uint8_t *)msg, strlen(msg), BLOCKING);
    }
}

/**
 * @brief Envía el búfer de la señal en formato binario (raw bytes).
 * Más eficiente, pero requiere una herramienta que interprete los bytes.
 */
void send_signal_binary(void) {
    UART_Send(UART0,(uint8_t *)triangle_buffer, SAMPLES_PER_CYCLE, BLOCKING);
}
