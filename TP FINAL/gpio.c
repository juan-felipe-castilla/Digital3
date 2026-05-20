/**
 * @file gpio.c
 * @brief Configure the ports that are going to be use in this project.
 * There's two GPIO pins, P0.30 and P0.31, on each one there's an LED,
 * and two External Interrupt pins P2.10 and P2.11, to interact with the ADC and DAC module.
 * We declare an output Active Buzzer pin, and the pin from the UART signal will come out of.
*/

#include "LPC17xx.h"
#include "lpc17xx_gpio.h"   /* GPIO handling */
#include "lpc17xx_pinsel.h" /* Pin function selection */
#include "gpio.h"

/* Pin Definitions */
#define CHANNEL_PIN   ((uint32_t)(1 << 10)) /* P2.10 connected to button */
#define TIMESCALE_PIN ((uint32_t)(1 << 11))  /* P2.11 connected to button */
#define RED_LED_PIN ((uint32_t)(1 << 30))  /* P1.30 connected to Red LED */
#define GREEN_LED_PIN ((uint32_t)(1 << 31))  /* P1.31 connected to Green LED */
#define TXD2_PIN ((uint32_t)(1 << 8)) /* P2.8 connected to oscilloscope */
#define BUZZER_PIN ((uint32_t)(1 << 0)) /* P2.0 connected to an active buzzer */


/* Definiciones de Direccion de GPIO */
#define INPUT  0
#define OUTPUT 1

/* Boolean Values */
#define TRUE  1
#define FALSE 0

/* Definiciones de Estados de Flanco */
#define RISING_EDGE 0
#define FALLING_EDGE 1

/* Variables */
volatile uint32_t active_pin = 0; // Variable para almacenar el pin activo

/**
 * @brief function to toggle between one or the other LED
 * as a way to know what ADC channel or oscilloscope channel is active.
 */
void toggle_leds(void);

/**
 * @brief Initialize the GPIO peripheral
 *
 */
void configure_ports(void)
{
    PINSEL_CFG_Type pin_cfg; /* Create a variable to store the configuration of the pin */

    /* Configure the pin P1.30 as GPIO */
    pin_cfg.Portnum = PINSEL_PORT_1;           /* The port number is 1 */
    pin_cfg.Pinnum = PINSEL_PIN_30;            /* The pin number is 31 */
    pin_cfg.Funcnum = PINSEL_FUNC_0;           /* The function number is 0 */
    pin_cfg.Pinmode = PINSEL_PINMODE_PULLUP;   /* The pin mode is pull-up */
    pin_cfg.OpenDrain = PINSEL_PINMODE_NORMAL; /* The pin is in the normal mode */
    PINSEL_ConfigPin(&pin_cfg);

    /* Configure the pin P1.31 as GPIO */
    pin_cfg.Pinnum = PINSEL_PIN_31;            /* The pin number is 31 */
    PINSEL_ConfigPin(&pin_cfg);

    /* Configure the pin P0.26 as DAC */
    pin_cfg.Portnum = PINSEL_PORT_0;
    pin_cfg.Pinnum = PINSEL_PIN_26;            /* The pin number is 26 */
    pin_cfg.Funcnum = PINSEL_FUNC_2;           /* The function number is 2 */
    pin_cfg.Pinmode = PINSEL_PINMODE_TRISTATE;   /* The pin mode is nor pull-up or pull-down */
    PINSEL_ConfigPin(&pin_cfg);

    /* Configure the pin P2.10 as EINT0 */
    pin_cfg.Portnum = PINSEL_PORT_2;           /* The port number is 2 */
    pin_cfg.Pinnum = PINSEL_PIN_10;            /* The pin number is 10 */
    pin_cfg.Funcnum = PINSEL_FUNC_1;           /* The function number is 1 */
    pin_cfg.Pinmode = PINSEL_PINMODE_PULLUP;   /* The pin mode is pull-up */
    PINSEL_ConfigPin(&pin_cfg);

    /* Configure the pin P2.11 as GPIO */
    pin_cfg.Pinnum = PINSEL_PIN_11;            /* The pin number is 11 */
    pin_cfg.Funcnum = PINSEL_FUNC_0;           /* The function number is 0 */
    PINSEL_ConfigPin(&pin_cfg);

    /* Configure the pin P2.0 as GPIO */
    pin_cfg.Pinnum = PINSEL_PIN_0;            /* The pin number is 0 */
    PINSEL_ConfigPin(&pin_cfg);

    /* Configure the pin P2.8 as TXD2 */
    pin_cfg.Portnum = PINSEL_PORT_0;           /* The port number is 0 */
    pin_cfg.Pinnum = PINSEL_PIN_10;            /* The pin number is 10 */
    pin_cfg.Funcnum = PINSEL_FUNC_1;           /* The function number is 1 */
    PINSEL_ConfigPin(&pin_cfg);

    /* Configure the pin P2.9 as RXD2 */
    pin_cfg.Pinnum = PINSEL_PIN_9;            /* The pin number is 9 */
    PINSEL_ConfigPin(&pin_cfg);

    /* Set the pins as outputs */
    GPIO_SetDir(PINSEL_PORT_1, RED_LED_PIN | GREEN_LED_PIN , OUTPUT);
    GPIO_SetDir(PINSEL_PORT_2, BUZZER_PIN, OUTPUT);

    GPIO_SetValue(PINSEL_PORT_1,RED_LED_PIN);   /* Turns on Red LED */
    GPIO_ClearValue(PINSEL_PORT_1, GREEN_LED_PIN);	/* Turns off Green LED */
    GPIO_ClearValue(PINSEL_PORT_2, BUZZER_PIN);		/* Turns off Active Buzzer */

    /* Set the P2.11 as a GPIO Interruption pin */
    GPIO_IntCmd(PINSEL_PORT_2, TIMESCALE_PIN, FALLING_EDGE);

    NVIC_EnableIRQ(EINT3_IRQn); // Enables GPIO Interruption
}

// Turns one LED on, and turns off  the other one, remembering which one is active
void toggle_leds(void){																					//Revisar para OPTIMIZAR

	GPIO_ClearValue(PINSEL_PORT_1, RED_LED_PIN | GREEN_LED_PIN); /* Turns off both LEDs */

	if (active_pin == RED_LED_PIN) {
	    active_pin = GREEN_LED_PIN;
	    GPIO_SetValue(PINSEL_PORT_1, GREEN_LED_PIN); /* Turns off Green Led */
	}
	else
	{
	    active_pin = RED_LED_PIN;
	    GPIO_SetValue(PINSEL_PORT_1, RED_LED_PIN); /* Turns off Red Led */
	}
}
