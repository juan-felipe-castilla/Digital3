#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <cr_section_macros.h>
#include <stdio.h>

#include "headers/o-adc.h"
#include "headers/o-dma.h"
#include "headers/o-uart.h"

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
