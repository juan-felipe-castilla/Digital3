/*
 * Copyright 2022 NXP
 * NXP confidential.
 */

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <cr_section_macros.h>
#include <string.h>
#include <stdio.h>

#include "lpc17xx_uart.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_adc.h"
#include "lpc17xx_gpdma.h"

// Definiciones para la señal
#define RES_BITS 8
#define VAL_MAX 255
#define SAMPLES_PER_CYCLE 510
#define ADC_BUFFER_SIZE 2000

// Búfer en memoria para almacenar un ciclo de la señal triangular
uint8_t triangle_buffer[SAMPLES_PER_CYCLE];

// Búfer para almacenar las muestras capturadas del ADC
// Se usa uint32_t para capturar el registro ADGDR completo (incluye datos y flags)
uint32_t adc_buffer[ADC_BUFFER_SIZE];

// Bandera para indicar fin de transferencia DMA
volatile uint8_t dma_done = 0;

/**
 * @brief Función de captura de ADC mediante DMA.
 * Configura el GPDMA para transferir 2000 muestras desde el ADC a la memoria.
 */
void capture_adc_dma(void) {
    GPDMA_Channel_CFG_T GPDMACfg;
    
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

/**
 * @brief Envía los datos capturados por el ADC a través de UART en formato ASCII.
 * Extrae el valor de 12 bits del registro ADGDR capturado por DMA.
 */
void send_adc_data_ascii(void) {
    char msg[16];
    uint32_t i;
    uint16_t adc_val;
    uint8_t adc_8;
    
    char header[] = "--- Datos Capturados por ADC (2000 muestras) ---\r\n";
    UART_Send(UART0, (uint8_t *)header, strlen(header), BLOCKING);

    for (i = 0; i < ADC_BUFFER_SIZE; i++) {
        // Extrae los bits [15:4] que contienen el resultado de la conversión
        adc_val = ADC_GDR_RESULT(adc_buffer[i]);
        //printf("%d/n",&adc_val);
        adc_8 = (uint8_t)(adc_val >> 4 );
        
        sprintf(msg, "%u\r\n", adc_8);
        UART_Send(UART0, (uint8_t *)msg, strlen(msg), BLOCKING);
    }
}

int main(void) {
	SystemInit();
    // Inicialización
    UART0_Init();
    ADC0_Init(); // Inicializa el ADC a 200kHz en P0.23
    
    // Generar la señal triangular en memoria (se mantiene la función según lo pedido)
    generate_triangle_in_memory();
    
    while(1) {
        // 1. Capturar 2000 muestras del ADC usando DMA
        // El modo ráfaga a 200kHz asegura una captura rápida
        capture_adc_dma();
    	//send_signal_ascii();
        // 2. Transmitir los datos capturados del ADC por UART
        send_adc_data_ascii();
        
        // Pausa entre ráfagas de transmisión (~1 segundo)
        for(volatile int i = 0; i < 2000000; i++);
    }
    return 0;
}
