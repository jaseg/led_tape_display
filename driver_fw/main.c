

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstrict-aliasing"
#include <stm32f1xx.h>
#pragma GCC diagnostic pop

#include <system_stm32f1xx.h>

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

/* Part number: STM32F030F4C6 */

static volatile unsigned int sys_time;

uint32_t get_tick() {
    return SysTick->VAL;
}

int main(void) {
    /* External crystal: 8MHz */
    RCC->CR |= RCC_CR_HSEON;
    while (!(RCC->CR&RCC_CR_HSERDY));

    /* Sysclk = HCLK = 48MHz */
    RCC->CFGR = (RCC->CFGR & (~RCC_CFGR_PLLMULL_Msk & ~RCC_CFGR_SW_Msk & ~RCC_CFGR_PPRE1_Msk & ~RCC_CFGR_PPRE2_Msk & ~RCC_CFGR_HPRE_Msk))
        | (10<<RCC_CFGR_PLLMULL_Pos) | RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLSRC | (4<<RCC_CFGR_PPRE1_Pos) | (4<<RCC_CFGR_PPRE2_Pos);

    RCC->CR |= RCC_CR_PLLON;
    while (!(RCC->CR&RCC_CR_PLLRDY));

    /* Switch to PLL */
    RCC->CFGR |= (2<<RCC_CFGR_SW_Pos);
    //RCC->CFGR = (RCC->CFGR & (~RCC_CFGR_PPRE1_Msk & ~RCC_CFGR_PPRE2_Msk))
    //    | (4<<RCC_CFGR_PPRE1_Pos) | (4<<RCC_CFGR_PPRE2_Pos);
    SystemCoreClockUpdate();

    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN | RCC_APB2ENR_IOPCEN;

    GPIOA->CRL =
          (0<<GPIO_CRL_CNF6_Pos) | (1<<GPIO_CRL_MODE6_Pos)  /* PA6  - Channel 1 low side */
        | (0<<GPIO_CRL_CNF7_Pos) | (1<<GPIO_CRL_MODE7_Pos); /* PA7  - Channel 2 low side */

    GPIOB->CRL =
          (0<<GPIO_CRL_CNF0_Pos) | (1<<GPIO_CRL_MODE0_Pos)  /* PB0  - Channel 1 high side */
        | (0<<GPIO_CRL_CNF1_Pos) | (1<<GPIO_CRL_MODE1_Pos); /* PB1  - Channel 2 high side */

    GPIOC->CRH =
          (0<<GPIO_CRH_CNF13_Pos) | (1<<GPIO_CRH_MODE13_Pos); /* PC13 - LED */

    /* Turn all outputs off */
    GPIOA->BRR |= 1<<6 | 1<<7;
    GPIOB->BRR |= 1<<0 | 1<<1;

    while (42) {
#define FOO 100000
        for (int i=0; i<FOO; i++) ;
        GPIOA->BRR |= 1<<6 | 1<<7;
        GPIOB->BRR |= 1<<0 | 1<<1;

        GPIOA->BSRR |= 1<<6;
        GPIOB->BSRR |= 1<<1;

        for (int i=0; i<FOO; i++) ;
        GPIOA->BRR |= 1<<6 | 1<<7;
        GPIOB->BRR |= 1<<0 | 1<<1;

        GPIOA->BSRR |= 1<<7;
        GPIOB->BSRR |= 1<<0;

        GPIOC->ODR ^= 1<<13;
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
