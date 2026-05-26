/*
IDEA DE UN PROMPT PARA IMPLEMENTAR DAC



Nos encontramos haciendo un trabajo final con la LPC1769. La idea es hacer un osciloscopio digital.



Si no me equivoco ya te he comentado un poco los requerimientos minimos, asi que usa eso como base.



Debo implementar un codigo de DAC para lo siguiente:



Simula la funcion de calibracion de osciloscopio. La idea es conectar la salida del DAC a la entrada del ADC para generar 2 tipos de ondas: Una cuadrada y otra triangular.



Necesito que me digas que limitaciones van a tener estas señales. Yo quiero la maxima resolucion posible y la maxima frrcuencia posible, que supongo que debe ser un comun entre el DAC y el ADC.



Ambos tipos de señal seran transmitidos por DMA.



Tambien quiero dejar puntos de “facil accesibilidad”. Con esto me refiero a lo siguiente:



Vamos a implementar interrupciones externas para ir cambiando los modos del osciloscopio. Una de dichas interrupciones cambiara al modo de DAC-DMA (modo ondas generadas/calibracion). Quiero que me des la forma mas optima de implementar esto (si en el handler inhabilito el DAC y viceversa, o como).



Tambien, una vez en modo DAC quiero que se habilite una interrupcion aparte que cambie entre la señal cuadrada o la triangular. 
 */

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <cr_section_macros.h>
#include <string.h>
#include <stdio.h>

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


