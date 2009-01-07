# This Makefile ==============================================================
#	make			-- compile all
#						Steps:
#						1) Compile $(OBJECTS) with no linking (.c -> .o)
#						2) Link all *.o to $(TARGET)
#	make clean		-- clear all objects, bin, hex, out and map files
#=============================================================================

# Project name ===============================================================
PRJ_NAME := 03_mutex

# Directories ----------------------------------------------------------------
BASE_ROOT := ../../../../..
LINUX := $(BASE_ROOT)/freertos_posix/demo_posix/linux
COMMON := $(BASE_ROOT)/freertos_posix/demo_posix/common
APP_PF := .

# Console Commands ------------------------------------------------------------
CC := gcc

# Flags options ---------------------------------------------------------------
CFLAG := -I$(APP_PF)/include/

# Target file -----------------------------------------------------------------
TARGET := $(PRJ_NAME).out

# Object files ----------------------------------------------------------------
OBJECTS := 	$(LINUX)/boot/boot.o \
			$(APP_PF)/led.o $(APP_PF)/output.o \
			$(COMMON)/app_mutex.o \
			$(COMMON)/03_mutex.o

#==============================================================================

# Compile and linking =========================================================

# STEP 3 ----------------------------------------------------------------------
# This first explicit rule in the file, which is the rule that make
# will attempt to build when you type "make". 
all: $(TARGET)
	
# STEP 2 ----------------------------------------------------------------------
# An explicit rule tells make how to build "$(TARGT)".
$(TARGET): $(OBJECTS)
	@echo 'Building target: $@'
	@echo 'Invoking: gcc'
	$(CC) $(CFLAG) $^ -lpthread -o $@
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
	@echo 'Invoking: gcc'
	$(CC) $(CFLAG) -c $< -o $@
	@echo 'Finished building target: $@'
	@echo ' '

# This final rule is what's called a "phony rule" or "phony target",
# because it isn't used in ary part of the normal build process.
# By typing "make clean", make will attempt to build this rule and
# these commands will delete all of the compiler generated files.
clean:
	find $(BASE_ROOT)/ \( -name '*.bin' -o -name '*.hex' -o -name '*.map' -o -name '*.o' -o -name '*.out' \) -print -exec rm -f '{}' ';'
