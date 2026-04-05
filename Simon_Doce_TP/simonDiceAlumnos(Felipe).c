#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <cr_section_macros.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
/* =========================
   CONFIGURACIÓN GENERAL
   ========================= */

#define MAX_SECUENCIA 10
#define CANT_BOTONES  4
#define CANT_LEDS     4

#define TIEMPO_LED_ON   1000
#define TIEMPO_LED_OFF   300

/* =========================
   BOTONES - P0.0 a P0.3
   ========================= */

#define BOTON_0    (1 << 0)   // P0.0
#define BOTON_1    (1 << 1)   // P0.1
#define BOTON_2    (1 << 2)   // P0.2
#define BOTON_3    (1 << 3)   // P0.3

#define MASK_BOTONES   (BOTON_0 | BOTON_1 | BOTON_2 | BOTON_3)

/* =========================
   LEDS - P2.0, P2.1, P2.2, P2.3
   ========================= */

const uint32_t led_mask[CANT_LEDS] = {
    (1 << 0),   // LED 0 -> P2.0
    (1 << 1),   // LED 1 -> P2.1
    (1 << 2),   // LED 2 -> P2.2
    (1 << 3)    // LED 3 -> P2.3
};

/* =========================
   TIPOS DE DATOS
   ========================= */

typedef enum
{
    ESTADO_IDLE = 0,
    ESTADO_INICIO,
    ESTADO_GENERAR_PASO,
    ESTADO_MOSTRAR_SECUENCIA,
    ESTADO_ESPERAR_JUGADOR,
    ESTADO_VALIDAR_JUGADA,
    ESTADO_RONDA_SUPERADA,
    ESTADO_GAME_OVER,
    ESTADO_VICTORIA
} estado_juego_t;

typedef enum
{
    SUB_ENCENDER_LED = 0,
    SUB_ESPERAR_LED_ON,
    SUB_APAGAR_LED,
    SUB_ESPERAR_LED_OFF
} subestado_mostrar_t;

/* =========================
   PROTOTIPOS
   ========================= */

void config_botones(void);
void leds_init(void);
void leds_apagar_todos(void);
void led_encender(uint8_t indice);
void led_apagar(uint8_t indice);
void led_mostrar(uint8_t indice);
uint8_t tiempo_cumplido(uint32_t referencia, uint32_t demora_ms);
uint8_t boton_sigue_presionado(uint8_t boton);
void demora_150ms(void);
void systick_init(void);

/* =========================
   VARIABLES DEL JUEGO
   ========================= */

volatile estado_juego_t estado_actual = ESTADO_IDLE;

/* Secuencia que genera la placa */
uint8_t secuencia[MAX_SECUENCIA] = {0};

/* Cantidad actual de pasos válidos en la secuencia */
uint8_t longitud_secuencia = 0;

/* Índice usado para mostrar la secuencia */
uint8_t indice_mostrar = 0;

/* Índice usado para validar la entrada del jugador */
uint8_t indice_jugador = 0;

/* Último botón presionado por el jugador */
volatile int8_t boton_presionado = -1;

/* Bandera para indicar que hubo una nueva pulsación */
volatile uint8_t evento_boton = 0;

/* Bandera de inicio de juego */
volatile uint8_t evento_start = 0;

/* Puntaje o nivel actual */
uint8_t nivel = 0;

/* Variable auxiliar para saber si hubo error */
uint8_t error_jugada = 0;

/* =========================
   TEMPORIZACIÓN
   ========================= */

volatile uint32_t ticks_ms = 0;
uint32_t tiempo_referencia = 0;

/* Subestado para mostrar la secuencia */
subestado_mostrar_t subestado_mostrar = SUB_ENCENDER_LED;

/* Habilitación de lectura del usuario */
volatile uint8_t habilitar_lectura_usuario = 0;

/* =========================
   MAIN
   ========================= */

