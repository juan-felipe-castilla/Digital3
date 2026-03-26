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

volatile int cont = 20;

int main(void) {
	LPC_GPIO0->FIODIR  |= (1<<22);

    SysTick -> CTRL &= ~(1<<0);//ENABLE
    SysTick -> CTRL |= (1<<1);//TICKINT
    SysTick -> CTRL |= (1<<2);//CLCKSOURCE
    SysTick -> LOAD = 0xF423F; //9999999 jeje
    SysTick -> CTRL |= (1<<0); //ENABLE
    //NVIC -> IP[1] = ; tengo que ver q gilada le ponemos a la prioridad // es 1 por que es la segunda direccion de 32 bits ya que nuestra direccion es 60
    while(1){
    }
    return 0;

}

void SysTick_Handler(void){
	cont--;
	if(cont >= 10){
		printf("apago led\n");
		LPC_GPIO0->FIOSET |= (1<<22);  // apago el led
	}
	else if(cont < 10 && cont > 0){
		printf("prendo led\n");
		LPC_GPIO0->FIOCLR |= (1<<22);  // prendo el led
	}
	else{
		printf("reinicio contador\n");
		cont = 20;
	}
}

