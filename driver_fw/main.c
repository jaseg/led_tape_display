

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstrict-aliasing"
#include <stm32f1xx.h>
#pragma GCC diagnostic pop

#include <system_stm32f1xx.h>

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

int main(void) {
    /* External crystal: 8MHz */
    RCC->CR |= RCC_CR_HSEON;
    while (!(RCC->CR&RCC_CR_HSERDY));

    /* Sysclk = HCLK = 48MHz */
    RCC->CFGR = (RCC->CFGR & (~RCC_CFGR_PLLMULL_Msk & ~RCC_CFGR_SW_Msk & ~RCC_CFGR_PPRE1_Msk & ~RCC_CFGR_PPRE2_Msk &
                ~RCC_CFGR_HPRE_Msk))
        | (10<<RCC_CFGR_PLLMULL_Pos) | RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLSRC | (4<<RCC_CFGR_PPRE1_Pos) |
        (4<<RCC_CFGR_PPRE2_Pos);

    RCC->CR |= RCC_CR_PLLON;
    while (!(RCC->CR&RCC_CR_PLLRDY));

    /* Switch to PLL */
    RCC->CFGR |= (2<<RCC_CFGR_SW_Pos);
    //RCC->CFGR = (RCC->CFGR & (~RCC_CFGR_PPRE1_Msk & ~RCC_CFGR_PPRE2_Msk))
    //    | (4<<RCC_CFGR_PPRE1_Pos) | (4<<RCC_CFGR_PPRE2_Pos);
    SystemCoreClockUpdate();

    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN | RCC_APB2ENR_IOPCEN | RCC_APB2ENR_TIM1EN;

    GPIOA->CRH =
          (2<<GPIO_CRH_CNF8_Pos) | (1<<GPIO_CRH_MODE8_Pos);   /* PA8  - low side */

    GPIOB->CRH =
          (2<<GPIO_CRH_CNF13_Pos) | (1<<GPIO_CRH_MODE13_Pos); /* PB13 - high side */

    GPIOC->CRH =
          (0<<GPIO_CRH_CNF13_Pos) | (1<<GPIO_CRH_MODE13_Pos); /* PC13 - LED */

    /* TIM1 running off 24MHz APB2 clk, T=41.667ns */
    TIM1->CR1 = 0; /* Disable ARR preload (double-buffering) */
    TIM1->PSC = 24-1; /* Prescaler 24 -> f=1MHz/T=1us */
    TIM1->DIER = TIM_DIER_UIE; /* Enable update (overflow) interrupt */
    TIM1->CCMR1 = 6<<TIM_CCMR1_OC1M_Pos | TIM_CCMR1_OC1PE; /* Configure output compare unit 1 to PWM mode 1, enable CCR1
                                                              preload */
    TIM1->CCER = TIM_CCER_CC1NE | TIM_CCER_CC1E; /* Confiugre CH1 to complementary outputs */
    TIM1->BDTR = TIM_BDTR_MOE | 100<<TIM_BDTR_DTG_Pos; /* Enable MOE on next update event, i.e. on initial timer load.
                                                          Set dead-time to 100us. */
    TIM1->CR1 |= TIM_CR1_CEN;
    TIM1->ARR = 1000-1; /* Set f=1.0kHz/T=1.0ms */

    xfr_8b10b_encode_reset(&txstate.st);
    txstate.current_symbol = txstate.next_symbol = xfr_8b10b_encode(&txstate.st, K28_1) | 1<<10;
    TIM1->EGR |= TIM_EGR_UG;

    NVIC_EnableIRQ(TIM1_UP_IRQn);
    NVIC_SetPriority(TIM1_UP_IRQn, 3<<4);

    uint8_t txbuf[128];
    int txpos = -1;
    /* FIXME test code */
    for (int i=0; i<sizeof(txbuf); i++)
        txbuf[i] = i;
    /* FIXME end test code */
    while (42) {
        if (txstate.next_symbol == -NO_SYMBOL) {
            if (txpos == -1)
                txstate.next_symbol = xfr_8b10b_encode(&txstate.st, K28_1);
            else
                txstate.next_symbol = xfr_8b10b_encode(&txstate.st, txbuf[txpos]);

            txpos++;
            if (txpos == sizeof(txbuf))
                txpos = -1;
        }
    }
}

void TIM1_UP_IRQHandler() {
    TIM1->SR &= ~TIM_SR_UIF;
    int sym = txstate.current_symbol;
    int bit = sym&1;
    sym >>= 1;
    if (sym == 1) { /* last bit shifted out */
        sym = txstate.next_symbol | 1<<10;
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
