#
# Original ucliblike8
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
CFLAGS = -Wall -g -DTEST
COMPILE_DIR = main_compile_dir

#
# compiling
#
init_compile : 
	mkdir -p $(COMPILE_DIR)

#
#       libc/stdio/printf
printf.o: libc/stdio/printf.c init_compile
	cp $< $(COMPILE_DIR)/
	cp include/printf.h $(COMPILE_DIR)/
	gcc $(CFLAGS) -DTEST_PRINT_INTEGER $(COMPILE_DIR)/printf.c -o $(COMPILE_DIR)/$@

#
#       utils/cirbuf
cirbuf.o: utils/cirbuf.c init_compile
	cp $< $(COMPILE_DIR)/
	gcc $(CFLAGS) -DTEST $(COMPILE_DIR)/cirbuf.c -o $(COMPILE_DIR)/$@

#
# cleaning
#
clean:
	find . \( -name '*.o' \) -print | xargs rm -f
	rm -rf $(COMPILE_DIR)
