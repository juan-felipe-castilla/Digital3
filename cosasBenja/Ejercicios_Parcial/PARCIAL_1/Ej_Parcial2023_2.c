#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

void Config(void);
int INT = 0;
int datos[10] = {0};
int dato = 0;
int lectura = 1;
int Prio = 0;
int main(void){
	
	while(1){

}

void Config(){
	
	//CONFIG GPIO tomamos los primeros 4 bits del p0
	LPC_PINCON->PINSEL0 &= ~(0xFF); // ponemos en gpio
	LPC_PINCON->PINMODE0 &= ~(1 << 0);//sin resistencias de pullup ni down
	LPC_PINCON->PINMODE0 |= (1 << 1);
	LPC_PINCON->PINMODE0 &= ~(1 << 2);//sin resistencias de pullup ni down
	LPC_PINCON->PINMODE0 |= (1 << 3);
	LPC_PINCON->PINMODE0 &= ~(1 << 4);//sin resistencias de pullup ni down
	LPC_PINCON->PINMODE0 |= (1 << 5);
	LPC_PINCON->PINMODE0 &= ~(1 << 6);//sin resistencias de pullup ni down
	LPC_PINCON->PINMODE0 |= (1 << 7);
	LPC_GPIO->FIO0DIR &= ~(0xF); //Ponemos en inuput
	// interrupciones de gpio
	LPC_GPIOINT->IO0IntEnF |= (0xF); //flanco desdendente porq tiene pull up
	NVIC->ISER[0] |= (1 << 21);// habilitamos en nvic
	NVIC->IP[5] &= ~(0x1F << 3);
}
void EINT3_IRQHandler(void){
	INT++;
	
	if(INT == 200){
			Prio ++;
			NVIC->IP[5] &= ~(0x1F << 3);
			NVIC->IP[5] |= (Prio << 3);//bajamos prioridad
			INT = 0;
	}
	if(Prio == 31){NVIC->ICER[0] |= (1 << 21);}//desactivamos
		
	dato = LPC_GPIO->FIO0PIN & 0XF;
	for(int i = 9; i > 0; i--){ // desplaza el array
	    datos[i] = datos[i-1];
	}
	
	datos[0] = dato;
				
					
	LPC_GPIOINT->IO0IntClr |= (0xF); // limpio flags
}




