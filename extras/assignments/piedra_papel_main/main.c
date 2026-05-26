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
#include <stdbool.h>
#include <stdlib.h>

#include <cr_section_macros.h>

// TODO: insert other include files here
/*
 * Estudiante:Agustin Dalmazzo
 * Carrera: Ing. Electronica
 * Descripción: Este programa prende y apaga un led
 *              conectado al pin P0.22 de forma intermitente
 *
 */

#include "stdio.h"

void retardo(void);

void configurar_GPIO0_entrada();

int valor_leer ();

void espera();

int jugar(int igm);

bool entroJugar = false;

static int input[8]={-3,0,1,-2,2,-2,-2,-2};//armamos el mapeo de los valores de jugadas, usamos -2 como una jugada invalidas
static int resultado[3][3]={{0,-1,1},{1,0,-1},{-1,1,0}};
static int cpugame[4]={0,1,2,3};//matriz de indices de jugadas del micro
int r;

int main(void){
	SystemInit();//poner para configurar sin romper
	configurar_GPIO0_entrada();

	while(1){
		int gv = valor_leer();
		int gm = input[gv];
		//int gm = -3;
		switch(gm){
			case -3:
				espera();
				break;
			case -2:
				printf("Jugada invalida\n");
				break;
			default:
				r = jugar(gm);
				break;
		}
		if (entroJugar){
			switch(r){
				case 0:
					printf("Empate\n");
					break;
				case 1:
					printf("Felicidades\n");
					break;
				default:
					printf("La casa siempre gana\n");
					break;
			}
		}
		retardo();
		entroJugar = false;
		}
	return 0;
}

void configurar_GPIO0_entrada(){
	LPC_PINCON->PINSEL0 &= ~(3<<0);
	LPC_PINCON->PINSEL0 &= ~(3<<2);
	LPC_PINCON->PINSEL0 &= ~(3<<4);


	LPC_PINCON->PINSEL0 &= ~(3<<8);
	LPC_PINCON->PINSEL0 &= ~(3<<10);
	LPC_PINCON->PINSEL0 &= ~(3<<12);


	LPC_GPIO0->FIODIR0 &= ~(7<<0); //configuramos pines como entrada
	LPC_GPIO0->FIODIR0 |= (7<<4);//configuramos pines como salida


	LPC_PINCON-> PINMODE0 |= (1<<1);
	LPC_PINCON-> PINMODE0 &= ~(1<<0);
	LPC_PINCON-> PINMODE0 |= (1<<3);
	LPC_PINCON-> PINMODE0 &= ~(1<<2);
	LPC_PINCON-> PINMODE0 |= (1<<5);
	LPC_PINCON-> PINMODE0 &= ~(1<<4);


	LPC_PINCON-> PINMODE0 |= (1<<9);
	LPC_PINCON-> PINMODE0 &= ~(1<<8);
	LPC_PINCON-> PINMODE0 |= (1<<11);
	LPC_PINCON-> PINMODE0 &= ~(1<<10);
	LPC_PINCON-> PINMODE0 |= (1<<13);
	LPC_PINCON-> PINMODE0 &= ~(1<<12);
}

int valor_leer(){
	//LPC_GPIO0->FIOMASK =0xFFFFFFF8;
	int a = LPC_GPIO0->FIOPIN&0x07;
	//LPC_GPIO0 -> FIOMASK = 0x00000000;
	return a;
}



int jugar(int igm){


	int cpujugada = rand() % 3;
	printf("%i",cpujugada);


	int out = resultado[igm][cpujugada];
	entroJugar = true;
	return out;
}

void espera(){
	printf("Dale\n");
}
void retardo(void){

	uint32_t contador;
	for(contador =0 ; contador<1000000; contador++){};

}

