# Makefile to fool with openocd and the ST-Link connected to a black pill
# 11-20-2020

term:
	telnet localhost 4444

#	openocd -f /usr/share/openocd/scripts/interface/stlink-v2.cfg -f /usr/share/openocd/scripts/target/stm32f1x.cfg
#	openocd -f /usr/share/openocd/scripts/interface/stlink-v2.cfg -f /usr/share/openocd/scripts/target/cs32f1x.cfg

# I use the script instead
run_ocd:
	openocd -f /usr/share/openocd/scripts/interface/stlink-v2.cfg -f /usr/share/openocd/scripts/target/stm32f4x.cfg

OCDCFG = -f /usr/share/openocd/scripts/interface/stlink-v2.cfg -f /usr/share/openocd/scripts/target/stm32f4x.cfg

# The only problem here is we don't have blink.elf, only a bin file.
flash:  blink.elf
	openocd $(OCDCFG) -c "program blink.elf verify reset exit"


