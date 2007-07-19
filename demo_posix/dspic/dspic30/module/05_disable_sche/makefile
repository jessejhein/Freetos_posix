# This Makefile ==============================================================
#	make			-- compile all
#						Steps:
#						1) Compile $(OBJECTS) with no linking (.c -> .o)
#						2) Link all *.o to $(TARGET_BIN)
#						3) Convert $(TARGET_BIN) to $(TARGET_HEX)
#	make clean		-- clear all objects, bin, hex and map files
#=============================================================================

# Note on Linker script ======================================================
# The official linker script for dsPic30f5011 is p30f5011.gld, found under
# /usr/pic30-elf/support/gld. A customized version, clinkscript5011.gld is
# included in this project to ensure that the _resetPRI function is located
# at address 0x100 so that the bootloader can always point this address.
# Please also include reset.s in the compilation list to achieve the desired 
# function.
#=============================================================================

# Project name ===============================================================
PRJ_NAME := 05_disable_sche

# Directories ----------------------------------------------------------------
BASE_ROOT := ../../../../../..
OS := $(BASE_ROOT)/FreeRTOS/Source
POSIX := $(BASE_ROOT)/freertos_posix/posix
COMMON := $(BASE_ROOT)/freertos_posix/demo_posix/common
DSPIC := $(BASE_ROOT)/freertos_posix/demo_posix/dspic/dspic30
PIC_H := /usr/pic30-elf/support/h
PIC_LIB := /usr/pic30-elf/lib
APP_PF := .

# Console Commands ------------------------------------------------------------
CC := pic30-elf-gcc
B2H	:= pic30-elf-bin2hex

# Flags options ---------------------------------------------------------------
# -Dmarcro:				Define macro with the string 1 as its definition.
# 						MPLAB_DSPIC30_PORT is equired by FreeRTOS for dsPic30 series uP
# -mcpu=target			This option selects the particular target. This option affects how 
#						some predifined constants are set (e.g. __dsPIC30F5011__)
# -O0:					no optimisation of code
# -Ox:					optimisation of code Level x, x = 1, 2, 3, s
# -fomit-frame-pointer: Required by FreeRTOS at -O0 (at other -O, this is automatically
#						enabled) to disable instructions to save, set up and restore
#						frame pointers. Refer to FreeRTOS Forum at
#						http://sourceforge.net/forum/message.php?msg_id=3899530
#						and MPLAB C30 C compiler user guide on Frame Pointer
# -I:					include the directory of header file (*.h)
# -idirafter: 			include the directory of header file (*.h) in secondary path
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
CFLAG1 := -DMPLAB_DSPIC30_PORT -mcpu=30F5011 -Os -fomit-frame-pointer \
			-idirafter$(OS)/include/ -I$(PIC_H)/ -I$(POSIX)/../Source/include/ \
			-I$(POSIX)/ -I$(DSPIC)/module/$(PRJ_NAME)/include/ 
CFLAG2 := -DMPLAB_DSPIC30_PORT -mcpu=30F5011 -Os -fomit-frame-pointer \
			-Xlinker -Map="$(APP_PF)/$(PRJ_NAME).map" \
			-L$(PIC_LIB)/ \
			-T$(DSPIC)/linker/clinkscript5011.gld

# Target file -----------------------------------------------------------------
TARGET := $(PRJ_NAME).bin
TARGET_HEX := $(PRJ_NAME).hex

# Object files ----------------------------------------------------------------
OBJECTS := $(DSPIC)/linker/reset.o \
			$(DSPIC)/boot/boot.o $(DSPIC)/../drivers/devices.o \
			$(DSPIC)/../drivers/uart.o $(DSPIC)/../drivers/cirbuf.o \
			$(DSPIC)/../drivers/adc.o $(DSPIC)/../drivers/pwm.o \
			$(DSPIC)/../drivers/traps.o \
			$(POSIX)/src/time.o \
			$(COMMON)/app_led.o \
			$(COMMON)/app_adc_eeprom.o $(COMMON)/app_pwm.o \
			$(APP_PF)/05_disable_sche.o \
			$(APP_PF)/led.o

#==============================================================================

# Compile and linking =========================================================

# STEP 3 ----------------------------------------------------------------------
# This first explicit rule in the file, which is the rule that make
# will attempt to build when you type "make".  The final *.hex is converted 
# by $(B2H) from $(TARGT_BIN). Before make can do "nothing", it must build $(TARGT_BIN).
all: $(TARGET)
	@echo 'Building target: $(TARGET_HEX)'
	@echo 'Invoking: pic30-elf-bin2hex'
	$(B2H) $^ $(TARGET_HEX)
	@echo 'Finished building target: $(TARGET_HEX)'
	@echo ' '
	
# STEP 2 ----------------------------------------------------------------------
# An explicit rule tells make how to build "$(TARGT_BIN)". Prerequisites
# are required (so if they are not up-to-date, make will use the implicit
# rule (i.e. STEP1) to build them before attempting to build $(TARGT_BIN)).
# The $^ is a all prerequisites. If you add more .c files to this 
# project, usually you can just add their .o name to the end of 
# the dependancy list, i.e. $(OBJECTS) in parameters
# -o is an option to specific the output file name, in this case $(TARGET_BIN)
$(TARGET): $(OBJECTS)
	@echo 'Building target: $@'
	@echo 'Invoking: pic30-elf-gcc'
	$(CC) $(CFLAG2) $^ -lc-elf -o $@
	@echo 'Finished building target: $@'
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
	@echo 'Invoking: pic30-elf-gcc'
	$(CC) $(CFLAG1) -c $< -o $@
	@echo 'Finished building target: $@'
	@echo ' '
	
%.o : %.s
	@echo 'Building target: $@'
	@echo 'Invoking: pic30-elf-gcc'
	$(CC) $(CFLAG1) -c $< -o $@
	@echo 'Finished building target: $@'
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
