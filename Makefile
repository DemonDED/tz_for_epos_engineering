TARGET = ld1_blink

PREFIX = arm-none-eabi-
CC = $(PREFIX)gcc
LD = $(PREFIX)ld
OBJCOPY = $(PREFIX)objcopy
SIZE = $(PREFIX)size

CFLAGS = -mcpu=cortex-m7 -mthumb -nostdlib -nostartfiles -ffreestanding -O2
CFLAGS += -DSTM32H745xx

SRCS = ld1_blink.c
OBJS = $(SRCS:.c=.o)
LSCRIPT = stm32h745xi_flash.ld

all: $(TARGET).bin

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET).elf: $(OBJS) $(LSCRIPT)
	$(LD) -T $(LSCRIPT) $(OBJS) -o $@
	$(SIZE) $@

$(TARGET).bin: $(TARGET).elf
	$(OBJCOPY) -O binary $< $@

clean:
	rm -f $(OBJS) $(TARGET).elf $(TARGET).bin

# Команда flash теперь должна работать корректно, так как адреса совпадают с Flash
flash: $(TARGET).bin
	openocd -f interface/stlink.cfg -f target/stm32h7x.cfg -c "program $(TARGET).elf verify reset exit"

.PHONY: all clean flash
