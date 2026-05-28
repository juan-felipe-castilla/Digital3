/*
 * o-adc.h
 *
 *  Created on: 25 may. 2026
 *      Author: benja
 */

#ifndef O_ADC_H_
#define O_ADC_H_

void ADC0_Init(void);
void filtrar_ruido_adc(uint32_t *buffer_crudo);
uint8_t obtener_mediana_5(uint8_t *ventana);

#endif /* O_ADC_H_ */
