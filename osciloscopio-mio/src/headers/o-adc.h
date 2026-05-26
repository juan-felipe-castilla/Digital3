/*
 * o-adc.h
 *
 *  Created on: 25 may. 2026
 *      Author: benja
 */

#ifndef O_ADC_H_
#define O_ADC_H_

void ADC0_Init(void);
void filtrar_ruido_adc(void);
void send_adc_data_ascii(void);

#endif /* O_ADC_H_ */
