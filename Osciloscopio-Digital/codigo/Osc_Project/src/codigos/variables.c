// variables.c
#include "../headers/variables.h"
#include <cr_section_macros.h> // Necesario para el atributo __BSS

// Definición de buffers en RAM principal
uint8_t filtered_buffer[FILTERED_BUFFER_SIZE];
uint16_t signal_buffer[SAMPLES_PER_CYCLE];

// Definición asignando los bancos de memoria específicos (AHB RAM de la LPC1769)
__BSS(RAM2) uint32_t adc_buffer_A[ADC_BUFFER_SIZE];
__BSS(RAM2) uint32_t adc_buffer_B[ADC_BUFFER_SIZE];

// Inicialización de las banderas de control
volatile uint8_t active_dma_buffer = 0;
volatile uint8_t buffer_ready = 0;

uint8_t contDAC = 0;
uint8_t contDACen = 0;

volatile uint8_t sistema_dac_activo = 0;
volatile uint8_t current_wave = 1;  // 1: Triángulo, 2: Seno, 3: Cuadrada

volatile uint8_t estado_captura = 0;
volatile int t1 = 0;
volatile int t2 = 0;
volatile int periodo_ticks = 1;
volatile uint8_t nueva_medicion_lista = 0; // Bandera de aviso
uint32_t frecuencia_hz = 0;
