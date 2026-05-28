/*
  -- Limpiar Ruido (filtrado por software) ///CUMPLIDO
  -- AGRANDAR EL BUFFER Y COORDINAR CON DMA ///CUMPLIDO (Doble Buffer Implementado)
  -- Implementar Single
  -- Cambiar en python las unidades de tiempo y gilada de como se muestra
  -- Corte señal Sinusoidal
  -- Generador de señales y Calibracion(DAC)
  -- Divisor de frecuencia
  -- Tratamiento de señales, calculo de Periodo, Frecuencia, Vpp, etc...
  -- Cambio de Escalas mediante EINT
  -- Atenuador de Señales (admitir señales de mayor frecuencia)
 */

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <cr_section_macros.h>
#include <string.h>
#include <stdio.h>

#include "lpc17xx_uart.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_adc.h"
#include "lpc17xx_gpdma.h"

// Definiciones para la señal
#define RES_BITS 8
#define VAL_MAX 255
#define SAMPLES_PER_CYCLE 510
#define ADC_BUFFER_SIZE 4095 // <--- Cambiado a 4095

// Nuevo tamaño: 4095 / 5 = 819 muestras filtradas
#define FILTERED_BUFFER_SIZE (ADC_BUFFER_SIZE / 5)

// Buffer global para la señal ya filtrada
uint8_t filtered_buffer[FILTERED_BUFFER_SIZE];
uint8_t triangle_buffer[SAMPLES_PER_CYCLE];

// --- DOBLE BUFFER PARA EL ADC ---
// Buffer global para la señal ya filtrada (Este se queda en la RAM principal)
uint8_t filtered_buffer[FILTERED_BUFFER_SIZE];
uint8_t triangle_buffer[SAMPLES_PER_CYCLE];

// Le decimos al linker que los mande al banco de memoria AHB32 (Peripheral RAM)
__BSS(RAM2) uint32_t adc_buffer_A[ADC_BUFFER_SIZE];
__BSS(RAM2) uint32_t adc_buffer_B[ADC_BUFFER_SIZE];

// Banderas de control para el Ping-Pong
volatile uint8_t active_dma_buffer = 0; // 0 = Llenando A, 1 = Llenando B
volatile uint8_t buffer_ready = 0;      // 0 = Ninguno, 1 = A listo para procesar, 2 = B listo para procesar


/**
 * @brief Configura y arranca el canal DMA apuntando al buffer activo.
 */
/**
 * @brief Configura y arranca el canal DMA apuntando al buffer activo.
 */
void capture_adc_dma_start(void) {
    // --- FIX ABSOLUTO: Inicializar en 0 ---
    // Esto asegura que campos críticos (como el puntero de DMALLI)
    // no arranquen con basura de la RAM y crasheen el micro.
    GPDMA_Channel_CFG_T GPDMACfg = {0};

    // El destapacañerías para evitar el Overrun del ADC
    volatile uint32_t dummy_read = LPC_ADC->ADGDR;

    // Configura los parámetros del canal DMA
    GPDMACfg.channelNum = GPDMA_CH_0;
    GPDMACfg.transferSize = ADC_BUFFER_SIZE;
    GPDMACfg.type = GPDMA_P2M;

    GPDMACfg.srcMemAddr = (uint32_t)&(LPC_ADC->ADGDR);
    GPDMACfg.srcConn = GPDMA_ADC;

    // Selecciona el destino según el buffer activo
    if (active_dma_buffer == 0) {
        GPDMACfg.dstMemAddr = (uint32_t)adc_buffer_A;
    } else {
        GPDMACfg.dstMemAddr = (uint32_t)adc_buffer_B;
    }

    GPDMACfg.dstConn = 0;

    GPDMACfg.src.width = GPDMA_WORD;
    GPDMACfg.src.burst = GPDMA_BSIZE_1;
    GPDMACfg.src.increment = DISABLE;

    GPDMACfg.dst.width = GPDMA_WORD;
    GPDMACfg.dst.burst = GPDMA_BSIZE_1;
    GPDMACfg.dst.increment = ENABLE;

    GPDMACfg.intTC = ENABLE;
    GPDMACfg.intErr = ENABLE;

    // Aplica y arranca
    GPDMA_SetupChannel(&GPDMACfg);
    GPDMA_ChannelStart(GPDMA_CH_0);
}
/**
 * @brief Interrupción del DMA. Se ejecuta automáticamente cada vez que se llena un buffer.
 */
