// variables.c
#include "../headers/variables.h"
#include <cr_section_macros.h> // Necesario para el atributo __BSS

// Definición de buffers en RAM principal
uint8_t filtered_buffer[FILTERED_BUFFER_SIZE];
uint8_t triangle_buffer[SAMPLES_PER_CYCLE];

// Definición asignando los bancos de memoria específicos (AHB RAM de la LPC1769)
__BSS(RAM2) uint32_t adc_buffer_A[ADC_BUFFER_SIZE];
__BSS(RAM2) uint32_t adc_buffer_B[ADC_BUFFER_SIZE];

// Inicialización de las banderas de control
volatile uint8_t active_dma_buffer = 0;
volatile uint8_t buffer_ready = 0;
