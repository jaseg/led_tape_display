
#include "global.h"
#include "i2c.h"
#include "ina226.h"

void ina226_init() {
    /* FIXME plug in final values for I meas backchannel */
    uint16_t config = INA226_CONFIG_AVG_1 | INA226_CONFIG_VBUSCT_204u | INA226_CONFIG_VSHCT_4m156 |
                     INA226_CONFIG_MODE_SHUNT | INA226_CONFIG_MODE_BUS | INA226_CONFIG_MODE_CONT;
    ina226_write_reg(INA226_REG_CONFIG, config);

    ina226_write_reg(INA226_REG_CAL, INA226_CAL);
}

void ina226_write_reg(uint8_t reg, uint16_t val) {
    uint8_t buf[3] = { reg, val>>8, val&0xff };
    i2c_transmit(INA226_I2C_PERIPH, buf, sizeof(buf), INA226_I2C_ADDR, I2C_GENSTOP_YES);
}

uint16_t ina226_read_reg(uint8_t reg) {
    uint8_t buf2[1] = { reg };
    i2c_transmit(INA226_I2C_PERIPH, buf2, sizeof(buf2), INA226_I2C_ADDR, I2C_GENSTOP_NO);
    uint8_t rx[2];
    i2c_receive(INA226_I2C_PERIPH, rx, sizeof(rx), INA226_I2C_ADDR);
    return (rx[0]<<8) | rx[1];
}

