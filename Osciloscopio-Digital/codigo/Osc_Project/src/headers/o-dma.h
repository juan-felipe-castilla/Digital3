/*
 * o-dma.h
 *
 *  Created on: 25 may. 2026
 *      Author: benja
 */

#ifndef O_DMA_H_
#define O_DMA_H_

void capture_adc_dma_start(void);
void DMA_IRQHandler(void);
void dac_dma(void);

#endif /* O_DMA_H_ */
