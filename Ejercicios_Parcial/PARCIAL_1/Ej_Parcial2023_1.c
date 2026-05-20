#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

void Config(void);
static int map[10] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x67};
static int num = 0;
static int cont = 0;
int Sal(void);
int E1= 0;
int SF= 1 ;
int pausa = 0;
int reset_activo = 0;


int main(void){
	Config();
	while(1){
		
		if (reset_activo == 1){
			LPC_GPIO->FIOCLR0 = 0x7F;
			LPC_GPIO->FIOSET0 = map[0];
			num = 0;
		}else{
			LPC_GPIO->FIOCLR0 = 0x7F;
			LPC_GPIO->FIOSET0 = map[Sal()];
			if(num >= 9){
				num = 0;
			}
		}
		
		
		
	}
}

	void Config(void){
		//GPIO puerto 0, pines 0 al 6 como salidas
		for(int e = 0; e < 14 ; e +=2){
			LPC_PINCON->PINSEL0 &=~(3 << e); // EN gpio, nehiter y salidas
		}

		for(int i = 1; i < 14; i+= 2){
			LPC_PINCON->PINMODE0 |= (1<<i);
			LPC_PINCON->PINMODE0 &= ~(1<<i-1);
		}


		for(int m = 0; m < 7; m++){
				LPC_GPIO->FIODIR0 |= (1<<m);
			}

		//config de interrupciones
		for (int n = 0 ; n < 3 ; n++)
			NVIC->ISER[0] |= (1<< 18 + n);

			NVIC->IP[18] = 0x00; // EINT0 → máxima prioridad
			NVIC->IP[19] = 0x80; // EINT1 → media
			NVIC->IP[20] = 0xC0; // EINT2 → baja

			// P2.10 → EINT0
			LPC_PINCON->PINSEL4 &= ~(3 << 20);
			LPC_PINCON->PINSEL4 |=  (1 << 20);

			// P2.11 → EINT1
			LPC_PINCON->PINSEL4 &= ~(3 << 22);
			LPC_PINCON->PINSEL4 |=  (1 << 22);

			// P2.12 → EINT2
			LPC_PINCON->PINSEL4 &= ~(3 << 24);
			LPC_PINCON->PINSEL4 |=  (1 << 24);


			// Modo flanco
			LPC_SC->EXTMODE |= (1 << 0); // EINT0
			LPC_SC->EXTMODE |= (1 << 1); // EINT1
			LPC_SC->EXTMODE |= (1 << 2); // EINT2

			// Flanco descendente
			LPC_SC->EXTPOLAR &= ~(1 << 0);
			LPC_SC->EXTPOLAR &= ~(1 << 1);
			LPC_SC->EXTPOLAR &= ~(1 << 2);

			//limpio flags

			LPC_SC->EXTINT |= (1 << 0);
			LPC_SC->EXTINT |= (1 << 1);
			LPC_SC->EXTINT |= (1 << 2);

		//CONFIG SYSTICK

			SysTick->LOAD = 159999;
			SysTick->CTRL = 0x07;
			SysTick->VAL=0;


	}

	void SysTick_Handler(void){
		cont++;

	}

	int Sal(void){
		if(pausa == 0){
		//cont incrementa cada 10ms
		if(SF == 1){
			if(cont <= 100){


			}else{
				cont = 0;
				num ++;
			}

		}else{
			num++;
		}
		return num;
	}else{return num;}
	}

	void EINT2_IRQHandler(void){

		if(SF == 0){

			SF=1;
		}else{SF=0;}
		LPC_SC->EXTINT |= (1 << 2);

	}

	void EINT1_IRQHandler(void){	
		
			pausa = !pausa;
			LPC_SC->EXTINT |= (1 << 1);
		
	}
	
	void EINT0_IRQHandler(void){

	    if(reset_activo == 0){
	        reset_activo = 1;
	        LPC_SC->EXTPOLAR |= (1 << 0); // flanco ascendente

	    }else{
	        reset_activo = 0;
	        LPC_SC->EXTPOLAR &= ~(1 << 0); // flanco descendente
	    }

	    LPC_SC->EXTINT |= (1 << 0); 
	}


