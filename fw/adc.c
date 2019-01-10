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

#include <stdbool.h>
#include <stdlib.h>

#define DETECTOR_CHANNEL a

volatile uint16_t adc_buf[ADC_BUFSIZE];
volatile struct adc_state adc_state = {0};
#define st adc_state
volatile struct adc_measurements adc_data;

static void adc_dma_init(int burstlen, bool enable_interrupt);
static void adc_timer_init(int psc, int ivl);


void adc_configure_scope_mode(uint8_t channel_mask, int sampling_interval_ns) {
	/* The constant SAMPLE_FAST (0) when passed in as sampling_interval_ns is handled specially in that we turn the ADC
	to continuous mode to get the highest possible sampling rate. */

	/* First, disable trigger timer, DMA and ADC in case we're reconfiguring on the fly. */
    TIM1->CR1 &= ~TIM_CR1_CEN;
    ADC1->CR &= ~ADC_CR_ADSTART;
    DMA1_Channel1->CCR &= ~DMA_CCR_EN;

	/* keep track of current mode in global variable */
	st.adc_mode = ADC_SCOPE;

	adc_dma_init(sizeof(adc_buf)/sizeof(adc_buf[0]), true);

    /* Clock from PCLK/4 instead of the internal exclusive high-speed RC oscillator. */
    ADC1->CFGR2 = (2<<ADC_CFGR2_CKMODE_Pos); /* Use PCLK/4=12MHz */
    /* Sampling time 13.5 ADC clock cycles -> total conversion time 2.17us*/
    ADC1->SMPR  = (2<<ADC_SMPR_SMP_Pos);

	/* Setup DMA and triggering */
	if (sampling_interval_ns == SAMPLE_FAST) /* Continuous trigger */
		ADC1->CFGR1 = ADC_CFGR1_DMAEN | ADC_CFGR1_DMACFG | ADC_CFGR1_CONT;
	else /* Trigger from timer 1 Channel 4 */
		ADC1->CFGR1 = ADC_CFGR1_DMAEN | ADC_CFGR1_DMACFG | (2<<ADC_CFGR1_EXTEN_Pos) | (1<<ADC_CFGR1_EXTSEL_Pos);
    ADC1->CHSELR = channel_mask;
	/* Perform self-calibration */
    ADC1->CR |= ADC_CR_ADCAL;
    while (ADC1->CR & ADC_CR_ADCAL)
        ;
	/* Enable conversion */
    ADC1->CR |= ADC_CR_ADEN;
    ADC1->CR |= ADC_CR_ADSTART;

	if (sampling_interval_ns == SAMPLE_FAST)
		return; /* We don't need the timer to trigger in continuous mode. */

	/* An ADC conversion takes 1.1667us, so to be sure we don't get data overruns we limit sampling to every 1.5us.
	Since we don't have a spare PLL to generate the ADC sample clock and re-configuring the system clock just for this
	would be overkill we round to 250ns increments. The minimum sampling rate is about 60Hz due to timer resolution. */
	int cycles = sampling_interval_ns > 1500 ? sampling_interval_ns/250 : 6;
	if (cycles > 0xffff)
		cycles = 0xffff;
	adc_timer_init(12/*250ns/tick*/, cycles);
}

void adc_configure_monitor_mode(int oversampling, int ivl_us, int mean_aggregate_len) {
	/* First, disable trigger timer, DMA and ADC in case we're reconfiguring on the fly. */
    TIM1->CR1 &= ~TIM_CR1_CEN;
    ADC1->CR &= ~ADC_CR_ADSTART;
    DMA1_Channel1->CCR &= ~DMA_CCR_EN;

	/* keep track of current mode in global variable */
	st.adc_mode = ADC_MONITOR;

	st.adc_oversampling = oversampling;
	st.ovs_count = 0;
	for (int i=0; i<NCH; i++)
		st.adc_aggregate[i] = 0;
	st.mean_aggregator[0] = st.mean_aggregator[1] = st.mean_aggregator[2] = 0; 
	st.mean_aggregate_ctr = 0;

    st.detector.hysteresis_mv = 6000;
    st.detector.debounce_cycles = 0;
    st.detector.base_interval_cycles = 10;

	st.detector.symbol = -1;
	st.detector.bit = 0;
	st.detector.committed_len_ctr = st.detector.len_ctr = 0;
	st.detector.debounce_ctr = 0;
	xfr_8b10b_reset((struct state_8b10b_dec *)&st.detector.rx8b10b);

	adc_dma_init(NCH, true);

	/* Setup DMA and triggering: Trigger from Timer 1 Channel 4 */
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

    adc_timer_init(SystemCoreClock/1000000/*1.0us/tick*/, ivl_us);
}

