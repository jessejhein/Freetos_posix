#
# for new application, you need to change the 
#       * OS_DRIVER_DIR 
#       * APP_DIR 
#       * object files in 
#               * APP_FILES
#               * Application Targets for each processes
#               * processes name in each Targets
#
# Remarks : you can remove any testing stuffs
#

VERSION = 0
PATCHLEVEL = 1
SUBLEVEL = 0
EXTRAVERSION = 
                                                                                                                                               
KERNELRELEASE=$(VERSION).$(PATCHLEVEL).$(SUBLEVEL)$(EXTRAVERSION)

#
# standard CFLAGS
#
CFLAGS = -Wall -g
COMPILE_DIR = main_compile_dir
OS_DRIVER_DIR = ../../../../../os/linlike8/arch/i386_linux
GUI_CORE_DIR = ../../../nanox
APP_DIR = ../..

APP_FILES = srvmain.o mw_pro1.o boot.o

CLEAN_FILES = vmlinlike8

#
# compiling
#
all: init_compile $(APP_FILES)
	@$(MAKE) vmlinlike8

init_compile : 
	mkdir -p $(COMPILE_DIR)

#
#       OS Drivers
boot.o: $(OS_DRIVER_DIR)/boot/boot.c
	cp $< $(COMPILE_DIR)/
	gcc $(CFLAGS) -c $(COMPILE_DIR)/boot.c -o $@

#
#       GUI CORE
srvmain.o: $(GUI_CORE_DIR)/srvmain.c
	cp $< $(COMPILE_DIR)/
	cp $(APP_DIR)/app.h $(COMPILE_DIR)/
	gcc $(CFLAGS) -c $(COMPILE_DIR)/srvmain.c -o $@

#
#       Application
mw_pro1.o: $(APP_DIR)/mw_pro1.c $(APP_DIR)/app.h
	cp $< $(COMPILE_DIR)/
	gcc $(CFLAGS) -c $(COMPILE_DIR)/mw_pro1.c -o $@

#
# linking
#
vmlinlike8: $(APP_FILES)
	gcc $(APP_FILES) -o $(COMPILE_DIR)/$@ -lnano-X

#
# cleaning
#
clean:
	find ../../ \( -name '*.o' \) -print | xargs rm -f
	rm -rf $(COMPILE_DIR)
	rm -f $(CLEAN_FILES)
