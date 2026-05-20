/**
 * @file interruptions.c
 * @brief File to manage GPIO and External interruptions in the project
 * There's EINT0 and EINT3, both related to the DAC and ADC modules.
 */

#include "LPC17xx.h"
#include "lpc17xx_exti.h"
#include "lpc17xx_adc.h"
#include "gpio.h"
#include "interruptions.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_gpdma.h"
#include "lpc17xx_dac.h"

/* Pin Definitions */
#define FREQ_PIN       ((uint32_t)(1 << 10))  /* P2.10 connected to switch */
#define BUTTON_PIN     ((uint32_t)(1 << 11))  /* P2.11 connected to switch */
#define RED_LED_PIN    ((uint32_t)(1 << 30))  /* P0.30 connected to Red LED */								//PINES GLOBALES> A REVISAR
#define GREEN_LED_PIN  ((uint32_t)(1 << 31))  /* P0.31 connected to Green LED */							//PINES GLOBALES> A REVISAR
#define NUM_SAMPLES    100   // Total number of samples for the triangular wave
#define CLOCK_DAC_MHZ  25    // DAC clock: 25 MHz (CCLK divided by 4)

/* GPIO Direction Definitions */
#define INPUT  0																						//PINES GLOBALES> A REVISAR
#define OUTPUT 1																						//PINES GLOBALES> A REVISAR

/* Boolean Values */
#define TRUE  1
#define FALSE 0

/* Variables */
volatile uint8_t output_mode_uart = FALSE;  // Starts with output on UART
static uint8_t current_frequency_index = 1;  // Start with 60 Hz
static uint32_t WAVE_FREQUENCY;

// Frequencies possible
#define FREQ_120HZ 120        //0
#define FREQ_60HZ  60		  //1
#define FREQ_30HZ  30		  //2

static uint32_t frequencies[] = {FREQ_120HZ, FREQ_60HZ, FREQ_30HZ};

/* Function prototypes */
void configure_external_interrupt(void);
void EINT0_IRQHandler(void);
void EINT3_IRQHandler(void);

/* Functions */

void configure_external_interrupt(void)
{
    EXTI_InitTypeDef exti_cfg;

    /* Configure EINT0 as falling edge */
    exti_cfg.EXTI_Line = EXTI_EINT0;
    exti_cfg.EXTI_Mode = EXTI_MODE_EDGE_SENSITIVE;
    exti_cfg.EXTI_polarity = EXTI_POLARITY_LOW_ACTIVE_OR_FALLING_EDGE;
    EXTI_Config(&exti_cfg);

    NVIC_EnableIRQ(EINT0_IRQn); // Enable EINT0 interrupt in NVIC

    /* Configure EINT3 for BUTTON_PIN as falling edge */
    GPIO_IntCmd(PINSEL_PORT_2, BUTTON_PIN, 1); // Enable interrupt for BUTTON_PIN
    NVIC_EnableIRQ(EINT3_IRQn); // Enable EINT3 interrupt in NVIC
}

void EINT0_IRQHandler(void)
{
	EXTI_ClearEXTIFlag(EXTI_EINT0); /* Clear the interrupt flag */

	toggle_leds(); // Alterna los LEDs para indicar el cambio
}

void EINT3_IRQHandler(void)
{
    GPIO_ClearInt(PINSEL_PORT_2, BUTTON_PIN); /* Clear the interrupt flag */

    // Toggle frequency between 120 Hz, 60 Hz, and 30 Hz
    current_frequency_index = (current_frequency_index + 1) % 3;

    // Change frequency and update the DAC
    uint32_t new_frequency = frequencies[current_frequency_index];
    WAVE_FREQUENCY = new_frequency;

    // Recalculate the update interval for the DAC with the new frequency
    uint32_t update_interval = (CLOCK_DAC_MHZ * 1000000) / (WAVE_FREQUENCY * NUM_SAMPLES);
    DAC_SetDMATimeOut(LPC_DAC, update_interval);  // Update the DAC update interval
}
