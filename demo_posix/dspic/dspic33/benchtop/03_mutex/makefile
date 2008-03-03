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
# The official linker script, found under /usr/pic30-elf/support/gld, is not used. A 
# customized version, is included in this project to ensure that the _resetPRI function 
# is located at address the proper address so that the bootloader can always point this 
# address. Please also include reset.s in the compilation list to achieve the desired 
# function.
#========================================================================================

# PROJECT NAME --------------------------------------------------------------------------
#	Target file (*.hex) will have the same name as the project name
PRJ_NAME := benchtop
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
ifeq ($(DSPIC_SERIES), DSPIC33)
	DSPIC := $(BASE_ROOT)/freertos_posix/demo_posix/dspic/dspic33
endif
ifeq ($(DSPIC_SERIES), DSPIC30)
	DSPIC := $(BASE_ROOT)/freertos_posix/demo_posix/dspic/dspic30
endif
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
ifeq ($(DSPIC_SERIES), DSPIC33)
	ifeq ($(DSPIC_MODEL), FJ256GP506)
		CFLAGS += -mcpu=33FJ256GP506
	endif
	ifeq ($(DSPIC_MODEL), FJ128GP306)
		CFLAGS += -mcpu=33FJ128GP306
	endif
endif
ifeq ($(DSPIC_SERIES), DSPIC30)
	ifeq ($(DSPIC_MODEL), F5011)
		CFLAGS := -mcpu=30F5011
	endif
endif
CFLAGS += -mlarge-code -mlarge-data -Os -fomit-frame-pointer -Werror

# HEADER INCLUDES
# -I:					include the directory of header file (*.h)
# -idirafter: 			include the directory of header file (*.h) in secondary path
CFLAGS += -idirafter$(OS)/include/ \
			-idirafter$(PIC_H)/ -I$(POSIX)/../Source/include/  \
			-I$(COMMON)/tools/ \
			-I$(POSIX)/ \
			-I$(APP_PF)/include/

# DEFINE DIRECTIVES
# -Dmacro:				Define macro with the string 1 as its definition.
# -Dmacro=defn:			Define macro as defn
ifeq ($(DSPIC_SERIES), DSPIC33)
	CFLAGS += -DMPLAB_DSPIC33_PORT
	ifeq ($(DSPIC_MODEL), FJ256GP506)
		CFLAGS += -DFJ256GP506
	endif
	ifeq ($(DSPIC_MODEL), FJ128GP306)
		CFLAGS += -DFJ128GP306
	endif
endif
ifeq ($(DSPIC_SERIES), DSPIC30)
	CFLAGS += -DMPLAB_DSPIC30_PORT
	ifeq ($(DSPIC_MODEL), F5011)
		CFLAGS += -DF5011
	endif
endif
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
	ifdef CONFIG_UART_BAUDRATE
		CFLAGS += -DUART_BAUDRATE=$(CONFIG_UART_BAUDRATE)
	endif
	ifeq ($(CONFIG_UART_TOTAL_CHANNEL), 2)
		CFLAGS += -DNO_OF_UART=2
		ifeq ($(CONFIG_UART_PRIMARY), RS485)
			CFLAGS += -DUARTA_RS485=1
		else
			CFLAGS += -DUARTA_RS485=0
		endif
		ifeq ($(CONFIG_UART_SECONDARY), RS485)
			CFLAGS += -DUARTB_RS485=1
		else
			CFLAGS += -DUARTB_RS485=0
		endif
	else
		CFLAGS += -DNO_OF_UART=1
		ifeq ($(CONFIG_UART_PRIMARY), RS485)
			CFLAGS += -DUARTA_RS485=1
		else
			CFLAGS += -DUARTA_RS485=0
		endif
	endif
	ifdef CONFIG_UART_BUFFER_SIZE
		CFLAGS += -DMAX_UART_RX_BUF=$(CONFIG_UART_BUFFER_SIZE) \
					-DMAX_UART_TX_BUF=$(CONFIG_UART_BUFFER_SIZE)
	endif
	ifeq ($(CONFIG_UART_BL_RESET), y)
		CFLAGS += -DBOOTLOADER_RESET
	endif
endif
# I2C_MOD
ifeq ($(CONFIG_I2C), y)
	CFLAGS += -DI2C_MOD
	ifdef CONFIG_I2C_BAUDRATE
		CFLAGS += -DI2C_BAUDRATE=$(CONFIG_I2C_BAUDRATE)
	endif
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
	ifdef CONFIG_ADC_SAMPLE_RATE
		CFLAGS += -DADC_SAMP_RATE=$(CONFIG_ADC_SAMPLE_RATE)
	endif
	ifdef CONFIG_ADC_ACQ_TIME
		CFLAGS += -DADC_ACQ_TIME=$(CONFIG_ADC_ACQ_TIME)
	endif
