#ifdef __USE_CMSIS
#include "LPC17xx.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_uart.h"
#include "lpc17xx_adc.h"
#include "lpc17xx_gpdma.h"
#include <stdio.h>
#include <string.h>
#endif

// --- CONFIGURACION DE TAMANOS ---
#define SAMPLES 128  // Tamano de los buffers; debe ser potencia de 2.
#define ADC_SAMPLE_RATE_HZ 2000
#define UART_BAUD_RATE 9600

// La senal de entrada es una sinusoide de 100 Hz. Muestreamos a 2 kHz
// (20 muestras por periodo). A 9600 baudios usamos salida binaria:
// 1 byte por muestra permite enviar 500 puntos/s sin saturar la UART.
#define TX_OUTPUT_RATE_HZ 500
#define TX_DECIMATION (ADC_SAMPLE_RATE_HZ / TX_OUTPUT_RATE_HZ)

// Filtro IIR suave: y = y + (x - y) / 2. Filtra ruido sin deformar
// demasiado una senal de 100 Hz muestreada a 2 kHz.
#define ADC_FILTER_SHIFT 1

// Estados para la bandera de procesamiento.
#define NADA_LISTO 0
#define A_LLENO    1
#define B_LLENO    2

// --- BUFFERS GLOBALES ---
// AHB SRAM: zona apropiada para buffers usados por el DMA.
uint32_t Buffer_A_Raw[SAMPLES];
uint32_t Buffer_B_Raw[SAMPLES];
char Buffer_TX_Masa[SAMPLES * 10]; // Texto tipo ">ADC:255\n".

// Variables compartidas con interrupciones.
volatile uint8_t buffer_activo = 0; // 0 = A, 1 = B
volatile uint8_t procesar_bloque = NADA_LISTO;
volatile uint8_t uart_dma_ocupado = 0;

// Prototipos.
void init_hardware_base(void);
void config_GPDMA_ADC(uint32_t *dest_buffer);
void config_GPDMA_UART(char *src_buffer, uint32_t size);
uint8_t adc_filter_8bits(uint16_t adc_raw);

// ---------------------------------------------------------
// MANEJADOR DE INTERRUPCIONES DEL DMA
// ---------------------------------------------------------
void DMA_IRQHandler(void) {
    // Canal 0: bloque ADC completo.
    if (GPDMA_IntGetStatus(GPDMA_INTTC, GPDMA_CH_0) == SET) {
        GPDMA_ClearIntPending(GPDMA_CLR_INTTC, GPDMA_CH_0);

        // Ping-pong: preparo el siguiente buffer y aviso cual se puede procesar.
        if (buffer_activo == 0) {
            config_GPDMA_ADC(Buffer_B_Raw);
            buffer_activo = 1;
            procesar_bloque = A_LLENO;
        } else {
            config_GPDMA_ADC(Buffer_A_Raw);
            buffer_activo = 0;
            procesar_bloque = B_LLENO;
        }
    }

    // Si el ADC por DMA falla, limpio el error y rearmo el buffer activo.
    if (GPDMA_IntGetStatus(GPDMA_INTERR, GPDMA_CH_0) == SET) {
        GPDMA_ClearIntPending(GPDMA_CLR_INTERR, GPDMA_CH_0);
        config_GPDMA_ADC(buffer_activo == 0 ? Buffer_A_Raw : Buffer_B_Raw);
    }

    // Canal 1: fin de transmision UART por DMA.
    if (GPDMA_IntGetStatus(GPDMA_INTTC, GPDMA_CH_1) == SET) {
        GPDMA_ClearIntPending(GPDMA_CLR_INTTC, GPDMA_CH_1);
        uart_dma_ocupado = 0;
    }

    if (GPDMA_IntGetStatus(GPDMA_INTERR, GPDMA_CH_1) == SET) {
        GPDMA_ClearIntPending(GPDMA_CLR_INTERR, GPDMA_CH_1);
        uart_dma_ocupado = 0;
    }
}

// ---------------------------------------------------------
// BUCLE PRINCIPAL
// ---------------------------------------------------------
int main(void) {
    init_hardware_base();

    // Arrancamos el ciclo: DMA apunta al Buffer A.
    config_GPDMA_ADC(Buffer_A_Raw);

    // START=0 y BURST=1 deja al ADC convirtiendo continuamente.
    ADC_StartCmd(ADC_START_CONTINUOUS);
    ADC_BurstEnable();

    while (1) {
        if (procesar_bloque != NADA_LISTO) {
            uint32_t *buffer_a_procesar =
                (procesar_bloque == A_LLENO) ? Buffer_A_Raw : Buffer_B_Raw;
            procesar_bloque = NADA_LISTO;

            // El TX usa un unico buffer; si el DMA UART sigue ocupado,
            // no lo pisamos con texto nuevo.
            if (uart_dma_ocupado) {
                continue;
            }

            uint32_t offset = 0;
            static uint8_t tx_decimador = 0;

            for (int i = 0; i < SAMPLES; i++) {
                // El DMA trae ADGDR completo: dato ADC en bits 4..15.
                uint16_t raw_val = (buffer_a_procesar[i] >> 4) & 0xFFF;
                uint8_t adc_filtrado = adc_filter_8bits(raw_val);

                if (++tx_decimador < TX_DECIMATION) {
                    continue;
                }
                tx_decimador = 0;

                Buffer_TX_Masa[offset++] = (char)adc_filtrado;
            }

            if (offset > 0) {
                config_GPDMA_UART(Buffer_TX_Masa, offset);
            }
        }

        if (procesar_bloque == NADA_LISTO) {
            __WFI();
        }
    }

    return 0;
}

