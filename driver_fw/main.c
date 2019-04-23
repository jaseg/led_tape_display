/* 8seg LED display driver firmware
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
#include "serial.h"
#include "i2c.h"
#include "lcd1602.h"
#include "mcp9801.h"
#include "ina226.h"

#include "mini-printf.h"

#include <8b10b.h>

/* Part number: STM32F030F4C6 */

volatile unsigned int comm_led_ctr, err_led_ctr;
volatile unsigned int sys_time_tick = 0;
volatile unsigned int sys_time_ms;
volatile unsigned int sys_time_s;
volatile unsigned int sys_flag_1Hz;
unsigned int frame_duration_us;
volatile uint8_t global_brightness; /* FIXME implement sending */

void trigger_error_led() {
    err_led_ctr = STATUS_LED_DURATION_MS/TICK_MS;
}

void trigger_comm_led() {
    comm_led_ctr = STATUS_LED_DURATION_MS/TICK_MS;
}

static volatile struct {
    int current_symbol;
    struct state_8b10b_enc st;
} txstate;

#define NO_SYMBOL (DECODER_RETURN_CODE_LAST + 1)

uint8_t random() {
    static uint8_t x, a, b, c;
    x++; //x is incremented every round and is not affected by any other variable
    a = (a ^ c ^ x); //note the mix of addition and XOR
    b = (b + a); //And the use of very few instructions
    c = ((c + ((b >> 1) ^ a))); // the AES S-Box Operation ensures an even distributon of entropy
    return c;
}

enum STATUS_LEDS {
    STATUS_LED_COMMUNICATION = 1,
    STATUS_LED_ERROR = 2,
    STATUS_LED_LOAD = 4,
    STATUS_LED_OPERATION = 8,
    STATUS_LED_J5_GREEN = 16,
    STATUS_LED_J5_YELLOW = 32,
    STATUS_LED_J4_GREEN = 64,
    STATUS_LED_J4_YELLOW = 128
};

static void set_status_leds(uint8_t val) {
    /* Reset strobe. Will be set in systick handler */
    GPIOA->BRR = 1<<4;
    //for (int i=0; i<100; i++)
    //    asm volatile ("nop");
    /* Workaround for *nasty* hardware behavior: If SPI data width is configured as 8 bit but DR is written as 16
     * bit, SPI actually sends 16 clock cycles. Thus, we have to make sure the compiler emits a 8-bit write here.
     * Thanks, TI! */
    *((volatile uint8_t *)&(SPI1->DR)) = val ^ 0x0f; /* Invert LEDs connected to VCC instead of GND */
}


