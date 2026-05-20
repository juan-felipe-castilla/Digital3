/**
 * @file adc.c
 * @brief  this file handles the ADC module configuracion and features
*/

#include "LPC17xx.h"
#include "lpc17xx_adc.h"    /* ADC */
#include "adc.h"

/* Pin Definitions */
//#define ADC_CHANNEL_0 ((uint32_t)(1 << 23)) /* P0.23 connected to ADC Channel 0 */
//#define ADC_CHANNEL_1 ((uint32_t)(1 << 24)) /* P0.24 connected to ADC Channel 1 */

/* GPIO Direction Definitions */
#define INPUT  0
#define OUTPUT 1

/* Define frequency variables */
#define ADC_FREQ 200000 /* 100 kHz */

/* Boolean Values */
#define TRUE  1
#define FALSE 0

//static uint32_t adc_read_value = 0;

/**
 * @brief Configure the ADC to sample the signal to measure at 100 kHz.
 */
void configure_adc(void)
{
    ADC_Init(LPC_ADC, ADC_FREQ); /* Initialize the ADC peripheral with a 100 kHz sampling frequency */          //porque esta en 100kHz?
    ADC_ChannelCmd(LPC_ADC, ADC_CHANNEL_0, ENABLE);  // Habilita el canal 0
    ADC_StartCmd(LPC_ADC, ADC_START_CONTINUOUS);      // Inicia la conversión continua

}