int main(void)
{
    leds_init();
    config_botones();
    systick_init();

    while (1)
    {
        switch (estado_actual)
        {
            case ESTADO_IDLE:
            {
                leds_apagar_todos();
                habilitar_lectura_usuario = 0;

                if (evento_start)
                {
                	srand(ticks_ms);// semilla para asegurar aleatoriedad
                    evento_start = 0;
                    estado_actual = ESTADO_INICIO;
                }
                break;
            }

            case ESTADO_INICIO:
            {
                longitud_secuencia = 0;
                indice_mostrar = 0;
                indice_jugador = 0;
                boton_presionado = -1;
                evento_boton = 0;
                nivel = 0;
                error_jugada = 0;

                tiempo_referencia = 0;
                subestado_mostrar = SUB_ENCENDER_LED;
                habilitar_lectura_usuario = 0;

                leds_apagar_todos();

                estado_actual = ESTADO_GENERAR_PASO;
                break;
            }

            case ESTADO_GENERAR_PASO:
                    {

                        secuencia[longitud_secuencia] = rand() % CANT_LEDS; // le metemos el random

                        longitud_secuencia++;// agrandamos secuencia

                        indice_mostrar = 0;
                        subestado_mostrar = SUB_ENCENDER_LED;

                        estado_actual = ESTADO_MOSTRAR_SECUENCIA;

                        break;
                    }


            case ESTADO_MOSTRAR_SECUENCIA:
            {

            	habilitar_lectura_usuario = 0;

                     switch (subestado_mostrar)
                            {
                                case SUB_ENCENDER_LED:
                                {
                                    led_encender(secuencia[indice_mostrar]);
                                    tiempo_referencia = ticks_ms;
                                    subestado_mostrar = SUB_ESPERAR_LED_ON;
                                    break;
                                }

                                case SUB_ESPERAR_LED_ON:
                                {
                                    if (tiempo_cumplido(tiempo_referencia, TIEMPO_LED_ON))
                                    {
                                        subestado_mostrar = SUB_APAGAR_LED;
                                    }
                                    break;
                                }

                                case SUB_APAGAR_LED:
                                {
                                    led_apagar(secuencia[indice_mostrar]);
                                    tiempo_referencia = ticks_ms;
                                    subestado_mostrar = SUB_ESPERAR_LED_OFF;
                                    break;
                                }

                                case SUB_ESPERAR_LED_OFF:
                                {
                                    if (tiempo_cumplido(tiempo_referencia, TIEMPO_LED_OFF))
                                    {
                                        indice_mostrar++;
                                        if (indice_mostrar < longitud_secuencia)
                                        {
                                            subestado_mostrar = SUB_ENCENDER_LED; // volvemos al principio
                                        }
                                        else
                                        {
                                            // final del sub
                                            indice_jugador = 0;
                                            subestado_mostrar = SUB_ENCENDER_LED;
                                            estado_actual = ESTADO_ESPERAR_JUGADOR;
                                        }
                                    }
                                    break;
                                }

                                default:
                                {
                                    subestado_mostrar = SUB_ENCENDER_LED;
                                    break;
                                }
                            }

                            break;
                        }



            case ESTADO_ESPERAR_JUGADOR:
                        {
                 habilitar_lectura_usuario = 1;

                            if (evento_boton)
                            {
                                led_encender(boton_presionado);

                                while (boton_sigue_presionado(boton_presionado))
                                {
                                //esperamos a que suelte el botón
                                }

                                led_apagar(boton_presionado);

                                estado_actual = ESTADO_VALIDAR_JUGADA;
                            }

                            break;
                        }


            case ESTADO_VALIDAR_JUGADA:
                        {
                evento_boton = 0;


                            if (boton_presionado == secuencia[indice_jugador])
                            {

                                indice_jugador++; //correcto


                                if (indice_jugador == longitud_secuencia)
                                {
                                    estado_actual = ESTADO_RONDA_SUPERADA;
                                }// vemo como venimo
                                else
                                {

                                    estado_actual = ESTADO_ESPERAR_JUGADOR;
                                }
                            }
                            else
                            {

                                estado_actual = ESTADO_GAME_OVER;//fail
                            }


                            boton_presionado = -1;

                            break;
                        }


        case ESTADO_RONDA_SUPERADA:
        {
                    nivel++;


                    if (longitud_secuencia >= MAX_SECUENCIA)
                    {

                        estado_actual = ESTADO_VICTORIA;
                    }
                    else
                    {

                        estado_actual = ESTADO_GENERAR_PASO;
                    }

                    break;
                }

      case ESTADO_GAME_OVER:
        {
            printf("\n\n");
            printf("⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣀⣠⡀⠀\n");
            printf("⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣤⣤⠀⠀⠀⢀⣴⣿⡶⠀⣾⣿⣿⡿⠟⠛⠁\n");
            printf("⠀⠀⠀⠀⠀⠀⣀⣀⣄⣀⠀⠀⠀⠀⣶⣶⣦⠀⠀⠀⠀⣼⣿⣿⡇⠀⣠⣿⣿⣿⠇⣸⣿⣿⣧⣤⠀⠀⠀\n");
            printf("⠀⠀⢀⣴⣾⣿⡿⠿⠿⠿⠇⠀⠀⣸⣿⣿⣿⡆⠀⠀⢰⣿⣿⣿⣷⣼⣿⣿⣿⡿⢀⣿⣿⡿⠟⠛⠁⠀⠀\n");
            printf("⠀⣴⣿⡿⠋⠁⠀⠀⠀⠀⠀⠀⢠⣿⣿⣹⣿⣿⣿⣿⣿⣿⡏⢻⣿⣿⢿⣿⣿⠃⣼⣿⣯⣤⣴⣶⣿⡤⠀\n");
            printf("⣼⣿⠏⠀⣀⣠⣤⣶⣾⣷⠄⣰⣿⣿⡿⠿⠻⣿⣯⣸⣿⡿⠀⠀⠀⠁⣾⣿⡏⢠⣿⣿⠿⠛⠋⠉⠀⠀⠀\n");
            printf("⣿⣿⠲⢿⣿⣿⣿⣿⡿⠋⢰⣿⣿⠋⠀⠀⠀⢻⣿⣿⣿⠇⠀⠀⠀⠀⠙⠛⠀⠀⠉⠁⠀⠀⠀⠀⠀⠀⠀\n");
            printf("⠹⢿⣷⣶⣿⣿⠿⠋⠀⠀⠈⠙⠃⠀⠀⠀⠀⠀⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n");
            printf("⠀⠀⠈⠉⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣀⣤⣤⣴⣶⣦⣤⡀⠀\n");
            printf("⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣀⡀⠀⠀⠀⠀⠀⠀⠀⣠⡇⢰⣶⣶⣾⡿⠷⣿⣿⣿⡟⠛⣉⣿⣿⣿⠆\n");
            printf("⠀⠀⠀⠀⠀⠀⢀⣤⣶⣿⣿⡎⣿⣿⣦⠀⠀⠀⢀⣤⣾⠟⢀⣿⣿⡟⣁⠀⠀⣸⣿⣿⣤⣾⣿⡿⠛⠁⠀\n");
            printf("⠀⠀⠀⠀⣠⣾⣿⡿⠛⠉⢿⣦⠘⣿⣿⡆⠀⢠⣾⣿⠋⠀⣼⣿⣿⣿⠿⠷⢠⣿⣿⣿⠿⢻⣿⣧⠀⠀⠀\n");
            printf("⠀⠀⠀⣴⣿⣿⠋⠀⠀⠀⢸⣿⣇⢹⣿⣷⣰⣿⣿⠃⠀⢠⣿⣿⢃⣀⣤⣤⣾⣿⡟⠀⠀⠀⢻⣿⣆⠀⠀\n");
            printf("⠀⠀⠀⣿⣿⡇⠀⠀⢀⣴⣿⣿⡟⠀⣿⣿⣿⣿⠃⠀⠀⣾⣿⣿⡿⠿⠛⢛⣿⡟⠀⠀⠀⠀⠀⠻⠿⠀⠀\n");
            printf("⠀⠀⠀⠹⣿⣿⣶⣾⣿⣿⣿⠟⠁⠀⠸⢿⣿⠇⠀⠀⠀⠛⠛⠁⠀⠀⠀⠀⠀⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀\n");
            printf("⠀⠀⠀⠀⠈⠙⠛⠛⠛⠋⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n");
            printf("\n");
            printf("                              Puntaje final: Nivel %d\n", nivel);
            printf("=======================================================================\n\n");


            LPC_GPIO2->FIOSET = (1<<0) | (1<<1) | (1<<2) | (1<<3); //prendemos los led

            tiempo_referencia = ticks_ms;
            while (!tiempo_cumplido(tiempo_referencia, 2000))
            {
                // El micro espera aquí
            }

            estado_actual = ESTADO_IDLE;

            break;
        }


      case ESTADO_VICTORIA:
              {
                  printf("\n\n");
                  printf("⠀⠀⠀⠀⠀⠀⠀⠀⢀⣀⣀⣀⣀⣀⣀⣀⣀⣀⣀⣀⣀⡀⠀⠀⠀⠀⠀⠀⠀⠀\n");
                  printf("⠀⠀⣀⠀⢀⣶⣿⡛⠛⠋⠉⠉⠉⠉⠉⠉⠉⠉⠉⠉⠙⠛⢛⣿⣶⡄⠀⣀⠀⠀\n");
                  printf("⠀⠀⣿⣧⣼⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣧⣼⣿⠀⠀\n");
                  printf("⠀⠀⣿⡏⠉⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠉⢹⣿⠀⠀\n");
                  printf("⠀⠀⢻⣧⠀⢹⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡏⠀⣼⡏⠀⠀\n");
                  printf("⠀⠀⠘⣿⡄⠀⢿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡿⠀⢰⣿⠃⠀⠀\n");
                  printf("⠀⠀⠀⠹⣷⡀⠈⢿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡿⠁⢠⣿⡏⠀⠀⠀\n");
                  printf("⠀⠀⠀⠀⢻⣿⣄⢀⣻⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡀⣠⣿⡟⠀⠀⠀⠀\n");
                  printf("⠀⠀⠀⠀⠀⠙⣿⣾⡿⠋⠻⣿⣿⣿⣿⣿⣿⣿⣿⠟⠙⢿⣿⣿⠏⠀⠀⠀⠀⠀\n");
                  printf("⠀⠀⠀⠀⠀⠀⠈⠻⠟⠀⠀⠀⢹⣿⣿⣿⣿⡏⠀⠀⠀⠻⠟⠁⠀⠀⠀⠀⠀⠀\n");
                  printf("⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⢻⣿⣿⣿⣿⡟⠃⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n");
                  printf("⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⣿⣿⣿⣿⣇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n");
                  printf("⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣀⣴⣿⣿⣿⣿⣿⣿⣦⣀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n");
                  printf("⠀⠀⠀⠀⠀⠀⠀⣶⣾⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣶⠀⠀⠀⠀⠀⠀⠀\n");
                  printf("⠀⠀⠀⠀⠀⠀⠀⠀⠈⠉⠉⠉⠉⠛⠛⠛⠛⠉⠉⠉⠉⠁⠀⠀⠀⠀⠀⠀⠀⠀\n");
                  printf("\n");
                  printf("                      ¡VICTORIA PERFECTA!\n");
                  printf("                  Completaste los %d niveles.\n", MAX_SECUENCIA);
                  printf("=======================================================================\n\n");

                  	  //secuencia de led victoriosa
                  for (int repeticiones = 0; repeticiones < 3; repeticiones++)
                  {

                      for (int i = 0; i < CANT_LEDS; i++)
                      {
                          led_mostrar(i);
                          tiempo_referencia = ticks_ms;
                          while (!tiempo_cumplido(tiempo_referencia, 150));
                      }

                      for (int i = CANT_LEDS - 2; i > 0; i--)
                      {
                          led_mostrar(i);
                          tiempo_referencia = ticks_ms;
                          while (!tiempo_cumplido(tiempo_referencia, 150));
                      }
                  }


                  leds_apagar_todos();


                  estado_actual = ESTADO_IDLE;

                  break;
              }

      	  	  default:
                  {
                      estado_actual = ESTADO_IDLE;
                      break;
                  }
              }
          }

          return 0;
      }

