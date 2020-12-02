# Makefile for delay demo
#

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
CC = $(TOOLS)-gcc -mcpu=cortex-m4 -mthumb -Wno-implicit-function-declaration -fno-builtin -O

#LD = $(TOOLS)-gcc
LD = $(TOOLS)-ld.bfd
OBJCOPY = $(TOOLS)-objcopy
DUMP = $(TOOLS)-objdump -d
GDB = $(TOOLS)-gdb

OBJS = locore.o init.o main.o rcc.o gpio.o serial.o nvic.o systick.o event.o

# *.o:	f411.h

all: delay.elf delay.dump delay.bin

# Look at object file sections
zoot:
	$(TOOLS)-objdump -h *.o
	$(TOOLS)-objdump -h delay.elf

delay.dump:	delay.elf
	$(DUMP) delay.elf >delay.dump

delay.elf: 	$(OBJS) f411.lds
	$(LD) -T f411.lds -o delay.elf $(OBJS)

delay.bin:        delay.elf
	$(OBJCOPY) delay.elf delay.bin -O binary

locore.o:	locore.s
	$(AS) locore.s -o locore.o

.c.o:
	$(CC) -o $@ -c $<

OCDCFG = -f /usr/share/openocd/scripts/interface/stlink-v2.cfg -f /usr/share/openocd/scripts/target/stm32f4x.cfg

flash:  delay.elf
	openocd $(OCDCFG) -c "program delay.elf verify reset exit"

ocd:
	openocd $(OCDCFG)

# I tried using gdb on Fedora 32, and apparently it is gone!
# It was last supplied as a Fedora package in Fedora 29, no telling why.

gdb:
	$(GDB) --eval-command="target remote localhost:3333" delay.elf

gdbtui:
	$(GDB) -tui --eval-command="target remote localhost:3333" delay.elf

clean:
	rm -f *.o delay.elf delay.dump delay.bin
