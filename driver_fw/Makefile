CUBE_PATH 		?= $(wildcard ~)/resource/STM32Cube_FW_F0_V1.9.0
CMSIS_PATH 		?= $(CUBE_PATH)/Drivers/CMSIS
CMSIS_DEV_PATH  ?= $(CMSIS_PATH)/Device/ST/STM32F0xx
HAL_PATH   		?= $(CUBE_PATH)/Drivers/STM32F0xx_HAL_Driver

MAC_ADDR        ?= 0xdeadbeef

CC      := arm-none-eabi-gcc
LD      := arm-none-eabi-ld
OBJCOPY := arm-none-eabi-objcopy
OBJDUMP := arm-none-eabi-objdump
SIZE    := arm-none-eabi-size

CFLAGS  = -g -Wall -std=gnu11 -O0 -fdump-rtl-expand -Wno-discarded-qualifiers
CFLAGS += -mlittle-endian -mcpu=cortex-m0 -mthumb
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
CFLAGS += -DSTM32F030x6 -DHSE_VALUE=8000000 -DMAC_ADDR=$(MAC_ADDR)

LDFLAGS += -Tstm32_flash.ld
CFLAGS += -I$(CMSIS_DEV_PATH)/Include -I$(CMSIS_PATH)/Include -I$(HAL_PATH)/Inc -Iconfig -I../common
#LDFLAGS += -L$(CMSIS_PATH)/Lib/GCC -larm_cortexM0l_math

SOURCES = main.c startup_stm32f030x6.s system_stm32f0xx.c base.c $(HAL_PATH)/Src/stm32f0xx_ll_utils.c cmsis_exports.c \
		  ../common/8b10b.c serial.c mac.c i2c.c lcd1602.c mcp9801.c ina226.c mini-printf.c

###################################################

.PHONY: program clean

all: main.elf

.clang:
	echo flags = $(CFLAGS) > .clang 

cmsis_exports.c: $(CMSIS_DEV_PATH)/Include/stm32f030x6.h $(CMSIS_PATH)/Include/core_cm0.h
	python3 gen_cmsis_exports.py $^ > $@

%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $^
#	$(CC) -E $(CFLAGS) -o $(@:.o=.pp) $^

%.o: %.s
	$(CC) -c $(CFLAGS) -o $@ $^
#	$(CC) -E $(CFLAGS) -o $(@:.o=.pp) $^

%.dot: %.elf
	r2 -a arm -qc 'aa;agC' $< 2>/dev/null >$@

main.elf: $(SOURCES)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)
	$(OBJCOPY) -O ihex $@ $(@:.elf=.hex)
	$(OBJCOPY) -O binary $@ $(@:.elf=.bin)
	$(OBJDUMP) -St $@ >$(@:.elf=.lst)
	$(SIZE) $@
	
program: main.elf openocd.cfg
	openocd -f openocd.cfg -c "program $< verify reset exit"

clean:
	rm -f **.o **.pp
	rm -f main.elf main.hex main.bin main.map main.lst
	rm -f **.expand

