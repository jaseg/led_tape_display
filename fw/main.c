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

#include "global.h"

#include "adc.h"

volatile unsigned int sys_time = 0;
volatile unsigned int sys_time_seconds = 0;

void TIM1_BRK_UP_TRG_COM_Handler() {
    TIM1->SR &= ~TIM_SR_UIF_Msk;
}

enum packet_type {
    PKT_TYPE_RESERVED = 0,
    PKT_TYPE_SET_OUTPUTS_BINARY = 1,
    PKT_TYPE_SET_GLOBAL_BRIGHTNESS = 2,
    PKT_TYPE_SET_OUTPUTS = 3,
    PKT_TYPE_MAX
};

struct {
    struct command_if_def cmd_if;
    int payload_len[PKT_TYPE_MAX];
} cmd_if = {{PKT_TYPE_MAX}, {
    [PKT_TYPE_RESERVED] = 0,
    [PKT_TYPE_SET_OUTPUTS_BINARY] = 1,
    [PKT_TYPE_SET_GLOBAL_BRIGHTNESS] = 1,
    [PKT_TYPE_SET_OUTPUTS] = 8 }
};

uint8_t out_state = 0x01;
void set_outputs(uint8_t val[8]) {
    /* TODO implement BCM for digital brightness control */
    int x = 0;
    for (int i=0; i<8; i++)
        if (val[i] > 127)
            x |= 1<<i;
    out_state = x;
}

void set_outputs_binary(int mask, int global_brightness) {
    uint8_t val[8];
    for (int i=0; i<8; i++)
        val[i] = (mask & (1<<i)) ? global_brightness : 0;
    set_outputs(val);
}

void handle_command(int command, uint8_t *args) {
    static int global_brightness = 0xff;
    switch (command) {
        case PKT_TYPE_SET_OUTPUTS_BINARY:
            set_outputs_binary(args[0], global_brightness);
            break;

        case PKT_TYPE_SET_GLOBAL_BRIGHTNESS:
            global_brightness = args[0];
            break;

        case PKT_TYPE_SET_OUTPUTS:
            set_outputs(args);
            break;
    }
}

int main(void) {
    RCC->CR |= RCC_CR_HSEON;
    while (!(RCC->CR&RCC_CR_HSERDY));
    RCC->CFGR &= ~RCC_CFGR_PLLMUL_Msk & ~RCC_CFGR_SW_Msk & ~RCC_CFGR_PPRE_Msk & ~RCC_CFGR_HPRE_Msk;
    RCC->CFGR |= ((6-2)<<RCC_CFGR_PLLMUL_Pos) | RCC_CFGR_PLLSRC_HSE_PREDIV; /* PLL x6 -> 48.0MHz */
    RCC->CR |= RCC_CR_PLLON;
    while (!(RCC->CR&RCC_CR_PLLRDY));
    RCC->CFGR |= (2<<RCC_CFGR_SW_Pos);
    SystemCoreClockUpdate();
    SysTick_Config(SystemCoreClock/1000); /* 1ms interval */

    /* Turn on lots of neat things */
    RCC->AHBENR  |= RCC_AHBENR_DMAEN | RCC_AHBENR_GPIOAEN | RCC_AHBENR_FLITFEN;
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN | RCC_APB2ENR_ADCEN| RCC_APB2ENR_DBGMCUEN | RCC_APB2ENR_TIM1EN | RCC_APB2ENR_TIM1EN;;
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;

    GPIOA->MODER |=
          (0<<GPIO_MODER_MODER0_Pos)  /* PA0  - Vmeas_A to ADC */
        | (0<<GPIO_MODER_MODER1_Pos)  /* PA1  - Vmeas_B to ADC */
        | (1<<GPIO_MODER_MODER2_Pos)  /* PA2  - LOAD */
        | (1<<GPIO_MODER_MODER3_Pos)  /* PA3  - CH0 */
        | (1<<GPIO_MODER_MODER4_Pos)  /* PA4  - CH3 */
        | (0<<GPIO_MODER_MODER5_Pos)  /* PA5  - TP1 */
        | (1<<GPIO_MODER_MODER6_Pos)  /* PA6  - CH2 */
        | (1<<GPIO_MODER_MODER7_Pos)  /* PA7  - CH1 */
        | (0<<GPIO_MODER_MODER9_Pos)  /* PA9  - TP2 */
        | (0<<GPIO_MODER_MODER10_Pos);/* PA10 - TP3 */

    /* Set shift register IO GPIO output speed */
    GPIOA->OSPEEDR |=
          (2<<GPIO_OSPEEDR_OSPEEDR2_Pos)   /* LOAD */
        | (2<<GPIO_OSPEEDR_OSPEEDR3_Pos)   /* CH0 */
        | (2<<GPIO_OSPEEDR_OSPEEDR4_Pos)   /* CH3   */
        | (2<<GPIO_OSPEEDR_OSPEEDR6_Pos)   /* CH2 */
        | (2<<GPIO_OSPEEDR_OSPEEDR7_Pos);  /* CH1 */

    void set_drv_gpios(uint8_t val) {
        int a=!!(val&1), b=!!(val&2), c=!!(val&4), d=!!(val&8);
        GPIOA->ODR &= ~(!a<<3 | !b<<7 | c<<6 | d<<4);
        GPIOA->ODR |= a<<3 | b<<7 | !c<<6 | !d<<4;
    }
    set_drv_gpios(0);

	adc_configure_monitor_mode(&cmd_if.cmd_if, 50 /*us*/);

    int debounce_ctr = 0;
    int val_last = 0;
    int reset_ctr = 0;
    while (42) {
        if (reset_ctr)
            reset_ctr--;
        else
            set_drv_gpios(0);

        int val = adc_state.det_st.last_bit;
        if (val != val_last) {
            if (val)
                set_drv_gpios(out_state & 0xf);
            else
                set_drv_gpios(out_state >> 4);
            reset_ctr = 500;
            val_last = val;
        }
    }
}

void NMI_Handler(void) {
}

void HardFault_Handler(void) __attribute__((naked));
void HardFault_Handler() {
    asm volatile ("bkpt");
}

void SVC_Handler(void) {
}


void PendSV_Handler(void) {
}

void SysTick_Handler(void) {
    static int n = 0;
    sys_time++;
    if (n++ == 1000) {
        n = 0;
        sys_time_seconds++;
    }
}

