ReadMe.txt of ucliblike8(uClibc-like in 8-bit mcu) 

Contents: (arithmetical order)
* API (uClibc-like)
* Contact
* Description 
* ToDo
* Version Number

========================================================================================================
Version Number - 0.01.00 (it is testing version, 0.02.XX will be stable version)
        0.01.00 : 
                * printf - print_float()
                * printf - print_integer()
                * created following uClibc

========================================================================================================
Description 
* using uClibc in 8-bit system, but all these code should be platform independant
* due to reduce code size in 8 bit system
        * application just add some source files in its project
        * each source C file has some define for each small modules, if needed, then enable it/them

========================================================================================================
API (uClibc-like)
* print_integer(char* out, const char* flag_ptr, int i)
        * out as string for output
        * flag_ptr pointer points to something like that "05", same to printf control flag, just remove the "%d"
        * i as the integer to be convert to string format

========================================================================================================
Contact
* yan - yan@amonics.com

========================================================================================================
ToDo
* print_float <wait to test> 
* gun C library - http://www.gnu.org/software/libc/libc.html
* adding header file -- stdio.h
* circular buffer
        * wr : update ptr, then wr.
        * get content, then update pttr

