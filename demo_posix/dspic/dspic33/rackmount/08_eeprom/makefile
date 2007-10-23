# This Makefile =========================================================================
#	make config		-- create soft link /asm for local dspic configuration
#	make			-- compile all
#						Steps:
#						1) Compile $(obj-y) with no linking (.c -> .o)
#						2) Link all *.o to $(TARGET)
#						3) Convert $(TARGET) to $(TARGET_HEX)
#	make clean		-- clear all objects, bin, hex and map files
#	make mrproper	-- clear all objects, bin, hex, map files and soft links
#========================================================================================

# Note on Linker script =================================================================
# The official linker script for dsPic33FGJ128GP306 is p33FJ128GP306.gld, found 
# under /usr/pic30-elf/support/gld. A customized version, clinkscript_p33FJ128GP306.gld
# is included in this project to ensure that the _resetPRI function is located
# at address 0x200 so that the bootloader can always point this address.
# Please also include reset.s in the compilation list to achieve the desired 
# function.
#========================================================================================

# PROJECT NAME --------------------------------------------------------------------------
#	Target file (*.hex) will have the same name as the project name
PRJ_NAME := 08_eeprom
#----------------------------------------------------------------------------------------

# CONFIG --------------------------------------------------------------------------------
#	Most configuration for project should be changed in .config file
include .config
#----------------------------------------------------------------------------------------

# DIRECTORY LABEL -----------------------------------------------------------------------
#	Directories Labels to be used in makefile
BASE_ROOT := ../../../../../..
OS := $(BASE_ROOT)/FreeRTOS/Source
POSIX := $(BASE_ROOT)/freertos_posix/posix
COMMON := $(BASE_ROOT)/freertos_posix/demo_posix/common
DSPIC := $(BASE_ROOT)/freertos_posix/demo_posix/dspic/dspic33
PIC_H := /usr/pic30-elf/support/h
PIC_LIB := /usr/pic30-elf/lib
ifeq ($(CONFIG_ETHERNET), y)
	UIP := $(BASE_ROOT)/uip
	WEBSERVER := $(BASE_ROOT)/webserver_8bit
endif
APP_PF := .
#----------------------------------------------------------------------------------------

# COMPILER ------------------------------------------------------------------------------
#	Use pic30-elf-gcc compiler by Microchip
CC := pic30-elf-gcc
B2H	:= pic30-elf-bin2hex
#----------------------------------------------------------------------------------------

# COMPILER OPTIONS-----------------------------------------------------------------------
# -mcpu=target			This option selects the particular target. This option affects how 
#						some predifined constants are set (e.g. __dsPIC33FJ128GP306__)
# -mlarge-code			Turn on large code model (code size >32KB)
# -mlarge-data			Turn on large data model (const location >32KB)
# -O0:					no optimisation of code
# -Ox:					optimisation of code Level x, x = 1, 2, 3, s
# -fomit-frame-pointer: Required by FreeRTOS at -O0 (at other -O, this is automatically
#						enabled) to disable instructions to save, set up and restore
#						frame pointers. Refer to FreeRTOS Forum at
#						http://sourceforge.net/forum/message.php?msg_id=3899530
#						and MPLAB C30 C compiler user guide on Frame Pointer
CFLAGS := -mcpu=33FJ128GP306 -mlarge-code -mlarge-data -Os -fomit-frame-pointer -Werror

# HEADER INCLUDES
# -I:					include the directory of header file (*.h)
# -idirafter: 			include the directory of header file (*.h) in secondary path
CFLAGS += -idirafter$(OS)/include/ \
			-idirafter$(PIC_H)/ -I$(POSIX)/../Source/include/ \
			-I$(POSIX)/ -I$(DSPIC)/rackmount/$(PRJ_NAME)/include/

# DEFINE DIRECTIVES
# -Dmacro:				Define macro with the string 1 as its definition.
CFLAGS += -DMPLAB_DSPIC33_PORT
# OS Scheduler
ifeq ($(FREERTOS_SCHED), y)
	CFLAGS += -DFREERTOS_SCHED
	ifeq ($(CRTHREAD_SCHED), y)
		CFLAGS += -DCRTHREAD_SCHED
	endif
endif
# UART_MOD
ifeq ($(CONFIG_UART), y)
	CFLAGS += -DUART_MOD
	ifeq ($(CONFIG_DEBUG), y)
		CFLAGS += -DDEBUG_CONSOLE \
					-I$(COMMON)/include/
	endif
