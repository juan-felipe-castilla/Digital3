/**
 * @file timers.c
 * @brief File to handle the timers module, it uses Timer2 with match
 * to make a buzzer generate a noise every 30 seconds.
 */

#include "LPC17xx.h"
#include "lpc17xx_timer.h"
#include "timers.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_pinsel.h"
#include "gpio.h"


#define BUZZER_PIN ((uint32_t)(1 << 0)) /* P2.0 connected to an active buzzer */


/* Configures Timer2 to generate an interrupt every 30 seconds */
void configure_timer(void)
{
    TIM_TIMERCFG_Type timerConfig;
    TIM_MATCHCFG_Type matchConfig;

    // Timer configuration: counter mode
    timerConfig.PrescaleOption = TIM_PRESCALE_USVAL;  // Prescaler in microseconds
    timerConfig.PrescaleValue = 1000;  // 1 ms (1000 us) as the time base

    // Initialize Timer2 with the specified configuration
    TIM_Init(LPC_TIM2, TIM_TIMER_MODE, &timerConfig);

    // Match configuration to achieve the 30-second interval
    matchConfig.MatchChannel = 0;
    matchConfig.IntOnMatch = ENABLE;  // Enable interrupt on match
    matchConfig.ResetOnMatch = ENABLE;  // Reset timer on match
    matchConfig.StopOnMatch = DISABLE;  // Do not stop timer on match
    matchConfig.ExtMatchOutputType = TIM_EXTMATCH_NOTHING;  // No external match output
    matchConfig.MatchValue = 30000;  // 30 seconds with a 1 ms time base (5000 ms)

    // Configure the match for Timer2
    TIM_ConfigMatch(LPC_TIM2, &matchConfig);

    // Enable the interrupt in the NVIC for Timer2
    NVIC_EnableIRQ(TIMER2_IRQn);

    // Start Timer2
    TIM_Cmd(LPC_TIM2, ENABLE);
}

/* Interrupt handler for Timer2 */
void TIMER2_IRQHandler(void)
{
	TIM_ClearIntPending(LPC_TIM2, TIM_MR0_INT);  // Clear the interrupt flag
  
    	transfer_adc_to_uart();// Call the function to transfer ADC data to UART
	
	GPIO_SetValue(PINSEL_PORT_2, BUZZER_PIN);// Turn on the buzzer connected to P2.0
	for(uint8_t i=0; i<200; i++);  // Small delay to keep buzzer on for a short period                          //POLLING, CORREGIR
	
	GPIO_ClearValue(PINSEL_PORT_2, BUZZER_PIN);// Turn off the buzzer
}
