/**
 * @file main.c
 * @brief Main Function in our project
 * Compiles all the different files and calls its functions to deliver the project.
 */

#include "LPC17xx.h"
#include "gpio.h"
#include "systick.h"
#include "timers.h"
#include "uart.h"
#include "dac.h"
#include "adc.h"
#include "interruptions.h"
#include "gdma.h"
#include "lpc17xx_gpdma.h"


/**
 * @brief Main Function
 */
int main(void)
{
	SystemInit();                /* Inicializa el System clock */
    configure_ports();            /* Configuracion puertos GPIO*/
    configure_systick();			/* Configuracion SysTick */
    configure_timer();				/* Configuracion Modulo Timer */
    configure_external_interrupt(); /* Configuracion Modulo de Interrupcion Externa */
    setup_dac();                 /* Configuracion modulo DAC */
    dma_setup();                 /* Inicializacion del Modulo DMA */
    configure_adc();             /* Configuracion de Modulo ADC */
    uart_init();                 /* Inicializacion Modulo UART */

    systick_enable();			/* Habilitacion de Interrupcion de SysTick */


    GPDMA_ChannelCmd(1, ENABLE);  /* Activacion del Canal del DAC con DMA */


    while (1)
        {
            __WFI();  // Espera una interrupción (bajo consumo)
        }

    return 0;
}

