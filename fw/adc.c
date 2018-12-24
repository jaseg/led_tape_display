/* Megumin LED display firmware
 * Copyright (C) 2018 Sebastian Götte <code@jaseg.net>
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "adc.h"

volatile struct adc_measurements adc_data = {0};

enum adc_channels {
    VREF_CH,
    VMEAS_A,
    VMEAS_B,
    TEMP_CH,
    NCH
};
static volatile uint16_t adc_buf[1024];

void adc_init(void) {
    /* The ADC is used for temperature measurement. To compute the temperature from an ADC reading of the internal
     * temperature sensor, the supply voltage must also be measured. Thus we are using two channels.
     *
     * The ADC is triggered by compare channel 4 of timer 1. The trigger is set to falling edge to trigger on compare
     * match, not overflow.
     */
    ADC1->CFGR1 = ADC_CFGR1_DMAEN | ADC_CFGR1_DMACFG | (2<<ADC_CFGR1_EXTEN_Pos) | (1<<ADC_CFGR1_EXTSEL_Pos) | ADC_CFGR1_CONT;
    /* Clock from PCLK/4 instead of the internal exclusive high-speed RC oscillator. */
    ADC1->CFGR2 = (2<<ADC_CFGR2_CKMODE_Pos); /* Use PCLK/4=12MHz */
    /* Sampling time 13.5 ADC clock cycles -> total conversion time 2.17us*/
    ADC1->SMPR  = (2<<ADC_SMPR_SMP_Pos);

    ADC1->CHSELR = ADC_CHSELR_CHSEL0 | ADC_CHSELR_CHSEL1;
    /* Perform ADC calibration */
    ADC1->CR |= ADC_CR_ADCAL;
    while (ADC1->CR & ADC_CR_ADCAL)
        ;
    /* Enable ADC */
    ADC1->CR |= ADC_CR_ADEN;
    ADC1->CR |= ADC_CR_ADSTART;

    /* Configure DMA 1 Channel 1 to get rid of all the data */
    DMA1_Channel1->CPAR = (unsigned int)&ADC1->DR;
    DMA1_Channel1->CMAR = (unsigned int)&adc_buf;
    DMA1_Channel1->CNDTR = sizeof(adc_buf)/sizeof(adc_buf[0]);
    DMA1_Channel1->CCR = (0<<DMA_CCR_PL_Pos);
    DMA1_Channel1->CCR |=
          DMA_CCR_CIRC /* circular mode so we can leave it running indefinitely */
        | (1<<DMA_CCR_MSIZE_Pos) /* 16 bit */
        | (1<<DMA_CCR_PSIZE_Pos) /* 16 bit */
        | DMA_CCR_MINC
        | DMA_CCR_TCIE; /* Enable transfer complete interrupt. */
    DMA1_Channel1->CCR |= DMA_CCR_EN; /* Enable channel */

    /* triggered on transfer completion. We use this to process the ADC data */
    NVIC_EnableIRQ(DMA1_Channel1_IRQn);
    NVIC_SetPriority(DMA1_Channel1_IRQn, 3<<5);
}

void DMA1_Channel1_IRQHandler(void) {
    /* Clear the interrupt flag */
    DMA1->IFCR |= DMA_IFCR_CGIF1;

}

