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
	SENSOR,
	INVALIDO,
	ABRIENDO,
	CONFIG,
}Estado;

Estado estado_actual = ESPERA;
int cont = 0,segundos = 0;

// TODO: insert other definitions and declarations here
void config_systick();
void config_EINT();
void config_GPIO();
void espera();
void abriendo();
void config();
void invalido();
void sensor();

int main(void) {
	config_systick();
	config_EINT();
	config_GPIO();

	while(1){
		switch(estado_actual){
		case SENSOR:
			sensor();
			break;
		case INVALIDO:
			invalido();
			break;
		case ABRIENDO:
			abriendo();
			break;
		case CONFIG:
			config();
			break;
		default:
			espera();
			break;
		}
	}
}

void espera(){
	if((LPC_GPIO3 ->FIOPIN0 & 0x10)==1){
		estado_actual = CONFIG;
	}
}

void sensor(){
	if((LPC_GPIO2 ->FIOPIN0 & 0x10)==1){
			estado_actual = ABRIENDO;
		}
	else{
		estado_actual = INVALIDO;
	}
}

void invalido(){
	LPC_GPIO1 -> FIOSET0 = 0x10;
	cont = 0;
	if(cont>300){
		LPC_GPIO1 -> FIOCLR0 = 0x10;
		estado_actual = ESPERA;
	}
}

void abriendo(){
	LPC_GPIO0 -> FIOSET1 = 0x80;
	cont = 0;
	if(cont>segundos){
		LPC_GPIO0 -> FIOCLR1 = 0x80;
		estado_actual=ESPERA;
	}
}

void config(){
	cont = 0;
	segundos = 0;
	while(cont<300){
		if((LPC_GPIO3 ->FIOPIN0 & 0x10)==1){
			segundos += 100;
		}
	}
	estado_actual = ESPERA;
}

void SysTick_Handler(){
	cont++;
}

void EINT0_IRQHandler(void){
	sensor();
}


void config_systick(){
	//configuramos systick
	SysTick->LOAD = 699999;
	SysTick->VAL = 0;
	SysTick ->CTRL = 0xA;

}

void config_EINT(){
	LPC_PINCON -> PINSEL4 |= (1<<20);
	LPC_PINCON -> PINSEL4 &= ~(1<<21);//hay que poner en p2.10 en neither las resistencias de pull
	LPC_PINCON -> PINMODE4 |= (1<<21);
	LPC_PINCON -> PINMODE4 &= ~(1<<20);

	LPC_SC ->EXTINT = 0x1;
	LPC_SC ->EXTPOLAR = 0x1;
	LPC_SC ->EXTMODE = 0x1;

	NVIC ->ISER |= (1<<18);
	NVIC -> IP[4] |= (2<<19);
}

void config_GPIO(){
	LPC_PINCON -> PINMODE0 |= (1<<31);//MODO DE PIN 15 PUERTO 0
	LPC_PINCON -> PINMODE0 &= ~(1<<30);
	LPC_PINCON -> PINSEL0 &= ~(1<<31);
	LPC_PINCON -> PINSEL0 &= ~(1<<30);


	LPC_PINCON -> PINMODE2 |= (1<<9);//PINMODE DE PIN 4 PUERTO 1
	LPC_PINCON -> PINMODE2 &= ~(1<<8);
	LPC_PINCON -> PINSEL2 &= ~(1<<9);
	LPC_PINCON -> PINSEL2 &= ~(1<<8);

	LPC_PINCON -> PINMODE4 |= (1<<9);//PINMODE DE PIN 4 PUERTO 2
	LPC_PINCON -> PINMODE4 &= ~(1<<8);
	LPC_PINCON -> PINSEL4 &= ~(1<<9);
	LPC_PINCON -> PINSEL4 &= ~(1<<8);


	LPC_PINCON -> PINMODE6 |= (1<<9);//PINMODE DE PIN 4 PUERTO 3
	LPC_PINCON -> PINMODE6 &= ~(1<<8);
	LPC_PINCON -> PINSEL6 &= ~(1<<9);
	LPC_PINCON -> PINSEL6 &= ~(1<<8);

	LPC_GPIO0 ->FIODIR1 |= (1<<8);//CONFIGURAMOS COMO SALIDA EL PIN 15 DE P0
	LPC_GPIO1 ->FIODIR0 |= (1<<4);//" PIN 4 DE P1
	LPC_GPIO2 ->FIODIR0 &= ~(1<<4)
	LPC_GPIO3 ->FIODIR0 &= ~(1<<4);// CONFIGURAMOS COMO ENTRADA EL PIN 4 DEL P3

}
