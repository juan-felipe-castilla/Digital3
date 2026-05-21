/**
 * @file uart.c
 * @brief UART configuration for data transmission in LPC1769.
 */

#include "LPC17xx.h"
#include "lpc17xx_uart.h"
#include "uart.h"
#include "lpc17xx_adc.h"

#define ADC_CONVERSION_DONE(channel)   ((LPC_ADC->ADSTAT) & (1 << (channel)))

/**
 * @brief Initializes UART0 for communication.
 */
void uart_init(void)
{
    UART_CFG_Type uartConfigStruct;
    UART_FIFO_CFG_Type uartFIFOConfigStruct;

    // Default UART configuration (9600 baudrate, 8 data bits, no parity, 1 stop bit)						//FIJARSE ACA PARA LA MUESTRA DEL UART
    UART_ConfigStructInit(&uartConfigStruct);
    uartConfigStruct.Baud_rate = 9600;  // Set baudrate to 9600

    UART_Init(LPC_UART2, &uartConfigStruct);  // Initialize UART2 with the configuration
    UART_FIFOConfigStructInit(&uartFIFOConfigStruct);  // Default FIFO configuration
    UART_FIFOConfig(LPC_UART2, &uartFIFOConfigStruct);  // Apply FIFO configuration
    UART_TxCmd(LPC_UART2, ENABLE);  // Enable UART transmission
}

/**
 * @brief Sends a 16-bit value via UART.
 * @param data 16-bit value to be sent.
 */
void uart_send(uint16_t data)
{
    char buffer[5];  // Buffer to store up to 5 characters for a 16-bit number                      //CRANEAR UNA BANDA

    // Convert the number to a string (base 10)
    int i = 0;
    if (data == 0) {
        buffer[i++] = '0';  // Handle case when the number is 0
    } else {
        while (data > 0) {
            buffer[i++] = (data % 10) + '0'; // Extract the last digit
            data /= 10;  // Remove the last digit
        }
    }

    // Reverse the string to correct order
    for (int j = 0; j < i / 2; j++) {
        char temp = buffer[j];
        buffer[j] = buffer[i - 1 - j];
        buffer[i - 1 - j] = temp;
    }

    // Add a newline character at the end
    buffer[i++] = '\n';

    // Send each character via UART
    for (int j = 0; j < i; j++) {
        UART_Send(LPC_UART2, (uint8_t *)&buffer[j], 1, BLOCKING);
    }
}

/**
 * @brief Transfers data from the ADC to UART.
 */
void transfer_adc_to_uart(void)
{
	// Read the converted value from ADC channel 0
	uint16_t data_to_send = ADC_ChannelGetData(LPC_ADC, ADC_CHANNEL_0);

	// Send the ADC value via UART
	uart_send(data_to_send);
}
