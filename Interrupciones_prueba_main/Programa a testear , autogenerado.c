#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <stdio.h>

static int buttonState;

void EINT3_IRQHandler(void)
{
    if ((LPC_GPIOINT->IO2IntStatR & (1U << 0)) != 0U ||
        (LPC_GPIOINT->IO2IntStatF & (1U << 0)) != 0U) {
        buttonState = ((LPC_GPIO2->FIOPIN & (1U << 0)) != 0U);

        if (buttonState) {
            LPC_GPIO0->FIOCLR = (1U << 22);
            printf("P2.0 = 1 -> PRESIONADO\r\n");
        } else {
            LPC_GPIO0->FIOSET = (1U << 22);
            printf("P2.0 = 0 -> SUELTO\r\n");
        }

        LPC_GPIOINT->IO2IntClr = (1U << 0);
    }
}

int main(void)
{
    // P0.22 como GPIO de salida para el LED interno (activo en bajo)
    LPC_PINCON->PINSEL1 &= ~(3U << 12);
    LPC_GPIO0->FIODIR |= (1U << 22);
    LPC_GPIO0->FIOSET = (1U << 22);

    // P2.0 como GPIO de entrada con pull-down
    LPC_PINCON->PINSEL4 &= ~(3U << 0);
    LPC_PINCON->PINMODE4 &= ~(3U << 0);
    LPC_PINCON->PINMODE4 |= (3U << 0);
    LPC_GPIO2->FIODIR &= ~(1U << 0);

    // Limpia pendientes previas y habilita flancos ascendente y descendente
    LPC_GPIOINT->IO2IntClr = (1U << 0);
    LPC_GPIOINT->IO2IntEnR |= (1U << 0);
    LPC_GPIOINT->IO2IntEnF |= (1U << 0);

    NVIC_EnableIRQ(EINT3_IRQn);

    printf("Debug por interrupcion en P2.0\r\n");
    printf("P2.0 con pull-down interno\r\n");

    buttonState = ((LPC_GPIO2->FIOPIN & (1U << 0)) != 0U);

    if (buttonState) {
        LPC_GPIO0->FIOCLR = (1U << 22);
        printf("Estado inicial: PRESIONADO\r\n");
    } else {
        LPC_GPIO0->FIOSET = (1U << 22);
        printf("Estado inicial: SUELTO\r\n");
    }

    while (1) {
        __WFI();
    }
}