// ---------------------------------------------------------
// FUNCIONES DE HARDWARE Y DMA
// ---------------------------------------------------------

void init_hardware_base(void) {
    // 1. Pines UART0: P0.2 TX, P0.3 RX.
    UART_PinConfig(UART_TX0_P0_2);
    UART_PinConfig(UART_RX0_P0_3);

    // 2. ADC canal 0: P0.23.
    ADC_Init(ADC_SAMPLE_RATE_HZ);
    ADC_BurstDisable();
    ADC_PinConfig(ADC_CHANNEL_0);
    ADC_ChannelEnable(ADC_CHANNEL_0);

    // 3. UART0. El Arduino/Serial Plotter deben usar el mismo baudrate.
    UART_CFG_T UARTConfig;
    UARTConfig.baudRate = UART_BAUD_RATE;
    UARTConfig.dataBits = UART_DBITS_8;
    UARTConfig.parity = UART_PARITY_NONE;
    UARTConfig.stopBits = UART_STOPBIT_1;
    UART_Init(UART0, &UARTConfig);

    // 4. FIFO UART en modo DMA. Este driver no trae UART_FIFOConfigStructInit().
    UART_FIFO_CFG_T fifoCfg;
    fifoCfg.resetRxBuf = ENABLE;
    fifoCfg.resetTxBuf = ENABLE;
    fifoCfg.dmaMode = ENABLE;
    fifoCfg.level = UART_FIFO_TRGLEV0;
    UART_FIFOConfig(UART0, &fifoCfg);
    UART_TxEnable(UART0);

    // 5. Controlador DMA e interrupcion NVIC.
    GPDMA_Init();
    NVIC_ClearPendingIRQ(DMA_IRQn);
    NVIC_EnableIRQ(DMA_IRQn);
}

uint8_t adc_filter_8bits(uint16_t adc_raw) {
    static int32_t filtro = -1;
    uint8_t adc_8bits = (uint8_t)(adc_raw >> 4);

    if (filtro < 0) {
        filtro = adc_8bits;
    } else {
        filtro += ((int32_t)adc_8bits - filtro) >> ADC_FILTER_SHIFT;
    }

    return (uint8_t)filtro;
}

void config_GPDMA_ADC(uint32_t *dest_buffer) {
    GPDMA_Channel_CFG_T GPDMACfg = {0};

    GPDMACfg.channelNum = GPDMA_CH_0;
    GPDMACfg.transferSize = SAMPLES;
    GPDMACfg.type = GPDMA_P2M;
    GPDMACfg.srcMemAddr = 0;
    GPDMACfg.dstMemAddr = (uint32_t)dest_buffer;
    GPDMACfg.srcConn = GPDMA_ADC;
    GPDMACfg.dstConn = GPDMA_ADC; // Valor valido requerido por el driver.
    GPDMACfg.src.width = GPDMA_WORD;
    GPDMACfg.src.burst = GPDMA_BSIZE_1;
    GPDMACfg.src.increment = DISABLE;
    GPDMACfg.dst.width = GPDMA_WORD;
    GPDMACfg.dst.burst = GPDMA_BSIZE_1;
    GPDMACfg.dst.increment = ENABLE;
    GPDMACfg.intTC = ENABLE;
    GPDMACfg.intErr = ENABLE;
    GPDMACfg.linkedList = 0;

    if (GPDMA_SetupChannel(&GPDMACfg) == SUCCESS) {
        GPDMA_ChannelStart(GPDMA_CH_0);
    }
}

void config_GPDMA_UART(char *src_buffer, uint32_t size) {
    GPDMA_Channel_CFG_T GPDMACfg = {0};

    if (size == 0 || uart_dma_ocupado) {
        return;
    }

    GPDMACfg.channelNum = GPDMA_CH_1;
    GPDMACfg.transferSize = size;
    GPDMACfg.type = GPDMA_M2P;
    GPDMACfg.srcMemAddr = (uint32_t)src_buffer;
    GPDMACfg.dstMemAddr = 0;
    GPDMACfg.srcConn = GPDMA_UART0_Tx; // Valor valido requerido por el driver.
    GPDMACfg.dstConn = GPDMA_UART0_Tx;
    GPDMACfg.src.width = GPDMA_BYTE;
    GPDMACfg.src.burst = GPDMA_BSIZE_1;
    GPDMACfg.src.increment = ENABLE;
    GPDMACfg.dst.width = GPDMA_BYTE;
    GPDMACfg.dst.burst = GPDMA_BSIZE_1;
    GPDMACfg.dst.increment = DISABLE;
    GPDMACfg.intTC = ENABLE;
    GPDMACfg.intErr = ENABLE;
    GPDMACfg.linkedList = 0;

    if (GPDMA_SetupChannel(&GPDMACfg) == SUCCESS) {
        uart_dma_ocupado = 1;
        GPDMA_ChannelStart(GPDMA_CH_1);
    }
}
