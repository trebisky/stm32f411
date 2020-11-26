# Makefile for bss demo
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
CC = $(TOOLS)-gcc -mcpu=cortex-m4 -mthumb -Wno-implicit-function-declaration -fno-builtin

#LD = $(TOOLS)-gcc
LD = $(TOOLS)-ld.bfd
OBJCOPY = $(TOOLS)-objcopy
DUMP = $(TOOLS)-objdump -d
GDB = $(TOOLS)-gdb

OBJS = locore.o init.o main.o rcc.o gpio.o serial.o nvic.o systick.o

all: bss.elf bss.dump bss.bin

# Look at object file sections
zoot:
	$(TOOLS)-objdump -h *.o
	$(TOOLS)-objdump -h bss.elf

bss.dump:	bss.elf
	$(DUMP) bss.elf >bss.dump

bss.elf: 	$(OBJS) f411.lds
	$(LD) -T f411.lds -o bss.elf $(OBJS)

bss.bin:        bss.elf
	$(OBJCOPY) bss.elf bss.bin -O binary

locore.o:	locore.s
	$(AS) locore.s -o locore.o

.c.o:
	$(CC) -o $@ -c $<

OCDCFG = -f /usr/share/openocd/scripts/interface/stlink-v2.cfg -f /usr/share/openocd/scripts/target/stm32f4x.cfg

flash:  bss.elf
	openocd $(OCDCFG) -c "program bss.elf verify reset exit"

ocd:
	openocd $(OCDCFG)

gdb:
	$(GDB) --eval-command="target remote localhost:3333" bss.elf

gdbtui:
	$(GDB) -tui --eval-command="target remote localhost:3333" bss.elf

clean:
	rm -f *.o bss.elf bss.dump bss.bin