/* =========================
   FUNCIONES AUXILIARES
   ========================= */

uint8_t tiempo_cumplido(uint32_t referencia, uint32_t demora_ms)
{
    return ((ticks_ms - referencia) >= demora_ms);
}

/* =========================
   SYSTICK
   ========================= */

void systick_init(void)
{
    /* Deshabilitar SysTick */
    SysTick->CTRL = 0;

    /* Cargar valor para 1 ms */
    SysTick->LOAD = (SystemCoreClock / 1000) - 1;

    /* Reiniciar contador actual */
    SysTick->VAL = 0;

    /* Habilitar SysTick:
       bit 0 = ENABLE
       bit 1 = TICKINT
       bit 2 = CLKSOURCE (clock del procesador) */
    SysTick->CTRL = (1 << 0) | (1 << 1) | (1 << 2);
}

void SysTick_Handler(void)
{
    ticks_ms++;
}

/* =========================
   INTERRUPCIÓN GPIO P0
   ========================= */

void EINT3_IRQHandler(void)
{
    uint32_t estado_p0;

    estado_p0 = LPC_GPIOINT->IO0IntStatF;

    LPC_GPIOINT->IO0IntClr = estado_p0;

    if (estado_actual == ESTADO_IDLE)
    {

        evento_start = 1; // apretamos una vez para prender


        return;
    }


    if (habilitar_lectura_usuario && (evento_boton == 0)) // chequeamos legalidad de la lectura
    {

        if (estado_p0 & BOTON_0)
        {
            boton_presionado = 0;
            evento_boton = 1;
        }
        else if (estado_p0 & BOTON_1)
        {
            boton_presionado = 1;
            evento_boton = 1;
        }
        else if (estado_p0 & BOTON_2)
        {
            boton_presionado = 2;
            evento_boton = 1;
        }
        else if (estado_p0 & BOTON_3)
        {
            boton_presionado = 3;
            evento_boton = 1;
        }
    }
}

