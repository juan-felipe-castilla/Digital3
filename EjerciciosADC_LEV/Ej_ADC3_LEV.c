	/*
	Configuraremos el módulo ADC de manera que realice conversiones cada 1 segundo, la base de tiempo la crearemos con el módulo Systick. La lectura del dato analógico lo haremos dentro de la rutina de interrupción del módulo ADC.

	Conectaremos 3 leds a la placa de desarrollo LPCXPRESSO y realizaremos la siguiente lógica.

	Si convertimos un dato entre 0 V - 1 V encenderemos el primer led (azul)

	Si convertimos un dato entre 1 V - 2 V encenderemos el segundo led (verde)

	Si convertimos un dato entre 2 V - 3 V encenderemos el tercer led (rojo)

Si convertimos un dato mayor a 3 V parpadearán los 3 leds

SUPONEMOS CLOCK DE 100M
CANAL 1
	 */

	#ifdef __USE_CMSIS
	#include "LPC17xx.h"
	#include "stdio.h"
	#endif

	#include <cr_section_macros.h>

void config_adc(void);
	//////////////////////////////////////////////////////////////////////////
	int main(void) {
		config_adc();
		
		
		
	}

void config_adc(){
	
	//PRENDEMOS
	LPC_SC->PCONP |= (1<<12);
	
	//CLCK EN 100 / 8 = 12.5 M
	
	LPC_SC->PCLKSEL0 |= (1 <<24);

	LPC_SC->PCLKSEL0 |= (1 <<25);
	// MODO ADC
	for(int i = 15; i <= 21 ; i+=2){
	LPC_PINCON->PINSEL1 &= ~(1<< i);
	LPC_PINCON->PINSEL1 |= (1<< i-1);
	
	LPC_PINCON->PINMODE1 &= ~(1<< i-1);
	LPC_PINCON->PINMODE1 |= (1<< i);
	}
	LPC_ADC->ADCR = 0;  // limpio
	LPC_ADC->ADCR |=(1<<0);//canales
	LPC_ADC->ADCR |=(1<<1);
	LPC_ADC->ADCR |=(1<<2);
	LPC_ADC->ADCR |=(1<<3);

	LPC_ADC->ADCR |=(1<<16); // burst, no toco la frecuenta, porq estoy en 12,5 y necesita 64 cloclks
	LPC_ADC->ADCR |=(1<<21);
}