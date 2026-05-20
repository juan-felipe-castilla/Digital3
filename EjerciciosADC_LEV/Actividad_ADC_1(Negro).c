/*
 * Copyright 2022 NXP
 * NXP confidential.
 * This software is owned or controlled by NXP and may only be used strictly
 * in accordance with the applicable license terms.  By expressly accepting
 * such terms or by downloading, installing, activating and/or otherwise using
 * the software, you are agreeing that you have read, and that you agree to
 * comply with and are bound by, such license terms.  If you do not agree to
 * be bound by the applicable license terms, then you may not retain, install,
 * activate or otherwise use the software.
 */

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <cr_section_macros.h>

#include <stdio.h>
/*
Configuraremos el módulo ADC de manera que realice conversiones cada 1 segundo, la base de tiempo la crearemos con el módulo Systick. La lectura del dato analógico lo haremos dentro de la rutina de interrupción del módulo ADC.

Conectaremos 3 leds a la placa de desarrollo LPCXPRESSO y realizaremos la siguiente lógica.

Si convertimos un dato entre 0 V - 1 V encenderemos el primer led (azul)

Si convertimos un dato entre 1 V - 2 V encenderemos el segundo led (verde)

Si convertimos un dato entre 2 V - 3 V encenderemos el tercer led (rojo)

Si convertimos un dato mayor a 3 V parpadearán los 3 leds
*/

static int cont=0;
static int out=0;

int main(void) {
	/**********SysTick*****************/
    SysTick -> CTRL &= ~(1<<0);//ENABLE
    SysTick -> CTRL |= (1<<1);//TICKINT
    SysTick -> CTRL |= (1<<2);//CLCKSOURCE
    SysTick -> LOAD = 0xF423F; //9999999 jeje
    SysTick -> CTRL |= (1<<0); //ENABLE
    /**********************************/


	LPC_SC ->PCONP |= (1<<12);//le damos tension al ADC
	LPC_SC -> PCLKSEL0 &= ~(1<<25);//pines 25 y 24 tenemos el PCLK_ADC
	LPC_SC -> PCLKSEL0 &= ~(1<<24);

	LPC_PINCON ->PINSEL0 |= (1<<5);
	LPC_PINCON ->PINSEL0 &= ~(1<<4);//elegimos la funicon del pin 2 del puerto 0 como AD0.7

	LPC_ADC-> ADCR |= (1<<7);//seleccionamos el canal 7 del adc, el cual corresponde a nuestro pin
	LPC_ADC -> ADCR |= (1<<8);//ponemos el CLKDIV para uqe divida por 2, de esta manera, con 100hmz, el PCLK a 25 mhz el adc va a 12,5MHz
	LPC_ADC-> ADCR |= (1<<21);//Ponemos en modo operacional Al ADC con el bit PDN

	//para etsa consigna no hace falta que trabaje en burst

	LPC_ADC-> ADINTEN |= (1<<7);//habilitamos la interrupcion del adc para el canal 7

	NVIC -> ISER[0]|= (1<<22);

	}

void SysTick_Handler(void){
	cont++;
	if(cont>=100){//pasa 1 segundo
		cont = 0;
		LPC_ADC-> ADCR |= (1<<24);//empezamos la conversion con el START
		LPC_ADC-> ADCR &= ~(1<<25);
		LPC_ADC-> ADCR &= ~(1<<26);
	}
}

void ADC_IRQHandler(){
	out =(LPC_ADC->ADDR7 >> 4) & 0xFFF;//Si se lee el registro directamente,en out se buarda un número que incluye banderas de estado.
										//se debe desplazar el resultado 4 bits a la derecha y aplicar una máscara de 12 bits
	if(out<1241 && out>=0){
		printf("Led AZUL/n");
	}
	else if(out<2481 && out>1240){
			printf("Led VERDE/n");
		}
	else if(out<= 3722 && out>2480){
			printf("Led ROJO/n");
		}
	else if(out>3722){
			printf("PARPADEO/n");
		}
}
