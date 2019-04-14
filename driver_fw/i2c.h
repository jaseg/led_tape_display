#ifndef __I2C_H
#define __I2C_H

#include "global.h"

// I2C HAL

// I2C1
// SCL [PB6, PB8]
#define I2C1_SCL_GPIO_PERIPH    RCC_AHB2ENR_GPIOBEN
#define I2C1_SCL_GPIO_PORT      GPIOB
#define I2C1_SCL_GPIO_PIN       GPIO_PIN_8
#define I2C1_SCL_GPIO_SRC       GPIO_PinSource8
// SDA [PB7, PB9]
#define I2C1_SDA_GPIO_PERIPH    RCC_AHB2ENR_GPIOBEN
#define I2C1_SDA_GPIO_PORT      GPIOB
#define I2C1_SDA_GPIO_PIN       GPIO_PIN_9
#define I2C1_SDA_GPIO_SRC       GPIO_PinSource9

// I2C2
// SCL [PB10, PB13]
#define I2C2_SCL_GPIO_PERIPH    RCC_AHB2ENR_GPIOBEN
#define I2C2_SCL_GPIO_PORT      GPIOB
#define I2C2_SCL_GPIO_PIN       GPIO_PIN_10
#define I2C2_SCL_GPIO_SRC       GPIO_PinSource10
// SDA [PB11, PB14]
#define I2C2_SDA_GPIO_PERIPH    RCC_AHB2ENR_GPIOBEN
#define I2C2_SDA_GPIO_PORT      GPIOB
#define I2C2_SDA_GPIO_PIN       GPIO_PIN_11
#define I2C2_SDA_GPIO_SRC       GPIO_PinSource11

// I2C3
// SCL [PC0]
#define I2C3_SCL_GPIO_PERIPH    RCC_AHB2ENR_GPIOCEN
#define I2C3_SCL_GPIO_PORT      GPIOC
#define I2C3_SCL_GPIO_PIN       GPIO_PIN_0
#define I2C3_SCL_GPIO_SRC       GPIO_PinSource0
// SDA [PC1]
#define I2C3_SDA_GPIO_PERIPH    RCC_AHB2ENR_GPIOCEN
#define I2C3_SDA_GPIO_PORT      GPIOC
#define I2C3_SDA_GPIO_PIN       GPIO_PIN_1
#define I2C3_SDA_GPIO_SRC       GPIO_PinSource1


// Definitions of I2C analog filter state
#define I2C_AF_ENABLE          ((uint32_t)0x00000000U) // Analog filter is enabled
#define I2C_AF_DISABLE         I2C_CR1_ANFOFF          // Analog filter is disabled

// Flags definitions for transmit function
#define I2C_TX_STOP            ((uint32_t)0x00000000U) // Generate STOP condition
#define I2C_TX_NOSTOP          ((uint32_t)0x10000000U) // Don't generate STOP condition
#define I2C_TX_NOSTART         ((uint32_t)0x20000000U) // Don't generate START condition
#define I2C_TX_CONT            ((uint32_t)0x40000000U) // The transmission will be continued
// Definitions for compatibility with old code using this library
#define I2C_GENSTOP_YES        I2C_TX_STOP
#define I2C_GENSTOP_NO         I2C_TX_NOSTOP

// Definition of bits to reset in CR2 register
#define I2C_CR2_ALL            (I2C_CR2_SADD    | \
								I2C_CR2_NBYTES  | \
								I2C_CR2_RELOAD  | \
								I2C_CR2_AUTOEND | \
								I2C_CR2_RD_WRN  | \
								I2C_CR2_START   | \
								I2C_CR2_STOP)

// Definition of all bits in ICR register (clear all I2C flags at once)
#define I2C_ICR_ALL            (I2C_ICR_ADDRCF  | \
								I2C_ICR_ALERTCF | \
								I2C_ICR_ARLOCF  | \
								I2C_ICR_BERRCF  | \
								I2C_ICR_NACKCF  | \
								I2C_ICR_OVRCF   | \
								I2C_ICR_PECCF   | \
								I2C_ICR_STOPCF  | \
								I2C_ICR_TIMOUTCF)


// Result of I2C functions
typedef enum {
	I2C_ERROR   = 0,
	I2C_SUCCESS = !I2C_ERROR
} I2CSTATUS;


// Public functions and macros

// Enable I2C peripheral
// input:
//   I2Cx - pointer to the I2C peripheral (I2C1, etc.)
static inline void i2c_enable(I2C_TypeDef* I2Cx) {
	I2Cx->CR1 |= I2C_CR1_PE;
}

// Disable I2C peripheral
// input:
//   I2Cx - pointer to the I2C peripheral (I2C1, etc.)
static inline void i2c_disable(I2C_TypeDef* I2Cx) {
	I2Cx->CR1 &= ~I2C_CR1_PE;
}

// Configure I2C noise filters
// input:
//   I2Cx - pointer to the I2C peripheral (I2C1, etc.)
//   af - analog filter state, I2C_AF_DISABLE or I2C_AF_ENABLE
//   df - digital filter configuration, can be a value in range from 0 to 15
//        zero value means the digital filter is disabled
//        this values means filtering capability up to (df * ti2cclk)
// note: must be called only when I2C is disabled (PE bit in I2C_CR1 register is reset)
static inline void i2c_config_filters(I2C_TypeDef* I2Cx, uint32_t af, uint32_t df) {
	I2Cx->CR1 &= ~(I2C_CR1_ANFOFF | I2C_CR1_DNF);
	I2Cx->CR1 |= (af & I2C_CR1_ANFOFF) | ((df << I2C_CR1_DNF_Pos) & I2C_CR1_DNF);
}

// Configure the I2C timings (SDA setup/hold time and SCL high/low period)
// input:
//   I2Cx - pointer to the I2C peripheral (I2C1, etc.)
//   timing - the value for I2C_TIMINGR register
// note: must be called only when I2C is disabled (PE bit in I2C_CR1 register is reset)
static inline void i2c_config_timing(I2C_TypeDef* I2Cx, uint32_t timing) {
	I2Cx->TIMINGR = timing;
}

// Generate START condition
// input:
//   I2Cx - pointer to the I2C peripheral (I2C1, etc.)
//   addr - I2C device address
// note: 7-bit addressing mode
static inline void i2c_genstart(I2C_TypeDef* I2Cx, uint32_t addr) {
	I2Cx->CR2 = (addr & I2C_CR2_SADD) | I2C_CR2_START | I2C_CR2_AUTOEND;
}

// Generate STOP condition
// input:
//   I2Cx - pointer to the I2C peripheral (I2C1, etc.)
static inline void i2c_genstop(I2C_TypeDef* I2Cx) {
	I2Cx->CR2 |= I2C_CR2_STOP;
}


// Function prototypes
I2CSTATUS i2c_is_device_ready(I2C_TypeDef* I2Cx, uint8_t devAddr, uint32_t Trials);
I2CSTATUS i2c_transmit(I2C_TypeDef* I2Cx, const uint8_t *pBuf, uint32_t nbytes, uint8_t devAddr, uint32_t flags);
I2CSTATUS i2c_receive(I2C_TypeDef* I2Cx, uint8_t *pBuf, uint32_t nbytes, uint8_t devAddr);

#endif // __I2C_H
