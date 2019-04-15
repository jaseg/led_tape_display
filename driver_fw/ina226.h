#ifndef __INA226_H__
#define __INA226_H__

#include <stdint.h>

#define INA226_I2C_PERIPH I2C1
#define INA226_I2C_ADDR 0x80

#define INA226_I_LSB_uA 2000
#define INA226_RSHUNT_uOhm 2000
/* FIXME validate this */
#define INA226_CAL (5120000 / INA226_I_LSB_uA * 1000 / INA226_RSHUNT_uOhm)

enum ina226_reg {
    INA226_REG_CONFIG   = 0x00,
    INA226_REG_VS       = 0x01,
    INA226_REG_VB       = 0x02,
    INA226_REG_P        = 0x03,
    INA226_REG_I        = 0x04,
    INA226_REG_CAL      = 0x05,
    INA226_REG_MASK_EN  = 0x06,
    INA226_REG_ALERT    = 0x07,
    INA226_REG_MFGID    = 0xfe,
    INA226_REG_DIEID    = 0xff
};

enum ina226_config {
    INA226_CONFIG_RST = 1<<15,

    INA226_CONFIG_AVG_Pos   = 9,
    INA226_CONFIG_AVG_Msk   = 3<<9,
    INA226_CONFIG_AVG_1     = 0<<9,
    INA226_CONFIG_AVG_4     = 1<<9,
    INA226_CONFIG_AVG_16    = 2<<9,
    INA226_CONFIG_AVG_64    = 3<<9,
    INA226_CONFIG_AVG_128   = 4<<9,
    INA226_CONFIG_AVG_256   = 5<<9,
    INA226_CONFIG_AVG_512   = 6<<9,
    INA226_CONFIG_AVG_1024  = 7<<9,

    INA226_CONFIG_VBUSCT_Pos    = 6,
    INA226_CONFIG_VBUSCT_Msk    = 7<<6,
    INA226_CONFIG_VBUSCT_140u   = 0<<6,
    INA226_CONFIG_VBUSCT_204u   = 1<<6,
    INA226_CONFIG_VBUSCT_332u   = 2<<6,
    INA226_CONFIG_VBUSCT_588u   = 3<<6,
    INA226_CONFIG_VBUSCT_1m1    = 4<<6,
    INA226_CONFIG_VBUSCT_2m116  = 5<<6,
    INA226_CONFIG_VBUSCT_4m156  = 6<<6,
    INA226_CONFIG_VBUSCT_8m244  = 7<<6,

    INA226_CONFIG_VSHCT_Pos     = 3,
    INA226_CONFIG_VSHCT_Msk     = 7<<3,
    INA226_CONFIG_VSHCT_140u    = 0<<3,
    INA226_CONFIG_VSHCT_204u    = 1<<3,
    INA226_CONFIG_VSHCT_332u    = 2<<3,
    INA226_CONFIG_VSHCT_588u    = 3<<3,
    INA226_CONFIG_VSHCT_1m1     = 4<<3,
    INA226_CONFIG_VSHCT_2m116   = 5<<3,
    INA226_CONFIG_VSHCT_4m156   = 6<<3,
    INA226_CONFIG_VSHCT_8m244   = 7<<3,

    INA226_CONFIG_MODE_Pos          = 0,
    INA226_CONFIG_MODE_Msk          = 7,
    INA226_CONFIG_MODE_POWERDOWN    = 0,

    INA226_CONFIG_MODE_SHUNT        = 1,

    INA226_CONFIG_MODE_BUS          = 2,

    INA226_CONFIG_MODE_TRIG         = 0,
    INA226_CONFIG_MODE_CONT         = 4
};

void ina226_init(void);
void ina226_write_reg(uint8_t reg, uint16_t val);
uint16_t ina226_read_reg(uint8_t reg);

static inline int16_t ina226_read_i(void) { return (int16_t)ina226_read_reg(INA226_REG_I); }
static inline int16_t ina226_read_v(void) { return (int16_t)ina226_read_reg(INA226_REG_VB); }
static inline uint16_t ina226_read_p(void) { return ina226_read_reg(INA226_REG_P); }
static inline int16_t ina226_read_vs(void) { return (int16_t)ina226_read_reg(INA226_REG_VS); }

#endif /* __INA226_H__ */
