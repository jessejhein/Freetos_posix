#
# Makefile template for At89c52 template driver coding
# Memory usage 
#	data, stack, idata 
#	data => 0 size, 
#		register bank 0 => normal program 
#		register bank 1 => ISR 
#		... 
#		bit memory 
#		direct memory as 0 size 
#	stack => more space, for system usage  
#	idata => program data for user application 
#	so all user data in program should add idata 
# c compiler : sdcc 
# MCU ; 8052 
# version : 2.00
#

# variables
#***********************************************
	# option of c compile *********************
	# 	--model-small => one chip only, no external memory 
	#	--stack-after-data => stack after data memory 
	#	--data-loc 0x30 => start of data location
	#	--iram-size 0x100 => check the size of internal RAM, also for 8052(0x100)
compilerOption = --model-small --stack-after-data --data-loc 0x30 --iram-size 0x100
	#	--idata-loc => indirect addr. inetrnal RAM, 8052 
compilerOption8052 = --idata-loc 0x80
	# choose c compiler ***********************
cCompiler = sdcc $(compilerOption) $(compilerOption8052) 
	# syntax for only compile *****************
compile = $(cCompiler) -c
	# header files need to compile 
headerFiles = ctrller.h uart.h timer.h packet.h commCmd.h applMain.h main.h
objectFiles = main.rel applMain.rel commCmd.rel packet.rel timer.rel uart.rel DevKit0_8.rel At89c52.rel 

# compile process
#***********************************************
hex : $(objectFiles)
	$(cCompiler) $(objectFiles) 

main.rel : main.c $(headerFiles)
	  $(compile) main.c 

applMain.rel : applMain.c $(headerFiles) 
	$(compile) applMain.c

commCmd.rel : commCmd.c $(headerFiles) 
	$(compile) commCmd.c

packet.rel : packet.c $(headerFiles) 
	$(compile) packet.c

timer.rel : timer.c $(headerFiles)
	$(compile) timer.c

uart.rel : uart.c $(headerFiles)
	$(compile) uart.c

DevKit0_8.rel : DevKit0_8.c $(headerFiles) 
	$(compile) DevKit0_8.c

At89c52.rel : At89c52.c $(headerFiles) 
	$(compile) At89c52.c

clean :
	rm $(objectFiles)
