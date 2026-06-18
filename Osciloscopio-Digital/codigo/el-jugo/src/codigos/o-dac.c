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

#include "../headers/o-dac.h"
#include "../headers/variables.h"


void conf_DAC(){ //OK
	static DAC_CONVERTER_CFG_T dacCfg;
	DAC_Init();   			//Inicializamos el DAC y su pin
	DAC_SetBias(DAC_700uA); //Se banca hasta 1000 KHz
	dacCfg.dmaCounter = ENABLE;
	dacCfg.dmaRequest = ENABLE;
	dacCfg.doubleBuffer = DISABLE;
	DAC_ConfigDAConverterControl(&dacCfg);
	DAC_SetDMATimeOut(300);

	DAC_UpdateValue(0);    //Limpiamos lo que este en el DAC
}

void generate_triangle_in_memory(void) {
    uint32_t i;
    uint16_t valor_amplitud;

    // Rampa de subida: 255 muestras (índices del 0 al 254)
    for(i = 0; i < 255; i++){
        valor_amplitud = i * 4; // Escala de 0 hasta 1016
        signal_buffer[i] = (valor_amplitud << 6); // Corrimiento vital para el DACR
    }

    // Rampa de bajada: 255 muestras (índices del 255 al 509)
    for(i = 255; i < 510; i++){
        valor_amplitud = (510 - i) * 4; // Escala de 1020 bajando hasta 4
        signal_buffer[i] = (valor_amplitud << 6);
    }
}

void generate_square_in_memory(void) {
    uint32_t i;

    // Primer semiciclo: ALTO (1023 corrido 6 posiciones)
    for (i = 0; i < 255; i++) {
    	signal_buffer[i] = (1023 << 6);
    }

    // Segundo semiciclo: BAJO (0 corrido 6 posiciones)
    for (i = 255; i < 510; i++) {
    	signal_buffer[i] = (0 << 6);
    }
}

void generate_sine_in_memory(void) {
    uint32_t i;
    float angle;
    uint16_t valor_amplitud;

    for (i = 0; i < 510; i++) {
        // Calculamos la fracción del círculo (0 a 2*PI)
        angle = (2.0f * M_PI * i) / 510.0f;

        // Escalamos de [-1.0 a 1.0] hacia [0 a 1023]
        valor_amplitud = (uint16_t)((sinf(angle) * 511.5f) + 511.5f);

        // Clip de seguridad: Evita que un error de redondeo del float desborde el DAC
        if (valor_amplitud > 1023) {
            valor_amplitud = 1023;
        }

        // Guardamos con el corrimiento hacia el registro DACR
        signal_buffer[i] = (valor_amplitud << 6);
    }
}

void conf_LED(){
	//0.21, 0.22, 0.27 y 0.28
	PINSEL_CFG_T pin21;
	pin21.port = PORT_0;
	pin21.pin = PIN_21;
	pin21.func = PINSEL_FUNC_00;
	pin21.mode = PINSEL_TRISTATE;
	pin21.openDrain = DISABLE;
	PINSEL_ConfigPin(&pin21);

	PINSEL_CFG_T pin22;
	pin22.port = PORT_0;
	pin22.pin = PIN_22;
	pin22.func = PINSEL_FUNC_00;
	pin22.mode = PINSEL_TRISTATE;
	pin22.openDrain = DISABLE;
	PINSEL_ConfigPin(&pin22);

	PINSEL_CFG_T pin27;
	pin27.port = PORT_0;
	pin27.pin = PIN_27;
	pin27.func = PINSEL_FUNC_00;
	pin27.mode = PINSEL_TRISTATE;
	pin27.openDrain = DISABLE;
	PINSEL_ConfigPin(&pin27);

	PINSEL_CFG_T pin28;
	pin28.port = PORT_0;
	pin28.pin = PIN_28;
	pin28.func = PINSEL_FUNC_00;
	pin28.mode = PINSEL_TRISTATE;
	pin28.openDrain = DISABLE;
	PINSEL_ConfigPin(&pin28);

	GPIO_SetDir(PORT_0, 408944640,GPIO_OUTPUT);
	GPIO_ClearPins(PORT_0, 408944640);
}

