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

#ifndef __MCP9801_H__
#define __MCP9801_H__

#define MCP9801_I2C_ADDR 0x90
#define MCP9801_I2C_PERIPH I2C1

#include <stdint.h>

enum mcp9801_regmap {
    MCP9801_REG_TEMP    = 0,
    MCP9801_REG_CONFIG  = 1,
    MCP9801_REG_HYST    = 2,
    MCP9801_REG_LIMIT   = 3
};

enum mcp9801_config {
    MCP9801_ONESHOT_ON  = 1<<7,
    MCP9801_ONESHOT_OFF = 0<<7,
    MCP9801_ONESHOT_Msk = 1<<7,

    MCP9801_RES_9BIT  = 0<<5,
    MCP9801_RES_10BIT = 1<<5,
    MCP9801_RES_11BIT = 2<<5,
    MCP9801_RES_12BIT = 3<<5,
    MCP9801_RES_Msk   = 3<<5,

    MCP9801_FAULT_QUEUE_1   = 0<<3,
    MCP9801_FAULT_QUEUE_2   = 1<<3,
    MCP9801_FAULT_QUEUE_4   = 2<<3,
    MCP9801_FAULT_QUEUE_6   = 3<<3,
    MCP9801_FAULT_QUEUE_Msk = 3<<3,

    MCP9801_ALERT_HIGH = 1<<2,
    MCP9801_ALERT_LOW  = 0<<2,
    MCP9801_ALERT_Msk  = 1<<2,

    MCP9801_MODE_INT  = 1<<1,
    MCP9801_MODE_COMP = 0<<1,
    MCP9801_MODE_Msk  = 1<<1,

    MCP9801_SHUTDOWN_ON  = 1<<0,
    MCP9801_SHUTDOWN_OFF = 0<<0,
    MCP9801_SHUTDOWN_Msk = 1<<0,
};

void mcp9801_init(void);
int32_t mcp9801_read_mdegC(void);

#endif /* __MCP9801_H__ */
