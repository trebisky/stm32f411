# Makefile for button demo
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

all: button.elf button.dump button.bin

# Look at object file sections
zoot:
	$(TOOLS)-objdump -h *.o
	$(TOOLS)-objdump -h button.elf

button.dump:	button.elf
	$(DUMP) button.elf >button.dump

button.elf: 	$(OBJS) f411.lds
	$(LD) -T f411.lds -o button.elf $(OBJS)

button.bin:        button.elf
	$(OBJCOPY) button.elf button.bin -O binary

locore.o:	locore.s
	$(AS) locore.s -o locore.o

.c.o:
	$(CC) -o $@ -c $<

OCDCFG = -f /usr/share/openocd/scripts/interface/stlink-v2.cfg -f /usr/share/openocd/scripts/target/stm32f4x.cfg

flash:  button.elf
	openocd $(OCDCFG) -c "program button.elf verify reset exit"

ocd:
	openocd $(OCDCFG)

# I tried using gdb on Fedora 32, and apparently it is gone!
# It was last supplied as a Fedora package in Fedora 29, no telling why.

gdb:
	$(GDB) --eval-command="target remote localhost:3333" button.elf

gdbtui:
	$(GDB) -tui --eval-command="target remote localhost:3333" button.elf

clean:
	rm -f *.o button.elf button.dump button.bin