int main(void) {
    /* Startup code */
    RCC->CR |= RCC_CR_HSEON;
    while (!(RCC->CR&RCC_CR_HSERDY));
    RCC->CFGR &= ~RCC_CFGR_PLLMUL_Msk & ~RCC_CFGR_SW_Msk & ~RCC_CFGR_PPRE_Msk & ~RCC_CFGR_HPRE_Msk;
    RCC->CFGR |= ((6-2)<<RCC_CFGR_PLLMUL_Pos) | RCC_CFGR_PLLSRC_HSE_PREDIV; /* PLL x6 -> 48.0MHz */
    RCC->CR |= RCC_CR_PLLON;
    while (!(RCC->CR&RCC_CR_PLLRDY));
    RCC->CFGR |= (2<<RCC_CFGR_SW_Pos);
    RCC->AHBENR  |= RCC_AHBENR_DMAEN | RCC_AHBENR_GPIOAEN | RCC_AHBENR_GPIOBEN | RCC_AHBENR_FLITFEN;
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN | RCC_APB1ENR_PWREN | RCC_APB1ENR_I2C1EN;
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN | RCC_APB2ENR_ADCEN| RCC_APB2ENR_DBGMCUEN | RCC_APB2ENR_USART1EN | RCC_APB2ENR_SPI1EN;

    SystemCoreClockUpdate();
    SysTick_Config(SystemCoreClock/(1000/TICK_MS)); /* 10ms interval */
    NVIC_EnableIRQ(SysTick_IRQn);
    NVIC_SetPriority(SysTick_IRQn, 3<<5);

    /* GPIO setup */
    GPIOA->MODER |=
          (3<<GPIO_MODER_MODER0_Pos)  /* PA0  - Vboot to ADC */
        | (2<<GPIO_MODER_MODER1_Pos)  /* PA1  - RS485 DE */
        | (2<<GPIO_MODER_MODER2_Pos)  /* PA2  - RS485 TX */
        | (2<<GPIO_MODER_MODER3_Pos)  /* PA3  - RS485 RX */
        | (1<<GPIO_MODER_MODER4_Pos)  /* PA4  - Strobe/Vin to ADC. CAUTION: This pin is dual-use  */
        | (2<<GPIO_MODER_MODER5_Pos)  /* PA5  - SCK */
        | (2<<GPIO_MODER_MODER6_Pos)  /* PA6  - CTRL_A  to TIM 3 ch 1 */
        | (2<<GPIO_MODER_MODER7_Pos)  /* PA7  - MOSI */
        | (2<<GPIO_MODER_MODER9_Pos)  /* PA9  - SCL */
        | (2<<GPIO_MODER_MODER10_Pos);/* PA10 - SDA */

    GPIOA->AFR[0] =
          (1<<GPIO_AFRL_AFSEL1_Pos)  /* PA1 */
        | (1<<GPIO_AFRL_AFSEL2_Pos)  /* PA2 */
        | (1<<GPIO_AFRL_AFSEL3_Pos)  /* PA3 */
        | (1<<GPIO_AFRL_AFSEL6_Pos); /* PA6 */
    GPIOA->AFR[1] =
          (4<<GPIO_AFRH_AFSEL9_Pos)  /* PA9 */
        | (4<<GPIO_AFRH_AFSEL10_Pos);/* PA10 */

    GPIOA->ODR = 0; /* Set PA4 ODR to 0 */

    GPIOA->OTYPER |=
          GPIO_OTYPER_OT_1
        | GPIO_OTYPER_OT_2;

    // FIXME lag 37.3us @ 720 Ohm / 16.0us @ 360 Ohm / 2.8us @ 88 Ohm
    GPIOA->OSPEEDR |=
          (3<<GPIO_OSPEEDR_OSPEEDR1_Pos)
        | (3<<GPIO_OSPEEDR_OSPEEDR2_Pos);

    /* Note: since we have quite a bunch of pin constraints we can't actually use complementary outputs for the
     * complementary MOSFET driver control signals (CTRL_A & CTRL_B). Instead, we use two totally separate output
     * channels (1 & 4) and emulate the dead-time generator in software. */
    GPIOB->MODER |=
          (2<<GPIO_MODER_MODER1_Pos); /* PB1  - CTRL_B to TIM 3 ch 4 */

    GPIOB->AFR[0] = (1<<GPIO_AFRL_AFSEL1_Pos); /* PB1 */

    serial_init();
    /* FIXME ADC config */

    /* SPI config. SPI1 is used to control the shift register controlling the eight status LEDs. */
    SPI1->CR2 = (7<<SPI_CR2_DS_Pos);

    /* Baud rate PCLK/128 -> 375.0kHz */
    SPI1->CR1 =
          SPI_CR1_SSM
        | SPI_CR1_SSI
        | (6<<SPI_CR1_BR_Pos)
        | SPI_CR1_MSTR;
    SPI1->CR1 |= SPI_CR1_SPE;

    /* I2C for LCD, temp sensor, current sensor */
    i2c_config_filters(I2C1, I2C_AF_ENABLE, 0);
    i2c_config_timing(I2C1, 0x2000090e); /* Magic value for 100kHz I2C @ 48MHz CLK. Fell out of STMCubeMX. I love
                                           downloading 120MB of software to download another 100MB of software, only
                                           this time over unsecured HTTP, to generate 3.5 bytes of configuration values
                                           using a Java(TM) GUI. */
    i2c_enable(I2C1);
    lcd1602_init();
    ina226_init();
    mcp9801_init();
    /* The MCP9801 temperature sensor is initialized below in the SysTick ISR since it needs a few milliseconds to
     * powerup. */

    /* TIM3 is used to generate the MOSFET driver control signals */
    /* TIM3 running off 48MHz APB1 clk, T=20.833ns */
    TIM3->CR1 = 0; /* Disable ARR preload (double-buffering) */
    TIM3->PSC = 48-1; /* Prescaler 48 -> f=1MHz/T=1us */
    TIM3->DIER = TIM_DIER_UIE; /* Enable update (overflow) interrupt */
    TIM3->CCR1 = 0xffff;
    TIM3->CCR4 = 0xffff;
    TIM3->CCMR1 = 6<<TIM_CCMR1_OC1M_Pos | TIM_CCMR1_OC1PE; /* Configure output compare unit 1 to PWM mode 1, enable CCR1 preload */
    TIM3->CCMR2 = 6<<TIM_CCMR2_OC4M_Pos | TIM_CCMR2_OC4PE; /* Configure output compare unit 4 to PWM mode 1, enable CCR4 preload */
    TIM3->CCER = TIM_CCER_CC1E | TIM_CCER_CC1P | TIM_CCER_CC4E | TIM_CCER_CC4P; /* Confiugre CH1 to complementary outputs */
    TIM3->BDTR = TIM_BDTR_MOE; /* Enable MOE on next update event, i.e. on initial timer load. */
    TIM3->CR1 |= TIM_CR1_CEN;
    TIM3->ARR = 800-1; /* Set f=2.5kHz/T=0.4ms */

    xfr_8b10b_encode_reset(&txstate.st);
    txstate.current_symbol = xfr_8b10b_encode(&txstate.st, K28_1) | 1<<10;
    TIM3->EGR |= TIM_EGR_UG;

    lcd_write_str(0, 0, "8seg driver");
    lcd_write_str(0, 1, "initialized \xbc");

    NVIC_EnableIRQ(TIM3_IRQn);
    NVIC_SetPriority(TIM3_IRQn, 2<<4);

    while (42) {
        if (sys_flag_1Hz) {
            sys_flag_1Hz = 0; 

            char buf[17];

            int temp = mcp9801_read_mdegC();
            int deg = temp/1000;
            int frac = (temp%1000)/100;
            mini_snprintf(buf, sizeof(buf), "Temp: %d.%01d\xdf""C" LCD_FILL, deg, frac);
            lcd_write_str(0, 0, buf);

            mini_snprintf(buf, sizeof(buf), "I=%dmA U=%dmV" LCD_FILL, ina226_read_i()*INA226_I_LSB_uA/1000, ina226_read_v()*INA226_VB_LSB_uV/1000);
            lcd_write_str(0, 1, buf);

        }
    }
}

