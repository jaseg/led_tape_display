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
static volatile uint16_t adc_buf[NCH];

void adc_init(void) {
    /* The ADC is used for temperature measurement. To compute the temperature from an ADC reading of the internal
     * temperature sensor, the supply voltage must also be measured. Thus we are using two channels.
     *
     * The ADC is triggered by compare channel 4 of timer 1. The trigger is set to falling edge to trigger on compare
     * match, not overflow.
     */
    ADC1->CFGR1 = ADC_CFGR1_DMAEN | ADC_CFGR1_DMACFG | (2<<ADC_CFGR1_EXTEN_Pos) | (1<<ADC_CFGR1_EXTSEL_Pos);
    /* Clock from PCLK/4 instead of the internal exclusive high-speed RC oscillator. */
    ADC1->CFGR2 = (2<<ADC_CFGR2_CKMODE_Pos); /* Use PCLK/4=12MHz */
    /* Sampling time 13.5 ADC clock cycles -> total conversion time 2.17us*/
    ADC1->SMPR  = (2<<ADC_SMPR_SMP_Pos);
    /* Internal VCC and temperature sensor channels */
    ADC1->CHSELR = ADC_CHSELR_CHSEL0 | ADC_CHSELR_CHSEL1 | ADC_CHSELR_CHSEL16 | ADC_CHSELR_CHSEL17;
    /* Enable internal voltage reference and temperature sensor */
    ADC->CCR = ADC_CCR_TSEN | ADC_CCR_VREFEN;
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
    DMA1_Channel1->CNDTR = NCH;
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

uint16_t buf_a[256];
uint16_t buf_b[256];
int bufp = 0;

void DMA1_Channel1_IRQHandler(void) {
    /* This interrupt takes either 1.2us or 13us. It can be pre-empted by the more timing-critical UART and LED timer
     * interrupts. */
    static int count = 0; /* oversampling accumulator sample count */
    static uint32_t adc_aggregate[NCH] = {0}; /* oversampling accumulator */

    /* Clear the interrupt flag */
    DMA1->IFCR |= DMA_IFCR_CGIF1;

    for (int i=0; i<NCH; i++)
        adc_aggregate[i] += adc_buf[i];

    if (++count == (1<<ADC_OVERSAMPLING)) {
        for (int i=0; i<NCH; i++)
            adc_aggregate[i] >>= ADC_OVERSAMPLING;
        /* This has been copied from the code examples to section 12.9 ADC>"Temperature sensor and internal reference
         * voltage" in the reference manual with the extension that we actually measure the supply voltage instead of
         * hardcoding it. This is not strictly necessary since we're running off a bored little LDO but it's free and
         * the current supply voltage is a nice health value.
         */
        adc_data.adc_vcc_mv = (3300 * VREFINT_CAL)/(adc_aggregate[VREF_CH]);

		int64_t read = adc_aggregate[TEMP_CH] * 10 * 10000;
		int64_t vcc = adc_data.adc_vcc_mv;
		int64_t cal = TS_CAL1 * 10 * 10000;
		adc_data.adc_temp_celsius_tenths = 300 + ((read/4096 * vcc) - (cal/4096 * 3300))/43000;

        adc_data.adc_vmeas_a_mv = (adc_aggregate[VMEAS_A]*13300L)/4096 * vcc / 3300;
        adc_data.adc_vmeas_b_mv = (adc_aggregate[VMEAS_B]*13300L)/4096 * vcc / 3300;

		buf_a[bufp] = adc_data.adc_vmeas_a_mv;
		buf_b[bufp] = adc_data.adc_vmeas_b_mv;
		if (++bufp >= sizeof(buf_a)/sizeof(buf_a[0])) {
			bufp = 0;
		}

        count = 0;
        for (int i=0; i<NCH; i++)
            adc_aggregate[i] = 0;
    }
}