endif
# PWM_MOD
ifeq ($(CONFIG_PWM), y)
	CFLAGS += -DPWM_MOD
endif
# KB_MOD
ifeq ($(CONFIG_KEYBOARD), y)
	CFLAGS += -DKB_MOD
	ifdef CONFIG_KB_BUFFER_SIZE
		CFLAGS += -DMAX_GPIO_BUF=$(CONFIG_KB_BUFFER_SIZE)
	endif
endif
# ETHERNET_MOD
ifeq ($(CONFIG_ETHERNET), y)
	CFLAGS += -DETHERNET_MOD \
				-idirafter$(WEBSERVER)/core/ \
				-I$(UIP)/uip/ \
				-I$(UIP)/apps/dhcpc/ \
				-I$(DSPIC)/../drivers/include/ 
	ifdef CONFIG_MAC_ADDRESS_BTYE1
		CFLAGS += -DDEFAULT_NIC_BYTE1=$(CONFIG_MAC_ADDRESS_BTYE1)
	endif
	ifdef CONFIG_MAC_ADDRESS_BTYE2
		CFLAGS += -DDEFAULT_NIC_BYTE2=$(CONFIG_MAC_ADDRESS_BTYE2)
	endif
	ifdef CONFIG_MAC_ADDRESS_BTYE3
		CFLAGS += -DDEFAULT_NIC_BYTE3=$(CONFIG_MAC_ADDRESS_BTYE3)
	endif
	ifeq ($(CONFIG_WEBSERVER_LOGIN), y)
		CFLAGS += -DETH_WEB_LOGIN_ENABLE
		ifdef CONFIG_WEBSERVER_USERNAME
			CFLAGS += -DETH_WEB_USERNAME=$(CONFIG_WEBSERVER_USERNAME)
		endif
		ifdef CONFIG_WEBSERVER_PASSWORD
			CFLAGS += -DETH_WEB_PASSWORD=$(CONFIG_WEBSERVER_PASSWORD)
		endif
	endif
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
LDFLAGS := -Xlinker -Map="$(APP_PF)/$(PRJ_NAME).map" -L$(PIC_LIB)/ -Xlinker --heap=$(HEAP_SIZE)
ifeq ($(DSPIC_SERIES), DSPIC33)
	ifeq ($(DSPIC_MODEL), FJ256GP506)
		LDFLAGS += -T$(DSPIC)/linker/clinkscript_p33FJ256GP506.gld
	endif
	ifeq ($(DSPIC_MODEL), FJ128GP306)
		LDFLAGS += -T$(DSPIC)/linker/clinkscript_p33FJ128GP306.gld
	endif
endif
ifeq ($(DSPIC_SERIES), DSPIC30)
	ifeq ($(DSPIC_MODEL), F5011)
		LDFLAGS += -T$(DSPIC)/linker/clinkscript5011.gld
	endif
endif
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
	$(DSPIC)/boot/boot.o $(DSPIC)/../drivers/devices.o \
	$(DSPIC)/../drivers/traps.o \
	$(OS)/list.o $(OS)/queue.o $(OS)/tasks.o \
	$(OS)/portable/MemMang/heap_3.o \
	$(COMMON)/app_idlehook.o \
	$(POSIX)/src/pthread.o $(POSIX)/src/time.o \
	$(COMMON)/example/thread/mutex.o \
	$(COMMON)/example/thread/idle.o \
	$(COMMON)/example/03_mutex/screen.o \
	$(COMMON)/example/03_mutex/pf_main.o \
	$(APP_PF)/led.o

# OS MEMORY
ifeq ($(DSPIC_SERIES), DSPIC33)
	obj-y += $(POSIX)/../Source/portable/MPLAB/dspic33/port.o
endif
ifeq ($(DSPIC_SERIES), DSPIC30)
	obj-y += $(POSIX)/../Source/portable/MPLAB/dspic30/port.o
endif

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
	find $(BASE_ROOT)/ \( \( -iwholename '*FreeRTOS/Demo*' -prune \) -type f \) -o \( -name '*.bin' -o -name '*.hex' -o -name '*.map' -o -name '*.o' \) -print -exec rm -f '{}' ';'

# Clean platform depended files, after thet user must do make config again
mrproper: clean
	rm -f $(POSIX)/asm
