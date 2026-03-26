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

// TODO: insert other definitions and declarations here

void retardo(void);
void configuracionInterrupciones();


int main(void) {
	configuracionInterrupciones();
	while(1){
	}
	return 0;
}

void retardo(void){
	uint32_t contador;
	for(contador =0 ; contador<6000000; contador++){};
}

void configuracionInterrupciones(){
	printf("Configura\n");

	LPC_PINCON -> PINSEL4 |= (1<<20);//a revisar
	LPC_PINCON -> PINSEL4 &= ~(1<<21);

	NVIC->ISER[0] = (1<<18) ;

	// 5. Configurar polaridad: Flanco de BAJADA (EXTPOLAR bit 0 = 0)
	LPC_SC->EXTPOLAR |= (1 << 0);

	// 6. Limpiar cualquier flag previo
	LPC_SC->EXTINT |= (1 << 0);
}

void EINT0_IRQHandler(void){
	printf("interrumpe\n");
	LPC_GPIO0->FIODIR |= (1<<22);     // 0B1000...000 | 0B010101..101 = 1_10101..101

	LPC_GPIO0->FIOSET |= (1<<22);  // prende el led
	retardo();
	LPC_GPIO0->FIOCLR |= (1<<22);  // apaga el led
	retardo();

	LPC_SC->EXTINT |= (1 << 0);
}
