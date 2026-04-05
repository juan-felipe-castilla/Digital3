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

void config_Systick();
void config_EINT();
void config_PINCON();
void Manejo_acciones();
static int cuenta=0;
static int cont=0;
static volatile int Modo=0;
static volatile int F_run=0;
static int tabla7seg[]={ 0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x67};

int main(void) {
	config_Systick();
	config_EINT();
	config_PINCON();
	while(1){
		Manejo_acciones();
		LPC_GPIO0 ->FIOPIN0 &= ~0x7F;//limpiamos el display
		LPC_GPIO0 ->FIOPIN0 |= tabla7seg[cuenta];//imprimimos el valor de la cuenta
	}
}

void Manejo_acciones(){
	if(F_run == 1){
		if(Modo == 0){//interrupcion cada 1 seg
			if(cont >= 1000){
				cont = 0;
				if(cuenta >= 0 && cuenta < 9){
					cuenta++;
				}
				else{
					cuenta=0;
				}
			}
		}else{//interrupcion cada 1ms
			if(cont >= 1){
				cont = 0;
				if(cuenta >= 0 && cuenta <9){
					cuenta++;
				}else{
					cuenta=0;
				}
			}
		}
	}
}

void SysTick_Handler(){
	cont++;
}


void EINT0_IRQHandler(){
	cuenta =0;
	F_run =0;
	LPC_SC-> EXTINT |= (1 << 0); // Limpiar bandera
}

void EINT1_IRQHandler(){
	F_run =! F_run;
	LPC_SC-> EXTINT |= (1 << 1); // Limpiar bandera
}

void EINT2_IRQHandler(){
	Modo =! Modo;
	LPC_SC->EXTINT |= (1 << 2); // Limpiar bandera
}


void config_Systick(){
	SysTick -> CTRL &= ~(1<<0);//ENABLE
	SysTick -> CTRL |= (1<<1);//TICKINT
	SysTick -> CTRL |= (1<<2);//CLCKSOURCE
	SysTick ->LOAD = 15999;//interrumpe cada 1 ms
	SysTick -> CTRL |= (1<<0); //ENABLE
}

void config_EINT(){
	LPC_SC ->EXTMODE &= ~(1<<0);//por nivel
	LPC_SC ->EXTMODE |= (1<<1);//por edge
	LPC_SC ->EXTMODE |= (1<<2);//por edge

	LPC_SC ->EXTPOLAR |= (1<<0);//se activa cuabdo la entrada esta en alto
	LPC_SC ->EXTPOLAR &= ~(1<<1);// se activa por flanco de bajada
	LPC_SC ->EXTPOLAR &= ~(1<<2);// """"""""""""""""""""""""""""""

	NVIC ->ISER[0]|= (1<<18);
	NVIC ->ISER[0]|= (1<<19);
	NVIC ->ISER[0]|= (1<<20);
}

void config_PINCON(){
	/************ Sel EINT0,1,2 *****************/
	LPC_PINCON ->PINSEL4 &= ~(1<<21);
	LPC_PINCON ->PINSEL4 |= (1<<20);

	LPC_PINCON ->PINSEL4 &= ~(1<<23);
	LPC_PINCON ->PINSEL4 |= (1<<22);

	LPC_PINCON ->PINSEL4 &= ~(1<<25);
	LPC_PINCON ->PINSEL4 |= (1<<24);

	/************ Pull-up/Down *****************/

	LPC_PINCON ->PINMODE4 |= (1<<21);//pull down
	LPC_PINCON ->PINMODE4 |= (1<<20);

	LPC_PINCON ->PINMODE4 &= ~(1<<23);//pull up
	LPC_PINCON ->PINMODE4 &= ~(1<<22);

	LPC_PINCON ->PINMODE4 &= ~(1<<25);//pull up
	LPC_PINCON ->PINMODE4 &= ~(1<<24);

	for(int i=0;i<7;i++){
		LPC_PINCON -> PINSEL0 &= ~(3 << (i * 2));// Limpia 2 bits por pin
		LPC_PINCON -> PINMODE0 &= ~(3 << (i * 2));// Limpia 2 bits por pin
		LPC_GPIO0 -> FIODIR0 |= (1<<i);	//ponemos como salida del 0 al 6
	}
}
