

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstrict-aliasing"
#include <stm32f0xx.h>
#pragma GCC diagnostic pop

#include <system_stm32f0xx.h>
#include <stm32f0xx_ll_spi.h>

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

#include <8b10b.h>

/* Part number: STM32F030F4C6 */

static volatile unsigned int sys_time;

uint32_t get_tick() {
    return SysTick->VAL;
}

static volatile struct {
    int current_symbol, next_symbol;
    struct state_8b10b_enc st;
} txstate;

#define NO_SYMBOL (DECODER_RETURN_CODE_LAST + 1)

unsigned char random() {
    static unsigned char x, a, b, c;
    x++; //x is incremented every round and is not affected by any other variable
    a = (a ^ c ^ x); //note the mix of addition and XOR
    b = (b + a); //And the use of very few instructions
    c = ((c + ((b >> 1) ^ a))); // the AES S-Box Operation ensures an even distributon of entropy
    return c;
}

enum STATUS_LEDS {
    STATUS_LED_COMM = 1,
    STATUS_LED_ERROR = 2,
    STATUS_LED_LOAD = 4,
    STATUS_LED_OPERATION = 8,
    STATUS_LED_J5_GREEN = 16,
    STATUS_LED_J5_YELLOW = 32,
    STATUS_LED_J4_GREEN = 64,
    STATUS_LED_J4_YELLOW = 128
};

static void set_status_leds(uint8_t val) {
    /* Workaround for *nasty* hardware behavior: If SPI data width is configured as 8 bit but DR is written as 16
     * bit, SPI actually sends 16 clock cycles. Thus, we have to make sure the compiler emits a 8-bit write here.
     * Thanks, TI! */
    *((volatile uint8_t *)&(SPI1->DR)) = val ^ 0x0f; /* Invert LEDs connected to VCC instead of GND */
}


int main(void) {
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
    SysTick_Config(SystemCoreClock/100); /* 10ms interval */
    NVIC_EnableIRQ(SysTick_IRQn);
    NVIC_SetPriority(SysTick_IRQn, 3<<5);

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

    GPIOB->MODER |=
          (2<<GPIO_MODER_MODER1_Pos); /* PB1  - CTRL_B to TIM 3 ch 4 */

    GPIOB->AFR[0] = (1<<GPIO_AFRL_AFSEL1_Pos); /* PB1 */

    /* FIXME USART config */
    /* FIXME ADC config */
    /* FIXME I2C config */

    /* SPI config. SPI1 is used to control the shift register controlling the eight status LEDs. */
    SPI1->CR2 = (7<<SPI_CR2_DS_Pos);

    /* Baud rate PCLK/128 -> 375.0kHz */
    SPI1->CR1 =
          SPI_CR1_SSM
        | SPI_CR1_SSI
        | (6<<SPI_CR1_BR_Pos)
        | SPI_CR1_MSTR
        | SPI_CR1_CPHA;
    SPI1->CR1 |= SPI_CR1_SPE;

    /* FIXME debug code */
    for (;;) {
        set_status_leds((sys_time & (1<<6)) ? STATUS_LED_OPERATION : 0);

        /* Toggle strobe */
        GPIOA->BSRR = 1<<4;
        for (int j = 0; j<100; j++)
            asm volatile ("nop");
        GPIOA->BRR = 1<<4;

        for (int j = 0; j<100000; j++)
            asm volatile ("nop");
    }
#if 0
    /* TIM3 running off 48MHz APB1 clk, T=20.833ns */
    TIM3->CR1 = 0; /* Disable ARR preload (double-buffering) */
    TIM3->PSC = 48-1; /* Prescaler 48 -> f=1MHz/T=1us */
    TIM3->DIER = TIM_DIER_UIE; /* Enable update (overflow) interrupt */
    TIM3->CCMR1 = 6<<TIM_CCMR1_OC1M_Pos | TIM_CCMR1_OC1PE; /* Configure output compare unit 1 to PWM mode 1, enable CCR1
                                                              preload */
    TIM3->CCER = TIM_CCER_CC1NE | TIM_CCER_CC1E; /* Confiugre CH1 to complementary outputs */
    TIM3->BDTR = TIM_BDTR_MOE | (0xc0 | (63-32))<<TIM_BDTR_DTG_Pos; /* Enable MOE on next update event, i.e. on initial timer load.
                                                          Set dead-time to 100us. */
    TIM3->CR1 |= TIM_CR1_CEN;
    TIM3->ARR = 800-1; /* Set f=2.5kHz/T=0.4ms */

    xfr_8b10b_encode_reset(&txstate.st);
    txstate.current_symbol = txstate.next_symbol = xfr_8b10b_encode(&txstate.st, K28_1) | 1<<10;
    TIM1->EGR |= TIM_EGR_UG;

    NVIC_EnableIRQ(TIM1_UP_IRQn);
    NVIC_SetPriority(TIM1_UP_IRQn, 3<<4);

    uint8_t txbuf[3] = {0x01, 0x05, 0x01};
    int txpos = -1;
    /* FIXME test code */
    //for (int i=0; i<sizeof(txbuf)/sizeof(txbuf[0]); i++)
    //    txbuf[i] = i;
    /* FIXME end test code */
    int i = 0;
    while (42) {
        if (txstate.next_symbol == -NO_SYMBOL) {
            if (txpos == -1)
                txstate.next_symbol = xfr_8b10b_encode(&txstate.st, -K28_1);
            else
                txstate.next_symbol = xfr_8b10b_encode(&txstate.st, txbuf[txpos]);

            txpos++;
            if (txpos >= sizeof(txbuf)/sizeof(txbuf[0])) {
                txpos = -1;

                i++;
                if (i == 5) {
                    //txbuf[2] = random();
                    //txbuf[2] <<= 1;
                    //if (!txbuf[2] & 0xff)
                    //    txbuf[2] = 0x01;
                    i = 0;
                }
            }
        }
    }
#endif
}

int flipbits(int in) {
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

void TIM1_UP_IRQHandler() {
    TIM1->SR &= ~TIM_SR_UIF;
    int sym = txstate.current_symbol;
    int bit = sym&1;
    sym >>= 1;
    if (sym == 1) { /* last bit shifted out */
        if (txstate.next_symbol == -NO_SYMBOL) /*FIXME debug code*/
            asm volatile("bkpt");
        sym = flipbits(txstate.next_symbol) | 1<<10;
        txstate.next_symbol = -NO_SYMBOL;
    }
    txstate.current_symbol = sym;

    TIM1->CCR1 = bit ? 0xffff : 0x0000;
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
    sys_time++;
}

void _init(void) {
}

void MemManage_Handler(void) __attribute__((naked));
void MemManage_Handler() {
    asm volatile ("bkpt");
}

void BusFault_Handler(void) __attribute__((naked));
void BusFault_Handler() {
asm volatile ("bkpt");
}
