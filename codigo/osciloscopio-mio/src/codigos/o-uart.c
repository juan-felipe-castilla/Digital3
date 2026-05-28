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
#include "../headers/variables.h"

// Definiciones para la señal
#define RES_BITS 8
#define VAL_MAX 255


void UART0_Init(void) {
    UART_CFG_T UARTConfigStruct;
    UARTConfigStruct.baudRate = 460800;
    UARTConfigStruct.parity = UART_PARITY_NONE;
    UARTConfigStruct.dataBits = UART_DBITS_8;
    UARTConfigStruct.stopBits = UART_STOPBIT_1;

    UART_Init(UART0, &UARTConfigStruct);
    UART_PinConfig(UART_TX0_P0_2);
    UART_PinConfig(UART_RX0_P0_3);
    UART_TxEnable(UART0);
}



void send_adc_data_ascii(void) {
    char msg[16];
    char header[] = "--- Señal Filtrada ---\r\n";
    UART_Send(UART0, (uint8_t *)header, strlen(header), BLOCKING);

    for (uint32_t i = 0; i < FILTERED_BUFFER_SIZE; i++) {
        sprintf(msg, "%u\r\n", filtered_buffer[i]);
        UART_Send(UART0, (uint8_t *)msg, strlen(msg), BLOCKING);
    }
}