static void adc_dma_init(int burstlen, bool enable_interrupt) {
    /* Configure DMA 1 Channel 1 to get rid of all the data */
    DMA1_Channel1->CPAR = (unsigned int)&ADC1->DR;
    DMA1_Channel1->CMAR = (unsigned int)&adc_buf;
    DMA1_Channel1->CNDTR = burstlen;
    DMA1_Channel1->CCR = (0<<DMA_CCR_PL_Pos);
    DMA1_Channel1->CCR |=
          DMA_CCR_CIRC /* circular mode so we can leave it running indefinitely */
        | (1<<DMA_CCR_MSIZE_Pos) /* 16 bit */
        | (1<<DMA_CCR_PSIZE_Pos) /* 16 bit */
        | DMA_CCR_MINC
        | (enable_interrupt ? DMA_CCR_TCIE : 0); /* Enable transfer complete interrupt. */

	if (enable_interrupt) {
		/* triggered on transfer completion. We use this to process the ADC data */
		NVIC_EnableIRQ(DMA1_Channel1_IRQn);
		NVIC_SetPriority(DMA1_Channel1_IRQn, 3<<5);
	} else {
		NVIC_DisableIRQ(DMA1_Channel1_IRQn);
		DMA1->IFCR |= DMA_IFCR_CGIF1;
	}

    DMA1_Channel1->CCR |= DMA_CCR_EN; /* Enable channel */
}

static void adc_timer_init(int psc, int ivl) {
    TIM1->BDTR  = TIM_BDTR_MOE; /* MOE is needed even though we only "output" a chip-internal signal TODO: Verify this. */
    TIM1->CCMR2 = (6<<TIM_CCMR2_OC4M_Pos); /* PWM Mode 1 to get a clean trigger signal */
    TIM1->CCER  = TIM_CCER_CC4E; /* Enable capture/compare unit 4 connected to ADC */
    TIM1->CCR4  = 1; /* Trigger at start of timer cycle */
	/* Set prescaler and interval */
    TIM1->PSC   = psc-1;
    TIM1->ARR   = ivl-1;
    /* Preload all values */
    TIM1->EGR  |= TIM_EGR_UG;
    TIM1->CR1   = TIM_CR1_ARPE;
    /* And... go! */
    TIM1->CR1  |= TIM_CR1_CEN;
}

/* FIXME DEBUG */
static void gdb_dump(void) {
}

