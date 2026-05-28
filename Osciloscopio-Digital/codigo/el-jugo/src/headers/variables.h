// variables.h
#ifndef VARIABLES_H
#define VARIABLES_H

#include <stdint.h>

// ==========================================
// 1. MACROS Y DEFINICIONES (Configuración)
// ==========================================
#define RES_BITS 8
#define VAL_MAX 255
#define SAMPLES_PER_CYCLE 510
#define ADC_BUFFER_SIZE 4095

// Nuevo tamaño: 4095 / 5 = 819 muestras filtradas
#define FILTERED_BUFFER_SIZE (ADC_BUFFER_SIZE / 5)


// ==========================================
// 2. DECLARACIÓN DE VARIABLES (extern)
// ==========================================

// Buffers globales para procesamiento (RAM Principal)
extern uint8_t filtered_buffer[FILTERED_BUFFER_SIZE];
extern uint8_t triangle_buffer[SAMPLES_PER_CYCLE];

// Doble buffer para el ADC (Declarados extern, el atributo __BSS va en el .c)
extern uint32_t adc_buffer_A[ADC_BUFFER_SIZE];
extern uint32_t adc_buffer_B[ADC_BUFFER_SIZE];

// Banderas de control del Ping-Pong (Llevan volatile por usarse en IRQs)
extern volatile uint8_t active_dma_buffer;
extern volatile uint8_t buffer_ready;

#endif // VARIABLES_H
