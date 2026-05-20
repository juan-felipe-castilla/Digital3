/**
 * @file systick.c
 * @brief File that handles the SysTick module in the project, generates an interruption
 * to make the channel selection LEDs blink with a 1-second frequency.
 */

#include "LPC17xx.h"
#include "lpc17xx_systick.h"
#include "lpc17xx_adc.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"
#include "systick.h"

/* Pin Definitions */
#define RED_LED_PIN    ((uint32_t)(1 << 30))   /* Define the pin for the red LED */
#define GREEN_LED_PIN  ((uint32_t)(1 << 31))   /* Define the pin for the green LED */

/* Variables */
extern volatile uint32_t active_pin;
uint32_t counter = 0;

/* Define time variables */
#define SYSTICK_TIME 100 /* Expressed in milliseconds */

/* Function Prototypes */
/**
* @brief Basic configuration for the SysTick module
*/
void configure_systick(void);

/**
* @brief Enables the SysTick exception
*/
void systick_enable(void);

/**
* @brief Overwrites the interrupt handler for SysTick
*/
void SysTick_Handler(void);


/**
 * @brief Configures the SysTick module to generate interrupts at a regular interval.
 */
void configure_systick(void)
{
    SYSTICK_InternalInit(SYSTICK_TIME); /* Initialize the SysTick timer with a time interval of 100 ms */
    SYSTICK_Cmd(ENABLE); /* Enable the SysTick counter */
}

/**
 * @brief Enables SysTick interrupts.
 */
void systick_enable(void){
   	SYSTICK_IntCmd(ENABLE); /* Enable SysTick interrupt */
 }

/**
 * @brief SysTick interrupt handler to toggle LED states.
 * This function is called every time the SysTick timer reaches the defined interval.
 */
void SysTick_Handler(void) {
    SYSTICK_ClearCounterFlag(); // Clear the interrupt flag

    // Increment the counter and reset it after 10 cycles (1 second if SYSTICK_TIME = 100 ms)
    if(counter < 10)
    {
        counter++;
    }
    else
    {
        counter = 0;

        // Check if the active LED is the Red LED
        if(active_pin == RED_LED_PIN && GPIO_ReadValue(PINSEL_PORT_1) & RED_LED_PIN) {                       //Reimplementacion mas Optima
            GPIO_ClearValue(PINSEL_PORT_1, RED_LED_PIN); /* Turn off Red LED */
        }
        else if ((active_pin == RED_LED_PIN) && !(GPIO_ReadValue(PINSEL_PORT_1) & RED_LED_PIN))
        {
            GPIO_SetValue(PINSEL_PORT_1, RED_LED_PIN); /* Turn on Red LED */
        }

        // Check if the active LED is the Green LED
        if (active_pin == GREEN_LED_PIN && GPIO_ReadValue(PINSEL_PORT_1) & GREEN_LED_PIN)
        {
            GPIO_ClearValue(PINSEL_PORT_1, GREEN_LED_PIN); /* Turn off Green LED */
        }
        else if ((active_pin == GREEN_LED_PIN) && !(GPIO_ReadValue(PINSEL_PORT_1) & GREEN_LED_PIN))
        {
            GPIO_SetValue(PINSEL_PORT_1, GREEN_LED_PIN); /* Turn on Green LED */
        }
    }
}
