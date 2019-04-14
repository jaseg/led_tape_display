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

#ifndef __GLOBAL_H__
#define __GLOBAL_H__

/* Workaround for sub-par ST libraries */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstrict-aliasing"
#include <stm32f0xx.h>
#include <stm32f0xx_ll_utils.h>
#include <stm32f0xx_ll_spi.h>
#pragma GCC diagnostic pop

#include <system_stm32f0xx.h>

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

/* Microcontroller part number: STM32F030F4P6 */

/* Things used for module status reporting. */
#define FIRMWARE_VERSION 1
#define HARDWARE_VERSION 3

#define TS_CAL1 (*(uint16_t *)0x1FFFF7B8)
#define VREFINT_CAL (*(uint16_t *)0x1FFFF7BA)

#define STATUS_LED_DURATION_MS 200
#define TICK_MS 10

extern volatile unsigned int sys_time_tick;
extern volatile unsigned int sys_time_ms;
extern volatile unsigned int sys_time_s;
extern unsigned int frame_duration_us;

extern volatile uint8_t global_brightness;

void trigger_error_led(void);
void trigger_comm_led(void);

#endif/*__GLOBAL_H__*/
