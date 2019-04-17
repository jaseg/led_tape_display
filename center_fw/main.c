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
#include "8seg_protocol.h"
#include "transmit.h"

volatile unsigned int sys_time = 0;
volatile unsigned int sys_time_seconds = 0;
uint16_t jitter_meas_avg_ns = 0;

void TIM1_BRK_UP_TRG_COM_Handler() {
    TIM1->SR &= ~TIM_SR_UIF_Msk;
}

void set_drv_gpios(uint8_t val) {
    int a=!!(val&1), b=!!(val&2), c=!!(val&4), d=!!(val&8);
    GPIOA->BSRR = ((!a<<3 | !b<<7 | c<<6 | d<<4)<<16) | (a<<3 | b<<7 | !c<<6 | !d<<4);
}

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

void set_load(bool load) {
    GPIOA->BSRR = (1<<2) << (load ? 0 : 16);
}

void blank(void) {
    set_drv_gpios(0);
}

volatile int bit; /* FIXME */
void unblank_low(void) {
    if (backchannel_frame) { /* Set from protocol.c */
        if (tx_next_bit() == 1)
            set_load(1);
        else /* 0; but also TX_IDLE */
            set_load(0);

    } else {
        if (bit)
            set_drv_gpios(out_state & 0xf);
        else
            set_drv_gpios(out_state >> 4);
    }
}

void TIM3_IRQHandler(void) {
    GPIOA->BSRR = 1<<10; /* debug */
    if (TIM3->SR & TIM_SR_UIF)
        unblank_low();
    else
        blank();

    TIM3->SR = 0;
    GPIOA->BRR = 1<<10; /* debug */
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
    //SysTick_Config(SystemCoreClock/1000); /* 1ms interval */

    /* Turn on lots of neat things */
    RCC->AHBENR  |= RCC_AHBENR_DMAEN | RCC_AHBENR_GPIOAEN | RCC_AHBENR_FLITFEN;
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN | RCC_APB2ENR_ADCEN| RCC_APB2ENR_DBGMCUEN | RCC_APB2ENR_TIM1EN | RCC_APB2ENR_TIM1EN;;
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;

    /* TIM3 foo */
    TIM3->CCMR2 = (6<<TIM_CCMR2_OC4M_Pos); /* PWM Mode 1 to get a clean trigger signal */
    TIM3->CCER  = TIM_CCER_CC4E; /* Enable capture/compare unit 4 connected to ADC */

    TIM3->PSC   =  48-1; /* 48MHz -> 1MHz */
    TIM3->CCR4  = 170-1; /* CC4 is ADC trigger, fire 30us before end of cycle. */
    TIM3->ARR   = 200-1; /* 1MHz -> 5kHz */

    TIM3->DIER |= TIM_DIER_UIE | TIM_DIER_CC4IE;

    TIM3->CR1  |= TIM_CR1_CEN;
    NVIC_EnableIRQ(TIM3_IRQn);
    NVIC_SetPriority(TIM3_IRQn, 3<<5);

    GPIOA->MODER |=
          (0<<GPIO_MODER_MODER0_Pos)  /* PA0  - Vmeas_A to ADC */
        | (0<<GPIO_MODER_MODER1_Pos)  /* PA1  - Vmeas_B to ADC */
        | (1<<GPIO_MODER_MODER2_Pos)  /* PA2  - LOAD */
        | (1<<GPIO_MODER_MODER3_Pos)  /* PA3  - CH0 */
        | (1<<GPIO_MODER_MODER4_Pos)  /* PA4  - CH3 */
        | (1<<GPIO_MODER_MODER5_Pos)  /* PA5  - TP1 */
        | (1<<GPIO_MODER_MODER6_Pos)  /* PA6  - CH2 */
        | (1<<GPIO_MODER_MODER7_Pos)  /* PA7  - CH1 */
        | (1<<GPIO_MODER_MODER9_Pos)  /* PA9  - TP2 */
        | (1<<GPIO_MODER_MODER10_Pos);/* PA10 - TP3 */

    /* Set shift register IO GPIO output speed */
    GPIOA->OSPEEDR |=
          (2<<GPIO_OSPEEDR_OSPEEDR2_Pos)   /* LOAD */
        | (2<<GPIO_OSPEEDR_OSPEEDR3_Pos)   /* CH0 */
        | (2<<GPIO_OSPEEDR_OSPEEDR4_Pos)   /* CH3   */
        | (2<<GPIO_OSPEEDR_OSPEEDR6_Pos)   /* CH2 */
        | (2<<GPIO_OSPEEDR_OSPEEDR7_Pos);  /* CH1 */

    set_drv_gpios(0);

    protocol_init();

    uint32_t jitter_meas_sum = 0, jitter_meas_cnt = 0;
    while (42) {
        int new = GPIOA->IDR & (1<<0); /* Sample current polarity */
        if (new != bit) { /* Zero-crossing detected */
            bit = new;

            /* Store old counter value for jitter measurement. Let it overflow to handle negative offsets. */
            int16_t cnt = (int16_t)TIM3->CNT;
            /* Re-initialize the counter to align it with the signal edge */
            TIM3->EGR  |= TIM_EGR_UG;
            /* Unblank since the update interrupt will not fire this time */
            unblank_low();

            /* Don't handle overflow of _sum here since this value is only for monitoring anyway */
            jitter_meas_sum += (cnt >= 0) ? cnt : -cnt;
            if (++jitter_meas_cnt == 4000) { /* One measurement roughly every 800ms */
                /* Divide aggregate over 4000 us-resolution measurements by 4 -> ns-resolution average */
                uint32_t divided = jitter_meas_sum>>2;
                jitter_meas_avg_ns = (divided < UINT16_MAX) ? divided : UINT16_MAX;
            }
        }
        /* idle */
    }
}

void NMI_Handler(void) {
    asm volatile ("bkpt");
}

void HardFault_Handler(void) __attribute__((naked));
void HardFault_Handler() {
    asm volatile ("bkpt");
}

void SVC_Handler(void) {
    asm volatile ("bkpt");
}


void PendSV_Handler(void) {
    asm volatile ("bkpt");
}

void SysTick_Handler(void) {
    static int n = 0;
    sys_time++;
    if (n++ == 1000) {
        n = 0;
        sys_time_seconds++;
    }
}

