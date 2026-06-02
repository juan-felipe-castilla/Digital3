
#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <cr_section_macros.h>
#include <stdio.h>
#include <math.h>

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

#include "headers/o-adc.h"
#include "headers/o-dma.h"
#include "headers/o-uart.h"
#include "headers/o-dac.h"
#include "headers/o-exti.h"
#include "headers/o-tim2CAP.h"
#include "headers/variables.h"


int main(void) {
    SystemInit();

    conf_LED();
    generate_triangle_in_memory();
    GPDMA_Init();
    conf_EXTI();
    conf_DAC();
    UART0_Init();
    ADC0_Init();


    dac_dma();

    // Inicializa el controlador GPDMA y habilita su interrupción en el micro
    NVIC_EnableIRQ(DMA_IRQn);

    // Habilita el ADC en modo ráfaga continua
    ADC_IntEnable(ADC_INT_GLOBAL);
    ADC_BurstEnable();

    // Dispara la primera captura del DMA
    capture_adc_dma_start();

    confTIM2();
    confCAP();

    while(1) {
            uint8_t buffer_a_procesar_local = 0;

            // Desactivamos la interrupción un instante para copiar la bandera sin que
            // el DMA nos la sobreescriba a la mitad y arruine la lógica.
            NVIC_DisableIRQ(DMA_IRQn);
            if (buffer_ready != 0) {
                buffer_a_procesar_local = buffer_ready;
                buffer_ready = 0; // Bajamos la bandera global rápido
            }
            NVIC_EnableIRQ(DMA_IRQn);

            // Si hay un buffer listo, lo procesamos sin bloquear al DMA
            if (buffer_a_procesar_local != 0) {

                uint32_t *p_buffer = (buffer_a_procesar_local == 1) ? adc_buffer_A : adc_buffer_B;

                // 1. Filtramos el buffer
                filtrar_ruido_adc(p_buffer);

                // 2. Transmitimos (Este paso es súper lento, pero el DMA sigue trabajando)
                send_adc_data_ascii();
            }

           //COSA PARA MOSTRAR LA FRECUENCIA 1 SOLA VEZ
            if (nueva_medicion_lista == 1) {
            			printf("%lu",&periodo_ticks);

                        // 1. Calculamos la frecuencia
                        if (periodo_ticks > 0) {
                            frecuencia_hz = 100000 / periodo_ticks;
                        } else {
                            frecuencia_hz = 0;
                        }

                        // 2. Imprimimos en la terminal (printf usa la UART por defecto si el retarget está configurado)
                        printf("La frecuencia es: %lu Hz\n\r", frecuencia_hz);

                        // 3. Bajamos la bandera para que NO se vuelva a imprimir
                        // hasta que el usuario presione EINT2 otra vez
                        nueva_medicion_lista = 0;
             }
        }

        // Acá podrías poner a dormir el micro (WFI) para ahorrar energía
        // mientras el hardware (ADC + DMA) hace el trabajo pesado.
    return 0;
}