static int flipbits10(int in) {
    return
        (in&0x200)>>9 |
        (in&0x100)>>7 |
        (in&0x080)>>5 |
        (in&0x040)>>3 |
        (in&0x020)>>1 |
        (in&0x010)<<1 |
        (in&0x008)<<3 |
        (in&0x004)<<5 |
        (in&0x002)<<7 |
        (in&0x001)<<9;

}

void TIM3_IRQHandler() {
    static int txpos = -1;
    static unsigned int tx_start_tick = 0;
    static uint8_t txbuf[3] = {0x01, 0x05, 0x01};

    TIM3->SR &= ~TIM_SR_UIF;
    int sym = txstate.current_symbol;
    int bit = sym&1;
    sym >>= 1;
    if (sym == 1) { /* last bit shifted out */
        if (txpos == -1)
            sym = xfr_8b10b_encode(&txstate.st, -K28_1);
        else
            sym = xfr_8b10b_encode(&txstate.st, txbuf[txpos]);

        txpos++;
        if (txpos >= sizeof(txbuf)/sizeof(txbuf[0])) {
            frame_duration_us = (sys_time_tick - tx_start_tick) * 10 * 1000;
            tx_start_tick = sys_time_tick;
            txpos = -1;
        }

        sym = flipbits10(sym) | 1<<10;
    }
    txstate.current_symbol = sym;

#define DEAD_TIME 100
    TIM3->CCR1 = bit ? 0xffff : DEAD_TIME;
    TIM3->CCR4 = bit ? DEAD_TIME : 0xffff;
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
    sys_time_tick++;
    sys_time_ms += TICK_MS;
    if (sys_time_ms++ == 1000) {
        sys_time_ms = 0;
        sys_time_s++;
        sys_flag_1Hz = 1;
    }

    /* This is a hack. We could use the SPI interrupt here if that didn't fire at the start instead of end of transmission.... -.- */
    if (sys_time_tick&1) {
        uint8_t val = (sys_time_ms >= 500) ? STATUS_LED_OPERATION : 0;

        if (comm_led_ctr) {
            comm_led_ctr--;
            val |= STATUS_LED_COMMUNICATION;
        }

        if (err_led_ctr) {
            err_led_ctr--;
            val |= STATUS_LED_ERROR;
        }

        set_status_leds(val);
    } else {
        GPIOA->BSRR = 1<<4;
    }
}

void _init(void) {
}

void BusFault_Handler(void) __attribute__((naked));
void BusFault_Handler() {
    asm volatile ("bkpt");
}
