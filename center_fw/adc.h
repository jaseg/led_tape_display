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

#ifndef __ADC_H__
#define __ADC_H__

#include "global.h"
#include "8b10b.h"
#include "protocol.h"

struct adc_measurements {
    int16_t vcc_mv;
    int16_t temp_celsius_tenths;
    int16_t vmeas_a_mv;
    int16_t vmeas_b_mv;
    int16_t mean_a_mv;
    int16_t mean_b_mv;
    int16_t mean_diff_mv;
};

enum channel_mask {
	MASK_VMEAS_A = ADC_CHSELR_CHSEL0,
	MASK_VMEAS_B = ADC_CHSELR_CHSEL1
};

enum adc_mode {
	ADC_UNINITIALIZED,
	ADC_MONITOR,
	ADC_SCOPE
};

enum sampling_mode {
	SAMPLE_FAST = 0
};

/* The weird order is to match the channels' order in the DMA buffer. Due to some configuration mistake I can't be
bothered to fix, the DMA controller outputs ADC measurements off-by-one into the output buffer. */
enum adc_channels {
    VREF_CH,
    VMEAS_A,
    VMEAS_B,
    TEMP_CH,
    NCH
};

struct bit_detector_st {
    int hysteresis_mv;
    int sync;
    int base_interval_cycles;
    struct proto_rx_st rx_st;
    /* private stuff */
    int last_bit;
    int len_ctr;
    int committed_len_ctr;
    struct state_8b10b_dec rx8b10b;
};

struct adc_state {
	enum adc_mode adc_mode;
    int dma_isr_duration;
    struct bit_detector_st det_st;
	/* private stuff */
	uint32_t adc_aggregate[NCH]; /* oversampling accumulator */
	uint32_t mean_aggregate_ctr;
	uint32_t mean_aggregator[3];
};

extern volatile struct adc_state adc_state;
extern volatile uint16_t adc_buf[ADC_BUFSIZE];
extern volatile struct adc_measurements adc_data;

void adc_init(void);
void adc_configure_scope_mode(uint8_t channel_mask, int sampling_interval_ns);
void adc_configure_monitor_mode(const struct command_if_def *cmd_if);

void bit_detector(struct bit_detector_st *st, int a);
void receive_bit(struct bit_detector_st *st, int bit);

void blank(void);
void unblank(int new_bit);

#endif/*__ADC_H__*/
