#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

void initGPIO(void);
void initEINT0(void);

int main(void){

    initGPIO();
    initEINT0();

    while(1){
        // loop vacío
    }
}

// ---------------- GPIO ----------------
void initGPIO(void){

    LPC_GPIO0->FIODIR |= (1 << 22);   // LED como salida
    LPC_GPIO0->FIOCLR |= (1 << 22);   // LED apagado
}

// ---------------- EINT0 ----------------
void initEINT0(void){

    // 1. Configurar P2.10 como EINT0
    LPC_PINCON->PINSEL4 &= ~(3 << 20); // limpiar bits
    LPC_PINCON->PINSEL4 |=  (1 << 20); // función EINT0

    // 2. Modo por flanco
    LPC_SC->EXTMODE |= (1 << 0);

    // 3. Flanco de bajada
    LPC_SC->EXTPOLAR &= ~(1 << 0);

    // 4. Limpiar interrupción previa
    LPC_SC->EXTINT = (1 << 0);

    // 5. Habilitar en NVIC (puntero directo)
    NVIC->ISER[0] |= (1 << 18);

    // 6. (Opcional) prioridad
    NVIC->IP[18] = (5 << 3);  // prioridad media (solo 5 bits útiles)

    // 7. Habilitar interrupciones globales
    __asm volatile ("cpsie i");
}

// ---------------- ISR ----------------
void EINT0_IRQHandler(void){

    // Verificar flag
    if (LPC_SC->EXTINT & (1 << 0)){

        LPC_GPIO0->FIOPIN ^= (1 << 22); // toggle LED

        // Limpiar flag (MUY IMPORTANTE)
        LPC_SC->EXTINT = (1 << 0);
    }
}