void DMA1_Channel1_IRQHandler(void) {
    int start = SysTick->VAL;
    static int debug_buf_pos = 0;

    /* Clear the interrupt flag */
    DMA1->IFCR |= DMA_IFCR_CGIF1;
    
    if (st.adc_mode == ADC_SCOPE)
        return;

    //for (int i=0; i<NCH; i++)
    //    st.adc_aggregate[i] += adc_buf[i];

    //if (++st.ovs_count == (1<<st.adc_oversampling)) {
        /* FIXME DEBUG
        for (int i=0; i<NCH; i++)
            st.adc_aggregate[i] >>= st.adc_oversampling;
        */
        /* This has been copied from the code examples to section 12.9 ADC>"Temperature sensor and internal reference
         * voltage" in the reference manual with the extension that we actually measure the supply voltage instead of
         * hardcoding it. This is not strictly necessary since we're running off a bored little LDO but it's free and
         * the current supply voltage is a nice health value.
         */
        // FIXME DEBUG adc_data.adc_vcc_mv = (3300 * VREFINT_CAL)/(st.adc_aggregate[VREF_CH]);

		int64_t vcc = 3300;
        /* FIXME debug
		int64_t vcc = adc_data.adc_vcc_mv;
		int64_t read = st.adc_aggregate[TEMP_CH] * 10 * 10000;
		int64_t cal = TS_CAL1 * 10 * 10000;
		adc_data.adc_temp_celsius_tenths = 300 + ((read/4096 * vcc) - (cal/4096 * 3300))/43000;
        */

		const long vmeas_r_total = VMEAS_R_HIGH + VMEAS_R_LOW;
        //int a = adc_data.adc_vmeas_a_mv = (st.adc_aggregate[VMEAS_A]*(vmeas_r_total * vcc / VMEAS_R_LOW)) >> 12;
        int a = adc_data.adc_vmeas_a_mv = (adc_buf[VMEAS_A]*13300) >> 12;
        //FIXME debug int b = adc_data.adc_vmeas_b_mv = (st.adc_aggregate[VMEAS_B]*vmeas_r_total)/4096 * vcc / VMEAS_R_LOW;

        /* FIXME debug
		st.mean_aggregator[0] += a;
		st.mean_aggregator[1] += b;
		st.mean_aggregator[2] += abs(b-a);
		if (++st.mean_aggregate_ctr == st.mean_aggregate_len) {
			adc_data.adc_mean_a_mv = st.mean_aggregator[0] / st.mean_aggregate_len;
			adc_data.adc_mean_b_mv = st.mean_aggregator[1] / st.mean_aggregate_len; 
			adc_data.adc_mean_diff_mv = st.mean_aggregator[2] / st.mean_aggregate_len;

			st.mean_aggregate_ctr = 0;
			st.mean_aggregator[0] = st.mean_aggregator[1] = st.mean_aggregator[2] = 0;
		}
        */

		//if (st.detector.debounce_ctr == 0) {
			int old_bit = st.detector.bit;
			int new_bit = old_bit;
            //FIXME debug int diff = (int)b - (int)a;
            int diff = a-5500;

            /*
            if (debug_buf_pos < NCH || debug_buf_pos >= sizeof(adc_buf)/sizeof(adc_buf[0])) {
                debug_buf_pos = NCH;
                gdb_dump();
            }
            adc_buf[debug_buf_pos++] = diff;
            */

			if (diff < - st.detector.hysteresis_mv/2)
				new_bit = 0;
			else if (diff > st.detector.hysteresis_mv/2)
				new_bit = 1;

            /*
            if (debug_buf_pos < NCH || debug_buf_pos >= sizeof(adc_buf)/sizeof(adc_buf[0])) {
                debug_buf_pos = NCH;
                gdb_dump();
            }
            adc_buf[debug_buf_pos++] = new_bit;
            */

			if (new_bit != old_bit) {
				st.detector.bit = new_bit;
				//st.detector.debounce_ctr = st.detector.debounce_cycles;
				st.detector.len_ctr = 0;
                st.detector.committed_len_ctr = st.detector.base_interval_cycles>>1;
			}
		//} else {
		//	st.detector.debounce_ctr--;
		//}

        if (debug_buf_pos < NCH || debug_buf_pos >= sizeof(adc_buf)/sizeof(adc_buf[0])) {
            debug_buf_pos = NCH;
            gdb_dump();
        }
        adc_buf[debug_buf_pos++] = st.detector.len_ctr;
        adc_buf[debug_buf_pos++] = st.detector.committed_len_ctr;
        adc_buf[debug_buf_pos++] = st.detector.bit;
        adc_buf[debug_buf_pos++] = diff;

		if (st.detector.len_ctr >= st.detector.committed_len_ctr) {
            /*
            if (debug_buf_pos < NCH || debug_buf_pos >= sizeof(adc_buf)/sizeof(adc_buf[0])) {
                debug_buf_pos = NCH;
                gdb_dump();
            }
            adc_buf[debug_buf_pos++] = st.detector.bit;
            */

			st.detector.committed_len_ctr += st.detector.base_interval_cycles;
			st.detector.symbol = xfr_8b10b_feed_bit((struct state_8b10b_dec *)&st.detector.rx8b10b, st.detector.bit);
            /*
            if (st.detector.symbol != -DECODING_IN_PROGRESS) {
                if (debug_buf_pos < NCH || debug_buf_pos >= sizeof(adc_buf)/sizeof(adc_buf[0])) {
                    debug_buf_pos = NCH;
                    gdb_dump();
                }
                adc_buf[debug_buf_pos++] = st.detector.symbol;
            }
            */
		}
		st.detector.len_ctr++;

        st.ovs_count = 0;
        for (int i=0; i<NCH; i++)
            st.adc_aggregate[i] = 0;
    //}
    int end = SysTick->VAL;
    /*
    if (debug_buf_pos < NCH || debug_buf_pos >= sizeof(adc_buf)/sizeof(adc_buf[0])) {
        debug_buf_pos = NCH;
        gdb_dump();
    }
    int tdiff = start - end;
    if (tdiff < 0)
        tdiff += SysTick->LOAD;
    adc_buf[debug_buf_pos++] = tdiff;
    */
}