/* =========================
   CONFIGURACIÓN BOTONES
   ========================= */

void config_botones(void)
{
    /* P0.0 a P0.3 como GPIO */
    LPC_PINCON->PINSEL0 &= ~0x000000FF;

    /* Pull-up internos */
    LPC_PINCON->PINMODE0 &= ~0x000000FF;

    /* Entradas */
    LPC_GPIO0->FIODIR &= ~MASK_BOTONES;

    /* Interrupción por flanco descendente */
    LPC_GPIOINT->IO0IntEnF |= MASK_BOTONES;
    LPC_GPIOINT->IO0IntEnR &= ~MASK_BOTONES;

    /* Limpiar pendientes */
    LPC_GPIOINT->IO0IntClr = MASK_BOTONES;

    /* Habilitar EINT3 */
    NVIC_EnableIRQ(EINT3_IRQn);
}

/* =========================
   MANEJO DE LEDS
   ========================= */

void leds_apagar_todos(void)
{
    LPC_GPIO2->FIOCLR = (
        (1 << 0) |
        (1 << 1) |
        (1 << 2) |
        (1 << 3)
    );
}

void led_mostrar(uint8_t indice)
{
    leds_apagar_todos();

    if (indice < CANT_LEDS)
    {
        LPC_GPIO2->FIOSET = led_mask[indice];
    }
}

