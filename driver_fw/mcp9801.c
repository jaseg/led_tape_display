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
#include "i2c.h"
#include "mcp9801.h"

void mcp9801_init()
{
    uint8_t config = MCP9801_ONESHOT_OFF | MCP9801_RES_12BIT | MCP9801_FAULT_QUEUE_1 | MCP9801_ALERT_LOW | \
                     MCP9801_MODE_COMP | MCP9801_SHUTDOWN_OFF;
    uint8_t buf[2] = { MCP9801_REG_CONFIG, config };
    i2c_transmit(MCP9801_I2C_PERIPH, buf, sizeof(buf), MCP9801_I2C_ADDR, I2C_GENSTOP_YES);

    /* Address device here */
    uint8_t buf2[1] = {  MCP9801_REG_TEMP };
    i2c_transmit(MCP9801_I2C_PERIPH, buf2, sizeof(buf2), MCP9801_I2C_ADDR, I2C_GENSTOP_NO);
}

int32_t mcp9801_read_mdegC()
{
    uint8_t rx[2];
    i2c_receive(MCP9801_I2C_PERIPH, rx, sizeof(rx), MCP9801_I2C_ADDR);

    /* Fixup endianness, sign-extend */
    int32_t temp = (int16_t)((rx[0]<<8) | rx[1]);
    return (temp * 1000) /256;
}
