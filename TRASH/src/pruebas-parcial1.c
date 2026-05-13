//LO UNICO QUE FALTA EN EL EJERCICIO ES CONFIGURAR PULL UOS O DOWNS. SEGUN GEMINI, EOL EJERCICIO ESTA BIEN. PARCIAL 2023, EJ1.
//LO UNICO QUE FALTA EN EL EJERCICIO ES CONFIGURAR PULL UOS O DOWNS. SEGUN GEMINI, EOL EJERCICIO ESTA BIEN. PARCIAL 2023, EJ1.
//LO UNICO QUE FALTA EN EL EJERCICIO ES CONFIGURAR PULL UOS O DOWNS. SEGUN GEMINI, EOL EJERCICIO ESTA BIEN. PARCIAL 2023, EJ1.
//LO UNICO QUE FALTA EN EL EJERCICIO ES CONFIGURAR PULL UOS O DOWNS. SEGUN GEMINI, EOL EJERCICIO ESTA BIEN. PARCIAL 2023, EJ1.
//LO UNICO QUE FALTA EN EL EJERCICIO ES CONFIGURAR PULL UOS O DOWNS. SEGUN GEMINI, EOL EJERCICIO ESTA BIEN. PARCIAL 2023, EJ1.
//LO UNICO QUE FALTA EN EL EJERCICIO ES CONFIGURAR PULL UOS O DOWNS. SEGUN GEMINI, EOL EJERCICIO ESTA BIEN. PARCIAL 2023, EJ1.
//LO UNICO QUE FALTA EN EL EJERCICIO ES CONFIGURAR PULL UOS O DOWNS. SEGUN GEMINI, EOL EJERCICIO ESTA BIEN. PARCIAL 2023, EJ1.
//LO UNICO QUE FALTA EN EL EJERCICIO ES CONFIGURAR PULL UOS O DOWNS. SEGUN GEMINI, EOL EJERCICIO ESTA BIEN. PARCIAL 2023, EJ1.
//LO UNICO QUE FALTA EN EL EJERCICIO ES CONFIGURAR PULL UOS O DOWNS. SEGUN GEMINI, EOL EJERCICIO ESTA BIEN. PARCIAL 2023, EJ1.



#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <cr_section_macros.h>

#include <stdio.h>

void configurarPines();
void configurarSystick();
void configurarNVIC();
void mostrar(int);

volatile int contador;
volatile int pausa;
volatile int velocidad;

int main(void) {
	configurarPines();
	configurarNVIC();
	configurarSystick();			//Al final, para no perder tiempo.
	contador = 0;
	pausa = 0;
	velocidad = 0;
	mostrar(0);
	while (1){

    }
    return 0 ;
}

void configurarPines() {
	LPC_PINCON -> PINSEL0 &= ~(0x3FFF << 0);					//esto es 1111111
	LPC_PINCON -> PINSEL4 |= (1<<20);
	LPC_PINCON -> PINSEL4 |= (1<<22);
	LPC_PINCON -> PINSEL4 |= (1<<24);
	LPC_PINCON -> PINSEL4 |= (1<<26);
	LPC_PINCON -> PINSEL4 &= ~(1<<27);
	LPC_PINCON -> PINSEL4 &= ~(1<<25);
	LPC_PINCON -> PINSEL4 &= ~(1<<23);
	LPC_PINCON -> PINSEL4 &= ~(1<<21);						//recordar: se manda un 1 tmb para poner 0.

	LPC_SC -> EXTPOLAR &= ~(1<<0);
	LPC_SC -> EXTPOLAR &= ~(1<<1);
	LPC_SC -> EXTPOLAR &= ~(1<<2);							//EINT0,1,2 se dan por bajos.

	LPC_SC -> EXTMODE &= ~(1<<0);							//EINT0 activado por nivel por la condicion de mantener del ejercicio.
	LPC_SC -> EXTMODE |= (1<<1);
	LPC_SC -> EXTMODE |= (1<<2);							//EINT1 y EINT2 activan por flancos.

}

void configurarSystick() {
	SysTick -> CTRL |= (1<<2);    //clk del micro
	SysTick -> LOAD = 0X00F423FF; //1 segundo a 16MHz
	SysTick -> CTRL |= (1<<1);    //Habilito interrupcion
	SysTick -> CTRL |= (1<<0);    //Arranca a contar
}

void configurarNVIC() {
	NVIC -> ISER[0] |= (1<<18);
	NVIC -> ISER[0] |= (1<<19);
	NVIC -> ISER[0] |= (1<<20);				//Habilito interrupciones EINT0,1,2

	NVIC->IP[18] = (1 << 3);    			// EINT0 Prioridad 1 (La más alta para nosotros)
	NVIC->IP[19] = (2 << 3);    			// EINT1 Prioridad 2 (Media)
	NVIC->IP[20] = (3 << 3);    			// EINT2 Prioridad 3 (La más baja de estas tres)
}

void EINT0_IRQHandler(void){				//DEBERIA: Poner display en 0, mantener mientras la flag sea 1
	contador = 0;
	mostrar(0);

	LPC_SC -> EXTINT |= (1<<0);				//Si sigue mantenido el boton, al salir inmediatamente vuelve a entrar.
}

void EINT1_IRQHandler(void){
	if (pausa == 1)
	{
		pausa = 0;
	}
	else
	{
		pausa = 1;
	}

	LPC_SC -> EXTINT |= (1<<1);
}

void EINT2_IRQHandler(void){
	if (velocidad == 0)
	{
		velocidad = 1;
		SysTick -> LOAD = 0x00003E7F;			//Velocidad 1ms
		SysTick -> VAL  = 0;						//Para que no espere al contador viejo.

	}
	else
	{
		velocidad = 0;
		SysTick -> LOAD = 0x00F423FF;			//Velocidad 1s
		SysTick -> VAL  = 0;						//Para que no espere al contador viejo.
	}

	LPC_SC -> EXTINT |= (1<<2);
}

void SysTick_Handler(void){
	if (pausa == 0)								//No estoy en pausa.
	{
		if (contador >= 9)						//Llegué al limite?
		{
			contador = 0;
			mostrar(contador);
			}
			else
			{
			contador++;
			mostrar(contador);
			}
	}
	else
	{
		mostrar(contador);
	}
}

void mostrar(int a){
	int mascara;
	LPC_GPIO0 -> FIOCLR0 = 0x7F;

	switch (a)
	{
	case 0 : mascara = 0x37;
				break;
	case 1 : mascara = 0x06;
				break;
	case 2 : mascara = 0x5B;
				break;
	case 3 : mascara = 0x4F;
				break;
	case 4 : mascara = 0x66;
				break;
	case 5 : mascara = 0x6D;
				break;
	case 6 : mascara = 0x7D;
				break;
	case 7 : mascara = 0x07;
				break;
	case 8 : mascara = 0x7F;
				break;
	case 9 : mascara = 0x67;
				break;
	}

	LPC_GPIO0 -> FIOSET0 = mascara;
}
