// variables.h
#ifndef VARIABLES_H
#define VARIABLES_H

#include <stdint.h>
#include <math.h>

// ==========================================
// 1. MACROS Y DEFINICIONES (Configuración)
// ==========================================
#define RES_BITS 8
#define VAL_MAX 255
#define SAMPLES_PER_CYCLE 510
#define ADC_BUFFER_SIZE 4095
#define DAC_MAX_VALUE     255  // 10 bits  // Dado que el objetivo del dac es que vaya al ADC, se enviara por dma un ancho de palabra de
#define DAC_MIN_VALUE     0     // Valor mínimo de la señal
#define M_PI 3.1416

// Nuevo tamaño: 4095 / 5 = 819 muestras filtradas
#define FILTERED_BUFFER_SIZE (ADC_BUFFER_SIZE / 5)

// ==========================================
// 2. DECLARACIÓN DE VARIABLES (extern)
// ==========================================

// Buffers globales para procesamiento (RAM Principal)
extern uint8_t filtered_buffer[FILTERED_BUFFER_SIZE];
extern uint16_t triangle_buffer[SAMPLES_PER_CYCLE];

// Doble buffer para el ADC (Declarados extern, el atributo __BSS va en el .c)
extern uint32_t adc_buffer_A[ADC_BUFFER_SIZE];
extern uint32_t adc_buffer_B[ADC_BUFFER_SIZE];

// Banderas de control del Ping-Pong (Llevan volatile por usarse en IRQs)
extern volatile uint8_t active_dma_buffer;
extern volatile uint8_t buffer_ready;

//VARIABLES PAAR DAC
extern volatile uint8_t sistema_dac_activo;
extern volatile uint8_t canal_dma_actual;  // 1: Triángulo, 2: Seno, 3: Cuadrada

//VARIABLES TMR0CAP
extern volatile uint8_t estado_captura;
extern volatile uint32_t t1;
extern volatile uint32_t t2;
extern volatile uint32_t periodo_ticks;
extern volatile uint8_t nueva_medicion_lista;
extern uint32_t frecuencia_hz;
#endif // VARIABLES_H

