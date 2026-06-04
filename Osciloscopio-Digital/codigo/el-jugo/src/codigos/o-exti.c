#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <cr_section_macros.h>
#include <string.h>
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

#include"../headers/o-dac.h"
#include"../headers/o-exti.h"
#include "../headers/variables.h"



void conf_EXTI(){ //OK
	EXTI_Init();

	EXTI_CFG_T eint0;
	EXTI_PinConfig(EXTI_EINT0, EXTI_PULLUP);
	eint0.line = EXTI_EINT0;
	eint0.mode = EXTI_EDGE_SENSITIVE;
	eint0.polarity = EXTI_FALLING_EDGE;
	EXTI_Config(&eint0);

	EXTI_CFG_T eint2;
	EXTI_PinConfig(EXTI_EINT2, EXTI_PULLUP);
	eint2.line = EXTI_EINT2;
	eint2.mode = EXTI_EDGE_SENSITIVE;
	eint2.polarity = EXTI_FALLING_EDGE;
	EXTI_Config(&eint2);

	EXTI_CFG_T eint1;
	EXTI_PinConfig(EXTI_EINT1, EXTI_PULLUP);
	eint1.line = EXTI_EINT1;
	eint1.mode = EXTI_EDGE_SENSITIVE;
	eint1.polarity = EXTI_FALLING_EDGE;
	EXTI_Config(&eint1);

	EXTI_EnableIRQ(EXTI_EINT0);
	EXTI_EnableIRQ(EXTI_EINT2);
	EXTI_EnableIRQ(EXTI_EINT1);
}

void EINT0_IRQHandler(void) {
    // 1. Limpiar el flag de la interrupción EINT0
    LPC_SC->EXTINT = (1 << 0);



    if (sistema_dac_activo == 0){
    	sistema_dac_activo = 1;

    // 3. Arrancar el Canal 1 (Triangular) directamente
    GPDMA_ChannelResume(GPDMA_CH_1);
    GPIO_SetPins(PORT_0, 2097152);
    }
    else if (sistema_dac_activo == 1){
    	sistema_dac_activo = 0;
    	GPDMA_ChannelGracefulStop(GPDMA_CH_1);
    	GPIO_ClearPins(PORT_0, 2097152);
    }
}

void EINT1_IRQHandler(void) {
    // 1. Limpiar el flag físico de la interrupción (CRÍTICO)
    LPC_SC->EXTINT = (1 << 1);



    // 2. Frenar el DMA INMEDIATAMENTE.
    // Si no hacemos esto, el DMA va a leer el buffer por la mitad mientras
    // la CPU lo está reescribiendo, generando un ruido espantoso en el DAC.

    // 3. Lógica para ciclar al siguiente canal (1 -> 2 -> 3 -> 1)
    current_wave++;
    if (current_wave > 3) {
        current_wave = 1;
    }

    // 4. Reescribir el buffer en memoria "en caliente"
    if (current_wave == 1) {
        generate_triangle_in_memory(); //ROJO
        GPIO_SetPins(PORT_0, 4194304);
        GPIO_ClearPins(PORT_0, 134217728);
        GPIO_ClearPins(PORT_0, 268435456);
    }
    else if (current_wave == 2) {
        generate_sine_in_memory();    //AMARILLO
        GPIO_ClearPins(PORT_0, 4194304);
        GPIO_SetPins(PORT_0, 134217728);
        GPIO_ClearPins(PORT_0, 268435456);
    }
    else if (current_wave == 3) {
        generate_square_in_memory();   //VERDE
        GPIO_ClearPins(PORT_0, 4194304);
        GPIO_ClearPins(PORT_0, 134217728);
        GPIO_SetPins(PORT_0, 268435456);
    }

    // 5. Volver a arrancar el DMA ahora que los datos nuevos están listos
}



void EINT2_IRQHandler(void) {
    // 1. Limpiar el flag de la interrupción externa
    LPC_SC->EXTINT = (1 << 2);



    // Solo disparamos una nueva lectura si el sistema está inactivo
    if (estado_captura == 0) {
        // 2. Resetear el contador del Timer0 (opcional, pero buena práctica)
       // LPC_TIM2->TCR |= (1 << 1);  // Reset
       // LPC_TIM2->TCR &= ~(1 << 1); // Liberar Reset
    	//NVIC_EnableIRQ(TIMER2_IRQn);
        // 3. Establecer el estado para esperar el primer flanco
        estado_captura = 1;

        // 4. Arrancar el Timer2

    }
}
