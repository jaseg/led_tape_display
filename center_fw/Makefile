# Megumin LED display firmware
# Copyright (C) 2018 Sebastian Götte <code@jaseg.net>
# 
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

CUBE_PATH 		?= $(wildcard ~)/resource/STM32CubeF0
CMSIS_PATH 		?= $(CUBE_PATH)/Drivers/CMSIS
CMSIS_DEV_PATH  ?= $(CMSIS_PATH)/Device/ST/STM32F0xx
HAL_PATH   		?= $(CUBE_PATH)/Drivers/STM32F0xx_HAL_Driver

MAC_ADDR        ?= 0xdeadbeef

CC      := arm-none-eabi-gcc
LD      := arm-none-eabi-ld
OBJCOPY := arm-none-eabi-objcopy
OBJDUMP := arm-none-eabi-objdump
SIZE    := arm-none-eabi-size

CFLAGS  = -g -Wall -std=gnu11 -O0 -fdump-rtl-expand -DMAC_ADDR=$(MAC_ADDR) -DADC_BUFSIZE=1024
CFLAGS += -mlittle-endian -mcpu=cortex-m0 -march=armv6-m -mthumb
#CFLAGS += -ffunction-sections -fdata-sections
LDFLAGS = -nostartfiles
#LDFLAGS += -specs=rdimon.specs -DSEMIHOSTING
LDFLAGS += -Wl,-Map=main.map -nostdlib
#LDFLAGS += -Wl,--gc-sections 
LIBS = -lgcc
#LIBS += -lrdimon

# Technically we're using an STM32F030F4, but apart from the TSSOP20 package that one is largely identical to the
# STM32F030*6 and there is no separate device header provided for it, so we're faking a *6 device here. This is
# even documented in stm32f0xx.h. Thanks ST!
CFLAGS += -DSTM32F030x6 -DHSE_VALUE=8000000

LDFLAGS += -Tstm32_flash.ld
CFLAGS += -I$(CMSIS_DEV_PATH)/Include -I$(CMSIS_PATH)/Include -I$(HAL_PATH)/Inc -Iconfig -Wno-unused -I../common
LDFLAGS += -L$(CMSIS_PATH)/Lib/GCC -larm_cortexM0l_math

###################################################

.PHONY: program clean

all: main.elf

.clang:
	echo flags = $(CFLAGS) > .clang 

cmsis_exports.c: $(CMSIS_DEV_PATH)/Include/stm32f030x6.h $(CMSIS_PATH)/Include/core_cm0.h
	python3 tools/gen_cmsis_exports.py $^ > $@

%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $^
#	$(CC) -E $(CFLAGS) -o $(@:.o=.pp) $^

%.o: %.s
	$(CC) -c $(CFLAGS) -o $@ $^
#	$(CC) -E $(CFLAGS) -o $(@:.o=.pp) $^

%.dot: %.elf
	r2 -a arm -qc 'aa;agC' $< 2>/dev/null >$@
	
sources.tar.xz: main.c adc.c ../common/8b10b.c Makefile
	tar -caf $@ $^

# don't ask...
sources.tar.xz.zip: sources.tar.xz
	zip $@ $^

sources.c: sources.tar.xz.zip
	xxd -i $< | head -n -1 | sed 's/=/__attribute__((section(".source_tarball"))) =/' > $@

main.elf: main.c startup_stm32f030x6.s system_stm32f0xx.c $(HAL_PATH)/Src/stm32f0xx_ll_utils.c base.c cmsis_exports.c ../common/8b10b.c adc.c protocol.c 8seg_protocol.c transmit.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)
	$(OBJCOPY) -O ihex $@ $(@:.elf=.hex)
	$(OBJCOPY) -O binary $@ $(@:.elf=.bin)
	$(OBJDUMP) -St $@ >$(@:.elf=.lst)
	$(SIZE) $@
	
program: main.elf openocd.cfg
	openocd -f openocd.cfg -c "program $< verify reset exit"

8b10b_test_encode: 8b10b_test_encode.c 8b10b.c
	gcc -o $@ $^

8b10b_test_decode: 8b10b_test_decode.c 8b10b.c
	gcc -o $@ $^

protocol_test: protocol.c protocol_test.c
	gcc -o $@ -O0 -Wall -Wextra -g -I../common $^

clean:
	rm -f **.o
	rm -f main.elf main.hex main.bin main.map main.lst
	rm -f **.expand
	rm -f cmsis_exports.c
	rm -f sources.tar.xz
	rm -f sources.tar.xz.zip
	rm -f sources.c
	rm -f *.dot
	rm -f protocol_test

