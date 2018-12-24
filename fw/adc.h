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

struct adc_measurements {
    int16_t adc_vcc_mv;
    int16_t adc_temp_celsius_tenths;
    int16_t adc_vmeas_a_mv;
    int16_t adc_vmeas_b_mv;
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


extern volatile struct adc_measurements adc_data;
extern volatile uint16_t adc_buf[ADC_BUFSIZE];
extern enum adc_mode adc_mode;
extern int adc_oversampling;

void adc_init(void);
void adc_configure_scope_mode(uint8_t channel_mask, int sampling_interval_ns);
void adc_configure_monitor_mode(int oversampling);

#endif/*__ADC_H__*/
