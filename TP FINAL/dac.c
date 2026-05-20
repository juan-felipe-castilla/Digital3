/**
 * @file dac.c
 * @brief File to handle the DAC module
 */

#include "LPC17xx.h"
#include "lpc17xx_dac.h"
#include "dac.h"

// Definitions
#define TRUE             1

// Function Prototypes
void setup_dac(void);
//void create_waveform_table(uint32_t* table, uint32_t num_samples);									//Implementacion Potencial

void setup_dac(void)
{
    DAC_CONVERTER_CFG_Type DAC_Struct;
    uint32_t update_interval;

    // Configure DAC settings
    DAC_Struct.CNT_ENA = SET;  // Enable DAC counter mode (timeout mode)
    DAC_Struct.DMA_ENA = SET;  // Enable DAC DMA mode
    DAC_Init(LPC_DAC);         // Initialize the DAC

    // Calculate sample update interval for the desired waveform frequency
    update_interval = (CLOCK_DAC_MHZ * 1000000) / (WAVE_FREQUENCY * NUM_SAMPLES);                          //MASTICARLO
    DAC_SetDMATimeOut(LPC_DAC, update_interval);  // Set the DAC timeout between samples

    // Apply the DAC configuration
    DAC_ConfigDAConverterControl(LPC_DAC, &DAC_Struct);
}