void DMA_IRQHandler(void) {
    // 1. Verificar y LIMPIAR bandera de ERROR (Esto evita que se cuelgue)
    if (GPDMA_IntGetStatus(GPDMA_INTERR, GPDMA_CH_0)) {
        GPDMA_ClearIntPending(GPDMA_CLR_INTERR, GPDMA_CH_0);

        // --- FIX VITAL: REVIVIR EL DMA ---
        // Si hubo error, el DMA se detuvo. Tenemos que volver a arrancarlo
        // forzosamente o no transmite nunca más.
        capture_adc_dma_start();
        return; // Salimos para no ejecutar lo de abajo
    }

    // 2. Verificar y limpiar bandera de Fin de Transferencia (Terminal Count)
    if (GPDMA_IntGetStatus(GPDMA_INTTC, GPDMA_CH_0)) {
        GPDMA_ClearIntPending(GPDMA_CLR_INTTC, GPDMA_CH_0);

        // Lógica de Ping-Pong
        if (active_dma_buffer == 0) {
            buffer_ready = 1;
            active_dma_buffer = 1;
        } else {
            buffer_ready = 2;
            active_dma_buffer = 0;
        }

        // Vuelve a arrancar el DMA al nuevo buffer
        capture_adc_dma_start();
    }
}

// Inicializaciones estándar
void ADC0_Init(void) {
    ADC_Init(200000);
    ADC_PinConfig(ADC_CHANNEL_0);
    ADC_ChannelEnable(ADC_CHANNEL_0);
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

void UART0_Init(void) {
    UART_CFG_T UARTConfigStruct;
    UARTConfigStruct.baudRate = 460800;
    UARTConfigStruct.parity = UART_PARITY_NONE;
    UARTConfigStruct.dataBits = UART_DBITS_8;
    UARTConfigStruct.stopBits = UART_STOPBIT_1;

    UART_Init(UART0, &UARTConfigStruct);
    UART_PinConfig(UART_TX0_P0_2);
    UART_PinConfig(UART_RX0_P0_3);
    UART_TxEnable(UART0);
}

uint8_t obtener_mediana_5(uint8_t *ventana) {
    uint8_t aux;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4 - i; j++) {
            if (ventana[j] > ventana[j + 1]) {
                aux = ventana[j];
                ventana[j] = ventana[j + 1];
                ventana[j + 1] = aux;
            }
        }
    }
    return ventana[2];
}

/**
 * @brief Ahora recibe un puntero al buffer crudo que le toca procesar (A o B).
 */
void filtrar_ruido_adc(uint32_t *buffer_crudo) {
    uint8_t ventana[5];
    uint16_t adc_val;
    uint32_t f_idx = 0;

    for (uint32_t i = 0; i < ADC_BUFFER_SIZE; i += 5) {
        // Se cambió el j < 4 por j < 5 para tomar las 5 muestras correctas
        for (int j = 0; j < 5; j++) {
            adc_val = ADC_GDR_RESULT(buffer_crudo[i + j]);
            ventana[j] = (uint8_t)(adc_val >> 4);
        }
        filtered_buffer[f_idx] = obtener_mediana_5(ventana);
        f_idx++;
    }
}

void send_adc_data_ascii(void) {
    char msg[16];
    char header[] = "--- Señal Filtrada ---\r\n";
    UART_Send(UART0, (uint8_t *)header, strlen(header), BLOCKING);

    for (uint32_t i = 0; i < FILTERED_BUFFER_SIZE; i++) {
        sprintf(msg, "%u\r\n", filtered_buffer[i]);
        UART_Send(UART0, (uint8_t *)msg, strlen(msg), BLOCKING);
    }
}
int main(void) {
    SystemInit();

    UART0_Init();
    ADC0_Init();
    generate_triangle_in_memory();

    // Inicializa el controlador GPDMA y habilita su interrupción en el micro
    GPDMA_Init();
    NVIC_EnableIRQ(DMA_IRQn);

    // Habilita el ADC en modo ráfaga continua
    ADC_IntEnable(ADC_INT_GLOBAL);
    ADC_BurstEnable();

    // Dispara la primera captura del DMA
    capture_adc_dma_start();

    while(1) {
            uint8_t buffer_a_procesar_local = 0;

            // Desactivamos la interrupción un instante para copiar la bandera sin que
            // el DMA nos la sobreescriba a la mitad y arruine la lógica.
            NVIC_DisableIRQ(DMA_IRQn);
            if (buffer_ready != 0) {
                buffer_a_procesar_local = buffer_ready;
                buffer_ready = 0; // Bajamos la bandera global rápido
            }
            NVIC_EnableIRQ(DMA_IRQn);

            // Si hay un buffer listo, lo procesamos sin bloquear al DMA
            if (buffer_a_procesar_local != 0) {

                uint32_t *p_buffer = (buffer_a_procesar_local == 1) ? adc_buffer_A : adc_buffer_B;

                // 1. Filtramos el buffer
                filtrar_ruido_adc(p_buffer);

                // 2. Transmitimos (Este paso es súper lento, pero el DMA sigue trabajando)
                send_adc_data_ascii();
            }
        }

        // Acá podrías poner a dormir el micro (WFI) para ahorrar energía
        // mientras el hardware (ADC + DMA) hace el trabajo pesado.
    return 0;
}
