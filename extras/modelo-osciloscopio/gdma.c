/**
 * @file dgma.c
 * @brief File to handle the DMA module, it works with the DAC module
 * its goal is to output a triangular wave continiously via Linked Lists.
 */
#include "LPC17xx.h"
#include "lpc17xx_gpdma.h"
#include "dac.h"
#include "gdma.h"

/* Definitions */
#define DMA_BUFFER_SIZE 60  // Tamaño de buffer ajustado a la tabla de onda
#define DMA_SIZE 60
const uint32_t PEAK_VALUE = 1023;  // Valor máximo del DAC para 3.3V (10 bits)
uint32_t step;                     // Paso calculado dinámicamente según la tabla

/* Global Variables */
static uint32_t dac_waveform_table[DMA_BUFFER_SIZE]; /* Tabla de onda triangular para el DAC */
void create_waveform_table(uint32_t* table, uint32_t num_samples);

/* DMA linked list item for continuous transfer */
GPDMA_LLI_Type DMA_LLI_Struct;

/* Function declarations */
void dma_setup(void);

/**
 * @brief Configura el DMA para transferencias de ADC a memoria y DAC.
 */
void dma_setup(void)
{
    /* Inicializa la tabla de onda para el DAC */
    create_waveform_table(dac_waveform_table, DMA_BUFFER_SIZE);

    // Set up the DMA linked list for continuous waveform transfer
    DMA_LLI_Struct.SrcAddr = (uint32_t)dac_waveform_table;           // Source: DAC waveform table
    DMA_LLI_Struct.DstAddr = (uint32_t) & (LPC_DAC->DACR);           // Destination: DAC register
    DMA_LLI_Struct.NextLLI = (uint32_t)&DMA_LLI_Struct;              // Point to itself for continuous transfer
    DMA_LLI_Struct.Control = (DMA_SIZE | (2 << 18)                    // Source width: 32-bit
                             | (2 << 21)                              // Destination width: 32-bit
                             | (1 << 26));                            // Increment source address

    /* Inicializa el controlador de DMA */
    GPDMA_Init();

    /* Configura el DMA para transferencia continua de onda triangular al DAC */
    GPDMA_Channel_CFG_Type dma_dac_config;
    dma_dac_config.ChannelNum = 1;
    dma_dac_config.SrcMemAddr = (uint32_t)dac_waveform_table; // Dirección de la tabla de onda
    dma_dac_config.DstMemAddr = 0;                            // Dirección del DAC
    dma_dac_config.TransferSize = DMA_BUFFER_SIZE;            // Número de datos a transferir
    dma_dac_config.TransferWidth = 0;                          // Tamaño de los datos (32 bits)
    dma_dac_config.TransferType = GPDMA_TRANSFERTYPE_M2P;     // Transferencia de memoria a periférico
    dma_dac_config.SrcConn = 0;                                // Conexión fuente (memoria)
    dma_dac_config.DstConn = GPDMA_CONN_DAC;                   // Conexión destino (DAC)
    dma_dac_config.DMALLI = (uint32_t)&DMA_LLI_Struct;         // Enlaza con la lista vinculada

    GPDMA_Setup(&dma_dac_config);
}

/**
 * @brief Crea una tabla de onda triangular.
 * @param table Dirección de la tabla de onda.
 * @param num_samples Número de muestras.
 */
void create_waveform_table(uint32_t* table, uint32_t num_samples)                         //IMPLEMENTAR OTRAS FORMAS DE ONDA
{
    uint32_t i;
    // Llenar la tabla con valores para una onda triangular
    for (i = 0; i < num_samples / 2; i++) {
        table[i] = i * step;  // Parte ascendente de la onda
    }
    for (i = num_samples / 2; i < num_samples; i++) {
        table[i] = PEAK_VALUE  - ((i - num_samples / 2) * step);  // Parte descendente de la onda
    }

}
