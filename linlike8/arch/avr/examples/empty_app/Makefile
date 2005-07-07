# This Makefile -----------------------------------------------------
#
#	this example as a template for any project to start
#	aims to avr arch (atmega32)
#
# -------------------------------------------------------------------
#
#	make config		-- setup middleware(os, ui, uclib ...)
#	make			-- compile all
#	make clean		-- clean compiled files
#	make mrporoper	-- clean middleware and compiled files
#

# parameters ---------------------------------------------------------
MCU=atmega32

# variables -----------------------------------------------------------
#	application -----------------
TARGT := helloworld
PATH_OF_LINLIKE8 := ../../../../../linlike8
PATH_OF_APP := .
OBJ := $(PATH_OF_LINLIKE8)/arch/avr/boot/boot.o $(PATH_OF_LINLIKE8)/arch/avr/kernel/setup.o \
	$(PATH_OF_LINLIKE8)/arch/avr/drivers/uart.o $(PATH_OF_LINLIKE8)/arch/avr/drivers/eeprom.o\
	$(PATH_OF_LINLIKE8)/arch/avr/kernel/time.o $(PATH_OF_LINLIKE8)/arch/avr/drivers/io.o \
	$(PATH_OF_LINLIKE8)/kernel/softirq.o $(PATH_OF_LINLIKE8)/init/main.o $(PATH_OF_LINLIKE8)/lib/cirbuf.o \
	$(PATH_OF_LINLIKE8)/kernel/timer.o $(PATH_OF_APP)/srvmain.o
#	system -----------------
CC=avr-gcc
OBJCOPY=avr-objcopy
CFLAGS=-g -mmcu=$(MCU) -Wall -Wstrict-prototypes -Os -mcall-prologues -I$(PATH_OF_LINLIKE8)/include/
RM_STUFFS := $(OBJ) $(TARGT).out $(TARGT).hex $(TARGT).map

# compile and linking -------------------------------------------------
all: $(TARGT).hex

$(TARGT).hex: $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGT).out -Wl,-Map,$(TARGT).map $(OBJ)
	$(OBJCOPY) -R .eeprom -O ihex $(TARGT).out $(TARGT).hex 

config:
	cp $(PATH_OF_LINLIKE8)/config/config.h.demo $(PATH_OF_LINLIKE8)/include/linlike8/config.h

clean:
	rm -f $(RM_STUFFS)

mrproper: clean
	rm -f $(PATH_OF_LINLIKE8)/include/linlike8/config.h