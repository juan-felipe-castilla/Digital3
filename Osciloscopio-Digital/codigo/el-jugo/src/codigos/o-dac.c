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


void conf_DAC(){
	static DAC_CONVERTER_CFG_T dacCfg;
	DAC_Init();   			//Inicializamos el DAC y su pin
	DAC_SetBias(DAC_350uA); //Se banca hasta 400 KHz
	dacCfg.dmaCounter = ENABLE;
	dacCfg.dmaRequest = ENABLE;
	dacCfg.doubleBuffer = DISABLE;
	DAC_ConfigDAConverterControl(&dacCfg);
	DAC_SetDMATimeOut(49); //El periodo de las senales genradas es de 512 muestras y su frecuencia esperada es de 1Khz,por lo tanto
						   //1ms/512 = 1954nS, luego 1954nS/40nS = 49
	DAC_UpdateValue(0);    //Limpiamos lo que este en el DAC
}

void generate_square_in_memory(void) {
    uint32_t i;

    // Mitad del ciclo en ALTO
    for (i = 0; i < SAMPLES_PER_CYCLE / 2; i++) {
        quad_buffer[i] = (uint16_t)DAC_MAX_VALUE;
    }

    // Mitad del ciclo en BAJO
    for (i = SAMPLES_PER_CYCLE / 2; i < SAMPLES_PER_CYCLE; i++) {
        quad_buffer[i] = (uint16_t)DAC_MIN_VALUE;
    }
}

void generate_triangle_in_memory(void) {
    uint32_t i;
    for (i = 0; i < 256; i++) {
        triangle_buffer[i] = (uint8_t)i;
    }
    for (i = 256; i < SAMPLES_PER_CYCLE; i++) {
        triangle_buffer[i] = (uint8_t)(510 - i);
    }
}

void generate_sine_in_memory(void) {
    uint32_t i;

    // El centro de la señal (offset) y la amplitud máxima para no saturar
    float amplitude = DAC_MAX_VALUE / 2.0f;
    float offset = DAC_MAX_VALUE / 2.0f;

    for (i = 0; i < SAMPLES_PER_CYCLE; i++) {
        // Calcular el ángulo en radianes para cada muestra (0 a 2*PI)
        float angle = (2.0f * M_PI * i) / SAMPLES_PER_CYCLE;

        // Generar el valor de la seno, aplicar amplitud, offset y castear a entero
        sine_buffer[i] = (uint16_t)(amplitude * sinf(angle) + offset);
    }
}
