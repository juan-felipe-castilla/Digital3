

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <cr_section_macros.h>

#include <stdio.h>

void config_pin();
void config_GPIOINT();

uint8_t Numeros[10]={0};
uint8_t lectura = 0;
static int index =0;
static int cont_int = 0;

int main(void) {
	config_pin();
	config_GPIOINT();
	while(1){
	}
}

void EINT3_IRQHandler(){
	lectura = LPC_GPIO0 -> FIOPIN0 & 0xF;//enmascaro y mando a lectura el valor de los pines del 0 al 3
	for(index=9;index>0;index--){
		Numeros[index]=Numeros[index-1];
	}
	Numeros[0]=lectura;
	cont_int++;
	if(cont_int>200){
		if(NVIC->IP[21] < 0xF8){
			NVIC -> IP[21] += (1<<3);//dsiminuimos la prioridad por cada interrupcion
		}
		else{
			NVIC -> ISER[0] &= ~(1<<21);//apagamos interrupcion de gpio
		}
	}
	LPC_GPIOINT->IO0IntClr = 0xF;//limpiamos las banderas
}

void config_GPIOINT(){
	LPC_GPIOINT ->IO0IntEnF |= (1<<0);
	LPC_GPIOINT ->IO0IntEnF |= (1<<1);
	LPC_GPIOINT ->IO0IntEnF |= (1<<2);
	LPC_GPIOINT ->IO0IntEnF |= (1<<3);//un flanco de bajada en cualquiera d elos pines manda la interrupcion
	LPC_GPIOINT ->IO0IntEnR |= (1<<0);
	LPC_GPIOINT ->IO0IntEnR |= (1<<1);
	LPC_GPIOINT ->IO0IntEnR |= (1<<2);
	LPC_GPIOINT ->IO0IntEnR |= (1<<3);


	NVIC -> IP[21] = 0x00;//ponemos la prioridad en 0


	NVIC -> ISER[0] |=(1<<21);//activamos interrupcion de eint3 , osea de gpio
}
void config_pin(){
	//el reset ya tiene todos los pines en gpio
	for(int i =0; i<=7; i++){
		LPC_PINCON -> PINMODE0 |= (1<<i);
		LPC_GPIO0 -> FIODIR0 &= ~(1<<i); // aqui son entradas hasta el pin 7 pero con el reset esto ya esta estipulado
	}
}