endif
# I2C_MOD
ifeq ($(CONFIG_I2C), y)
	CFLAGS += -DI2C_MOD
endif
# I2C_DAC_MOD
ifeq ($(CONFIG_I2C_DAC), y)
	CFLAGS += -DI2C_DAC_MOD
endif
# NVM_MOD
ifeq ($(CONFIG_NVM), y)
	CFLAGS += -DNVM_MOD
	ifeq ($(CONFIG_NVM_SRC), FLASH)
		CFLAGS += -DNVM_FLASH \
					-I$(DSPIC)/../drivers/include/
	endif
	ifeq ($(CONFIG_NVM_SRC), I2C)
		CFLAGS += -DNVM_I2C
	endif
	ifeq ($(CONFIG_NVM_SRC), ON_CHIP)
		CFLAGS += -DNVM_ON_CHIP
	endif
endif
# ADC_MOD
ifeq ($(CONFIG_ADC), y)
	CFLAGS += -DADC_MOD
endif
# PWM_MOD
ifeq ($(CONFIG_PWM), y)
	CFLAGS += -DPWM_MOD
endif
# KB_MOD
ifeq ($(CONFIG_KEYBOARD), y)
	CFLAGS += -DKB_MOD
endif
# ETHERNET_MOD
ifeq ($(CONFIG_ETHERNET), y)
	CFLAGS += -DETHERNET_MOD \
				-idirafter$(WEBSERVER)/core/ \
				-I$(UIP)/uip/ \
				-I$(UIP)/apps/dhcpc/ \
				-I$(DSPIC)/../drivers/include/ 
endif
#----------------------------------------------------------------------------------------

# LINKER OPTIONS ------------------------------------------------------------------------
# -T:					include the linker script file (*.gld)
# -L:					include the directory of library file (*.a)
# -Xlinker:				pass argument from pic30-elf-gcc (compiler) to pic30-elf-ld (linker)
#						e.g. 	-Map file (create a map file)
#								--print-map (print a map file on screen)
#								--report-mem (print the memory usage summary on screen)
# 								--heap=512 (allocate a heap size of 512bytes)
#								  [for more arguments, refer to: 
#									MPLAB ASM30 MALAB LINK30 and utilities user's guide and 
#									MPLAB C30 C compiler user's guide]
LDFLAGS := -Xlinker -Map="$(APP_PF)/$(PRJ_NAME).map" -Xlinker --heap=4096 \
			-L$(PIC_LIB)/ \
			-T$(DSPIC)/linker/clinkscript_p33FJ128GP306.gld
#----------------------------------------------------------------------------------------

# TARGET FILE ---------------------------------------------------------------------------
TARGET := $(PRJ_NAME).bin
TARGET_HEX := $(PRJ_NAME).hex
#----------------------------------------------------------------------------------------

# OBJECT FILE --------------------------------------------------------------------------
# Compulsory Files:
#   Bootloader:	reset.c
#	FreeRTOS: 	list.c, queue.c, tasks.c, heap_1.c, port.c, boot.c,
#				app_idlehook.c, app_idle.c
#	Driver:		devices.c
#	POSIX:		thread.c
# Special Files:
#	POSIX:		time.c (CAUTION: if this is not included, compiler will use time 
#						function in standard libary)
#	Driver:		traps.c (detect critical hardware/software malfunction of dsPic)
obj-y := \
	$(DSPIC)/linker/reset.o \
	$(OS)/list.o $(OS)/queue.o $(OS)/tasks.o \
	$(OS)/portable/MemMang/heap_1.o \
	$(POSIX)/../Source/portable/MPLAB/dspic33/port.o \
	$(POSIX)/src/pthread.o $(POSIX)/src/time.o \
	$(DSPIC)/boot/boot.o $(DSPIC)/../drivers/devices.o \
	$(DSPIC)/../drivers/traps.o \
	$(COMMON)/app_idlehook.o $(COMMON)/app_idle.o \
	$(COMMON)/app_led.o \
	$(APP_PF)/08_eeprom.o \
	$(APP_PF)/led.o
			
