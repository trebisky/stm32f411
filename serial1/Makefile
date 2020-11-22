# Makefile for serial demo
#
# basic seria/uart driver for the STM32F411.

TOOLS = arm-none-eabi

# Assembling with gcc makes it want crt0 at link time.
#AS = $(TOOLS)-gcc
AS = $(TOOLS)-as

# Use the -g flag if you intend to use gdb
#CC = $(TOOLS)-gcc -mcpu=cortex-m4 -mthumb
#CC = $(TOOLS)-gcc -mcpu=cortex-m4 -mthumb -g
#CC = $(TOOLS)-gcc -mcpu=cortex-m4 -mthumb -Os

# In truth the implicit fn warnings are good, but for the purpose of
#  these demos, I can't be busy to set up prototypes as I should.
CC = $(TOOLS)-gcc -mcpu=cortex-m4 -mthumb -Wno-implicit-function-declaration

#LD = $(TOOLS)-gcc
LD = $(TOOLS)-ld.bfd
OBJCOPY = $(TOOLS)-objcopy
DUMP = $(TOOLS)-objdump -d
GDB = $(TOOLS)-gdb

OBJS = locore.o main.o led.o serial.o

all: serial.elf serial.dump

serial.dump:	serial.elf
	$(DUMP) serial.elf >serial.dump

serial.elf: 	$(OBJS)
	$(LD) -T serial.lds -o serial.elf $(OBJS)

locore.o:	locore.s
	$(AS) locore.s -o locore.o

.c.o:
	$(CC) -o $@ -c $<

OCDCFG = -f /usr/share/openocd/scripts/interface/stlink-v2.cfg -f /usr/share/openocd/scripts/target/stm32f4x.cfg

flash:  serial.elf
	openocd $(OCDCFG) -c "program serial.elf verify reset exit"

gdb:
	$(GDB) --eval-command="target remote localhost:3333" serial.elf

gdbtui:
	$(GDB) -tui --eval-command="target remote localhost:3333" serial.elf

clean:
	rm -f *.o serial.elf serial.dump
