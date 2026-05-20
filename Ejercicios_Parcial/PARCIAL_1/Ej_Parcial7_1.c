	/*
	PULSO DE 0.1uS SENSOR PIN P2.10
	P0.20 AVANCE CINTA
	P0.0.1.2.3 NIVELS DE ALTURA DE 0 A 16mm
	EN 0X10003001 LIM SUP Y 0X10003005 INF
	MMEDICION 400uS DESPUES DE DETENER

	PARA - 10 MEDICIONES - PROMEDIO - COMPARAMOS - SIESTA LEGAL P0.20 1
	 */

	#ifdef __USE_CMSIS
	#include "LPC17xx.h"
	#include "stdio.h"
	#endif

	#include <cr_section_macros.h>
	void config_EINT(void);
	void config_GPIO(void);
	void config_Sys(void);
	void lectura(void);
	void led (void);
	int moviendo(void);
	volatile int datos[10] = {0};
	int dato = 0;
	uint32_t sup = *(volatile uint32_t *)0x10003001;
	uint32_t inf = *(volatile uint32_t *)0x10003005;
	int cont = 0;
	int movimiento = 0;
	typedef enum{
		MOVIENDO,
		LECTURA,
		LED
	}ESTADOS;

	ESTADOS estado = MOVIENDO;
	//////////////////////////////////////////////////////////////////////////
	int main(void) {
	while(1){
		switch(estado){
		case LECTURA:
			lectura();
			break;
		case LED:
			led();
			break;
		default:
			moviendo();
			break;


	}
	}
	}

	void config_EINT(){
		LPC_SC->EXTINT = 0; // LIMPIO FLAG
		LPC_SC->EXTMODE = 1; // POR FLANCO
		LPC_SC->EXTPOLAR = 1; //DE SUBIDA
		NVIC->ISER |=(1 << 18);//ACTIVO
		NVIC->IP[4] |=(1 << 19);// PRIORIDAD ALTA PERO TAMPOCO TAAANTO
		NVIC->IP[4] |=(1 << 20);
		LPC_PINCON->PINSEL4 |= (1<< 20);
		LPC_PINCON->PINSEL4 &= ~(1 << 21);// PONGO EINT0
		LPC_PINCON->PINMODE4 |= (1<<21);//SIN PULL
		LPC_PINCON->PINMODE4 &= ~(1<< 20);

	};

	void config_GPIO(){
		LPC_PINCON->PINSEL0 = 0; //PONGO TODO EN GPIO, YA QUE NO USAMOS NADA MÁS
		LPC_PINCON->PINSEL1 &=  ~(1<< 8); // pin 20 p0
		LPC_PINCON->PINSEL1 &=  ~(1<< 9);

		LPC_PINCON->PINMODE0 |= (1 << 0);
		LPC_PINCON->PINMODE0 |= (1 << 2);// SIN RESISTENCIAS
		LPC_PINCON->PINMODE0 |= (1 << 4);
		LPC_PINCON->PINMODE0 |= (1 << 6);
		LPC_PINCON->PINMODE0 &= ~(1 << 1);
		LPC_PINCON->PINMODE0 &= ~(1 << 3);
		LPC_PINCON->PINMODE0 &= ~(1 << 5);
		LPC_PINCON->PINMODE0 &= ~(1 << 7);

		LPC_PINCON->PINMODE0 |= (1 << 8);
		LPC_PINCON->PINMODE1 &= ~(1 << 9);

		for(int i = 0; i <= 3; i++){
		LPC_GPIO0->FIODIR0 &= ~(1<< i); >// INPUTS P0 0123
		}
		LPC_GPIO0->FIODIR2|= (1<< 4);// OUTPUT P0 20
	}

	void config_Sys(){

		SysTick->LOAD = 9999;
		SysTick->VAL = 0;
		SysTick->CTRL = 7;

	}

	void SysTick_Handler(){
		cont++;}

	void moviendo(){
		LPC_GPIO0->FIOSET2 |= (1<< 4); // PONE EN 1 PIN Y HACE COSAS DE MOVER
	}

	void EINT0_IRQHandler(){
		estado = LECTURA;
		LPC_SC->EXTINT = 0;


	}

	void lectura(){
		LPC_GPIO0->FIOCLR2 |= (1<< 4);
		dato = 0;
		for(int i = 0; i < 10; i++){
		    cont = 0;
		    while(cont < 1); //delay
		    datos[i] = LPC_GPIO0->FIOPIN & 0xF;
		}
		for(int e = 0; e < 10; e++){

			dato += datos[e];//promedio

		}
		dato = dato/10;

		if(dato < sup && dato > inf){
			estado = MOVIENDO;
		}else{

			estado = LED;
		}

	}
	}

	void LED(){

		// encendemos un led en este estado, pero al no estar definido, no lo pongo, seria configurar otro y en este punto setearlo


	}
