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
	DAC_SetBias(DAC_700uA); //Se banca hasta 400 KHz
	dacCfg.dmaCounter = ENABLE;
	dacCfg.dmaRequest = ENABLE;
	dacCfg.doubleBuffer = DISABLE;
	DAC_ConfigDAConverterControl(&dacCfg);
	DAC_SetDMATimeOut(300); //El periodo de las senales genradas es de 512 muestras y su frecuencia esperada es de 1Khz,por lo tanto
						   //1ms/512 = 1954nS, luego 1954nS/40nS = 49
	DAC_UpdateValue(0);    //Limpiamos lo que este en el DAC
}

void generate_triangle_in_memory(void) {
    uint32_t i;
    uint16_t valor_amplitud;

    // Rampa de subida: 255 muestras (índices del 0 al 254)
    for(i = 0; i < 255; i++){
        valor_amplitud = i * 4; // Escala de 0 hasta 1016
        triangle_buffer[i] = (valor_amplitud << 6); // Corrimiento vital para el DACR
    }

    // Rampa de bajada: 255 muestras (índices del 255 al 509)
    for(i = 255; i < 510; i++){
        valor_amplitud = (510 - i) * 4; // Escala de 1020 bajando hasta 4
        triangle_buffer[i] = (valor_amplitud << 6);
    }
}

