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

// TODO: insert other include files here
typedef enum{
	ESPERA,
	ABIERTO,
	ALARMA,
}Estado;

Estado estado_actual=ESPERA;

volatile int lectura = 0;
// TODO: insert other definitions and declarations here
void config_systick();
void config_EINT_GPIO();
void config_GPIO();
void espera();
void alarma();
void abierto();

int main(void) {
	config_systick();
	config_EINT_GPIO();
	config_GPIO();

	while(1){
		switch(estado_actual){
			case ABIERTO:
				abierto();
				break;
			case ALARMA:
				alarma();
				break;
			default:
				espera();
				break;
		}
	}

}

void abierto(){
	if(intentos<2){
		if(cont<3000){
			lectura = (LPC_GPIO2 ->FIOPIN0 & 0x0F);
			if(lectura == 0xAA){
				estado_actual = ESPERA;
				return;
			}else{
				intentos++;
			}
		}
	}
	else{
		estado_actual=ALARMA;
	}
}

void alarma(){
	LPC_GPIO1 -> FIOSET0 |= (1<<11);
}

void espera(){
	intentos = 0;
	lectura = 0;
}



void config_EINT_GPIO(){
	LPC_PINCON -> PINSEL0 &= ~(1<<12);
	LPC_PINCON -> PINSEL0 &= ~(1<<13); // SELECCION GPIO

	LPC_PINCON -> PINMODE0 |= (1<<12);
	LPC_PINCON -> PINMODE0 &= ~(1<<13);

	LPC_GPIOINT->IO0IntEnF |= (1<<6);//HABILITAMOS LA INTERRUPCION POR BAJADA DEL PIN 6 PUERTO 0
}

void config_systick(){
	SysTick -> LOAD = 0xF423F;//systick a 10ms
	SysTick -> CTRL = 0x7;//habilitamos el enable, el tickint y la source
}

void config_GPIO(){
	for(int i=0;i<=8;i+=2){
		LPC_PINCON -> PINSEL4 &= ~(1<<i); // SELECCION GPIO
		LPC_PINCON -> PINSEL4 &= ~(1<<i+1); // SELECCION GPIO

		LPC_PINCON -> PINMODE4 |= (1<<i); // MODO GPIO
		LPC_PINCON -> PINMODE4 &= ~(1<<i+1); // MODO GPIO
	}
	LPC_PINCON -> PINSEL2 &= ~(1<<20); // SELECCION GPIO
	LPC_PINCON -> PINSEL2 &= ~(1<<21); // SELECCION GPIO

	LPC_PINCON -> PINMODE2 |= (1<<20); // MODO GPIO
	LPC_PINCON -> PINMODE2 &= ~(1<<21); // MODO GPIO

	LPC_GPIO2->FIODIR0 = 0x00;
	LPC_GPIO1->FIODIR1 = 0x4;
}
