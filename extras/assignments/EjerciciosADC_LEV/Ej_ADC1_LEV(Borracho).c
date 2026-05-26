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
void config_gpio(void);
void config_sys(void);
void delay(void);
float dato = 0;
int cont = 0;
int contd= 0;
	//////////////////////////////////////////////////////////////////////////
	int main(void) {
		config_adc();
		config_gpio();
		config_sys();
		while(1){
			if (dato < 1) {
			    LPC_GPIO0->FIOCLR0 = 0XFF;
			    LPC_GPIO0->FIOSET0 |= (1<<0);
			}
			else if (dato < 2.0) {
				 LPC_GPIO0->FIOCLR0 = 0XFF;
				 LPC_GPIO0->FIOSET0 |= (1<<1);
			}
			else if (dato < 3.0) {
				 LPC_GPIO0->FIOCLR0 = 0XFF;
				 LPC_GPIO0->FIOSET0 |= (1<<2);
			}
			else {
				 LPC_GPIO0->FIOSET0 = 0xF;
				 delay();
				 LPC_GPIO0->FIOCLR0 = 0XFF;
				 delay();
			}





		}
	}


void config_adc(){
	// power ir pconp

	LPC_SC->PCONP |=(1 << 12);

	//clock clk/8

	LPC_SC->PCLKSEL0 |=(1<< 24);
	LPC_SC->PCLKSEL0 |=(1<< 25);

	//modo adc

	LPC_PINCON->PINSEL1 |= (1 << 16);

	LPC_PINCON->PINSEL1 &= ~(1 << 17);

	LPC_PINCON->PINMODE1 |= (1 << 17);

	LPC_PINCON->PINMODE1 &= ~(1 << 16);

	LPC_ADC->ADCR &= ~(1<<16);

	//INTERRUPCION

	NVIC->ISER |= (1<<22);

	// ADC DURO

	LPC_ADC->ADCR |= (1 << 1);//PRENDEMOS CANAL 1

	for (int i = 8; i <= 15; i++){
	LPC_ADC->ADCR |= (1 << i);}// LO PONGO LO MÁS LENTO POSIBLE

	LPC_ADC->ADCR |= (1 << 21);//PRENDEMOS a/d

	LPC_ADC->ADGDR |= (1 << 1);
	LPC_ADC->ADINTEN |= (1<<1);

}

void config_gpio(){

	for(int i = 1; i <= 5 ; i += 2){
	LPC_PINCON->PINSEL0 &= ~(1 << i-1);
	LPC_PINCON->PINSEL0 &= ~(1 << i);

	LPC_PINCON->PINMODE0 |= (1 << i); // primeros 3 como gpio sin pull
	LPC_PINCON->PINMODE0 &= ~(1 << i-1);

	}
for(int e = 0; e <=3 ; e++){
	LPC_GPIO0->FIODIR0 |= (1<<e);

}

}

void config_sys(){

	SysTick->VAL = 0;
	SysTick->LOAD  = 0xF423F;
	SysTick->CTRL = 7;


}

void ADC_IRQHandler(){
    int adc_val = (LPC_ADC->ADDR1 >> 4) & 0xFFF;
    dato = (adc_val * 3.3) / 4095.0;
	LPC_ADC->ADSTAT;
}

void SysTick_Handler(){
	cont ++;
	contd++;
	if(cont >= 100){
		LPC_ADC->ADCR &= ~(7<<24);
		LPC_ADC->ADCR |= (1<<24);// INICIA LA CONVER
		cont =0;
	}


}

void delay(){

	while(contd <= 80){
	}
	contd = 0;
}
