
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

#include "../headers/o-tim2CAP.h"
#include "../headers/variables.h"

void confTIM2(){
	TIM_TIMERCFG_T tim2;
	tim2.prescaleOpt = TIM_US;
	tim2.prescaleValue = 10;							//TMR0 Cuenta c/10 microsegundos
	TIM_InitTimer(LPC_TIM2, &tim2);
}

void confCAP(){
	//Primero configuro el P1.26 en CAP0.0
	PINSEL_CFG_T pin;
	pin.port = PORT_0;
	pin.pin = PIN_4;
	pin.func = PINSEL_FUNC_11;
	pin.mode = PINSEL_TRISTATE; 		//	---------------> PUEDE ESTAR MAL (SIN PULLS)
	pin.openDrain = DISABLE;
	PINSEL_ConfigPin(&pin);

	TIM_CAPTURECFG_T cap0;
	cap0.channel = TIM_CAPTURE_0;
	cap0.risingEn = ENABLE;
	cap0.fallingEn = DISABLE;
	cap0.intEn = ENABLE;					//PRUEBA 2: Interrumpe en 1 flanco.
	TIM_ConfigCapture(LPC_TIM2, &cap0);

	NVIC_EnableIRQ(TIMER2_IRQn);
}

void TIMER2_IRQHandler(void) {
    // Verificar si la interrupción proviene de una captura en el Canal 0
    if (LPC_TIM2->IR & (1 << 4)) {

        // 1. Limpiar el flag de interrupción de captura CR0
        LPC_TIM2->IR = (1 << 4);

        // 2. Máquina de estados de medición usando 'switch'
        switch (estado_captura) {

            case 1:
                // Primer flanco detectado: guardamos el tiempo t1
                t1 = LPC_TIM2->CR0;

               // printf("El valor de t1 es: %d\r\n", t1);
                estado_captura = 2; // Preparamos para el próximo flanco
                break; // <-- CRÍTICO: Evita que el código caiga al 'case 2' en esta misma ejecución



            case 2:
                // Segundo flanco detectado: guardamos el tiempo t2
                t2 = LPC_TIM2->CR0;
               // printf("El valor de t2 es: %d\r\n", t2);
                // Calculamos la diferencia en ticks
                periodo_ticks = t2 - t1;

                // Apagamos el Timer para ahorrar CPU


                // Reiniciamos el estado
                estado_captura = 0;
                nueva_medicion_lista = 1; // Avisamos al main()
                //NVIC_DisableIRQ(TIMER2_IRQn);
                break;

            default:
                // Si el estado es 0 u otro valor, no hacemos nada
                break;
        }
    }
}