void leds_init(void)
{
    /* P2.0, P2.1, P2.2, P2.3 como GPIO */
    LPC_PINCON->PINSEL4 &= ~(
        (3 << 0) |
        (3 << 2) |
        (3 << 4) |
        (3 << 6)
    );

    /* Salidas */
    LPC_GPIO2->FIODIR |= (
        (1 << 0) |
        (1 << 1) |
        (1 << 2) |
        (1 << 3)
    );

    leds_apagar_todos();
}

void led_encender(uint8_t indice)
{
    if (indice >= CANT_LEDS)
        return;

    LPC_GPIO2->FIOSET = led_mask[indice];
}

void led_apagar(uint8_t indice)
{
    if (indice >= CANT_LEDS)
        return;

    LPC_GPIO2->FIOCLR = led_mask[indice];
}

uint8_t boton_sigue_presionado(uint8_t boton)
{
    switch (boton)
    {
        case 0:
            return ((LPC_GPIO0->FIOPIN & BOTON_0) == 0);

        case 1:
            return ((LPC_GPIO0->FIOPIN & BOTON_1) == 0);

        case 2:
            return ((LPC_GPIO0->FIOPIN & BOTON_2) == 0);

        case 3:
            return ((LPC_GPIO0->FIOPIN & BOTON_3) == 0);

        default:
            return 0;
    }
}