# DRIVERS
obj-$(CONFIG_UART) += $(DSPIC)/../drivers/uart.o $(DSPIC)/../drivers/cirbuf.o
obj-$(CONFIG_DEBUG) += $(COMMON)/tools/console.o
obj-$(CONFIG_I2C) += $(DSPIC)/../drivers/i2c.o 
obj-$(CONFIG_I2C_DAC) += $(DSPIC)/../drivers/i2c_dac.o
obj-$(CONFIG_ADC) += $(DSPIC)/../drivers/adc.o
obj-$(CONFIG_PWM) += $(DSPIC)/../drivers/pwm.o
obj-$(CONFIG_KEYBOARD) += $(DSPIC)/../drivers/gpio.o
# NVM_MOD
ifeq ($(CONFIG_NVM), y)
	ifeq ($(CONFIG_NVM_SRC), FLASH)
		obj-y += $(DSPIC)/../drivers/flash_eeprom.o $(DSPIC)/../drivers/rtsp.o
	endif
	ifeq ($(CONFIG_NVM_SRC), I2C)
		obj-y += $(DSPIC)/../drivers/i2c_eeprom.o
	endif
	ifeq ($(CONFIG_NVM_SRC), ON_CHIP)
		obj-y += $(DSPIC)/../drivers/eeprom.o $(DSPIC)/../drivers/eeprom_asm.o
	endif
endif
# ETHERNET_MOD
obj-$(CONFIG_ETHERNET) += \
		$(DSPIC)/../drivers/dm9000a.o \
		$(DSPIC)/../drivers/clock-arch.o \
		$(UIP)/uip/timer.o \
		$(UIP)/uip/uip.o \
		$(UIP)/uip/uip_arp.o \
		$(UIP)/uip/psock.o \
		$(UIP)/apps/dhcpc/dhcpc.o \
		$(WEBSERVER)/core/httpd.o \
		$(WEBSERVER)/core/httpd-fs.o \
#------------------------------------------------------------------------------

# Compile and linking =========================================================

# STEP 3 ----------------------------------------------------------------------
# This first explicit rule in the file, which is the rule that make
# will attempt to build when you type "make".  The final *.hex is converted 
# by $(B2H) from $(TARGET). Before make can do "nothing", it must build $(TARGET).
all: $(TARGET)
	@echo 'Building target: $(TARGET_HEX)'
	$(B2H) $^ $(TARGET_HEX)
	@echo ' '
	
# STEP 2 ----------------------------------------------------------------------
# An explicit rule tells make how to build "$(TARGET)". Prerequisites
# are required (so if they are not up-to-date, make will use the implicit
# rule (i.e. STEP1) to build them before attempting to build $(TARGET)).
# The $^ is a all prerequisites. If you add more .c files to this 
# project, usually you can just add their .o name to the end of 
# the dependancy list, i.e. $(obj-y) in parameters
# -o is an option to specific the output file name, in this case $(TARGET)
$(TARGET): $(obj-y)
	@echo 'Building target: $@'
	$(CC) $(LDFLAGS) $^ -lc-elf -o $@
	@echo ' '
	
# STEP 1 ----------------------------------------------------------------------
# This "implicit" rule tells make that any file ending in .o is
# build from two files with the same base name ending in .c and .h,
# and that the command followed by the two .c and .h files is the
# way to obtain the .o file.  Whenever any of the rules below
# specifies that a .o file is required, make will use this rule
# (in the absence of an explicit rule for that file) to build the
# .o file.
# -o is an option to specific the output file name, in this case %.o
# -c is an option to compile but not link
%.o : %.c
	@echo 'Building target: $@'
	$(CC) $(CFLAGS) -c $< -o $@
	@echo ' '
	
%.o : %.s
	@echo 'Building target: $@'
	$(CC) $(CFLAGS) -c $< -o $@
	@echo ' '

# configuration ---------------------------------------------------------------
#	setup symbolic link <asm-dsPic/...> to setup <asm/...>
config:
	@if [ ! -d "$(POSIX)/asm" ] ; then ln -s asm-dsPic/ $(POSIX)/asm; fi

# This final rule is what's called a "phony rule" or "phony target",
# because it isn't used in ary part of the normal build process.
# By typing "make clean", make will attempt to build this rule and
# these commands will delete all of the compiler generated files.
clean:
	find $(BASE_ROOT)/ \( -name '*.bin' -o -name '*.hex' -o -name '*.map' -o -name '*.o' \) -print -exec rm -f '{}' ';'

# Clean platform depended files, after thet user must do make config again
mrproper: clean
	rm -f $(POSIX)/asm
