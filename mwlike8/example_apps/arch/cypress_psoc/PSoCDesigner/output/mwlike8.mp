
Area                               Addr   Size   Decimal Bytes (Attributes)
--------------------------------   ----   ----   ------- ----- ------------
                             TOP   0000   00D0 =    208. bytes (abs,ovr,rom)

       Addr  Global Symbol
      -----  --------------------------------
       0068  __Start
       00A9  __Exit

Area                               Addr   Size   Decimal Bytes (Attributes)
--------------------------------   ----   ----   ------- ----- ------------
                             lit   0100   07D9 =   2009. bytes (rel,con,rom)

       Addr  Global Symbol
      -----  --------------------------------
       0100  __lit_start
       0100  _AmonicsLogo
       0537  _Font_Table
       079F  _Font_SPACE
       07A7  _Font_DEGREE
       07AF  _Font_EXC
       07B7  _ClearPaper
       07B8  LoadConfigTBL_mwlike8_Ordered
       0867  LoadConfigTBL_mwlike8_Bank0
       08A4  LoadConfigTBL_mwlike8_Bank1
       08D9  __lit_end

Area                               Addr   Size   Decimal Bytes (Attributes)
--------------------------------   ----   ----   ------- ----- ------------
                        func_lit   08D9   0002 =      2. bytes (rel,con,rom)

       Addr  Global Symbol
      -----  --------------------------------
       08D9  __func_lit_start
       08DB  __func_lit_end

Area                               Addr   Size   Decimal Bytes (Attributes)
--------------------------------   ----   ----   ------- ----- ------------
                     psoc_config   08DB   006B =    107. bytes (rel,con,rom)

       Addr  Global Symbol
      -----  --------------------------------
       08DB  LoadConfigInit
       08DB  _LoadConfigInit
       08DB  __psoc_config_start
       08E2  LoadConfig_mwlike8
       08E2  _LoadConfig_mwlike8
       0946  __psoc_config_end

Area                               Addr   Size   Decimal Bytes (Attributes)
--------------------------------   ----   ----   ------- ----- ------------
                     UserModules   0946   006D =    109. bytes (rel,con,rom)

       Addr  Global Symbol
      -----  --------------------------------
       0946  _UART_1_EnableInt
       0946  __usermodules_start
       0946  UART_1_EnableInt
       094D  _UART_1_DisableInt
       094D  UART_1_DisableInt
       0954  _UART_1_Start
       0954  UART_1_Start
       095B  _UART_1_Stop
       095B  UART_1_Stop
       0962  _UART_1_SendData
       0962  UART_1_SendData
       0965  UART_1_bReadTxStatus
       0965  bUART_1_ReadTxStatus
       0965  _UART_1_bReadTxStatus
       0965  _bUART_1_ReadTxStatus
       0968  _bUART_1_ReadRxData
       0968  bUART_1_ReadRxData
       0968  UART_1_bReadRxData
       0968  _UART_1_bReadRxData
       096B  _bUART_1_ReadRxStatus
       096B  _UART_1_bReadRxStatus
       096B  UART_1_bReadRxStatus
       096B  bUART_1_ReadRxStatus
       096E  _Timer8_1_EnableInt
       096E  Timer8_1_EnableInt
       0972  _Timer8_1_DisableInt
       0972  Timer8_1_DisableInt
       0976  _Timer8_1_Start
       0976  Timer8_1_Start
       097A  Timer8_1_Stop
       097A  _Timer8_1_Stop
       097E  Timer8_1_WritePeriod
       097E  _Timer8_1_WritePeriod
       0981  _Timer8_1_WriteCompareValue
       0981  Timer8_1_WriteCompareValue
       0984  bTimer8_1_ReadCompareValue
       0984  _bTimer8_1_ReadCompareValue
       0984  Timer8_1_bReadCompareValue
       0984  _Timer8_1_bReadCompareValue
       0987  _bTimer8_1_ReadCounter
       0987  bTimer8_1_ReadTimerSaveCV
       0987  Timer8_1_bReadTimerSaveCV
       0987  _Timer8_1_bReadTimerSaveCV
       0987  _bTimer8_1_ReadTimerSaveCV
       0987  bTimer8_1_ReadCounter
       09AE  Timer8_1_bReadTimer
       09AE  bTimer8_1_ReadTimer
       09AE  _Timer8_1_bReadTimer
       09AE  _bTimer8_1_CaptureCounter
       09AE  _bTimer8_1_ReadTimer
       09AE  bTimer8_1_CaptureCounter
       09B3  __UserModules_end

Area                               Addr   Size   Decimal Bytes (Attributes)
--------------------------------   ----   ----   ------- ----- ------------
                            text   09B3   1954 =   6484. bytes (rel,con,rom)

       Addr  Global Symbol
      -----  --------------------------------
       09B3  _pre_wr_cir254buf
       09B3  __text_start
       09D6  _pre_rd_cir254buf
       09F9  _do_fork
       0AA9  _gpio_open
       0ABC  _gpio_isr
       0B69  _gpio_softirq
       0C8F  _main
       0C93  _start_kernel
       0CA7  _kernel_thread
       0CB9  _mwlike8_process1
       0F98  _format_manipulate
       1034  _print_i
       1477  _print_integer
       149F  _sched_init
       14BB  _schedule
       1538  _schedule_timeout
       1597  _serial_open
       15AE  _serial_tx_isr
       1679  _serial_rx_isr
       173A  _write
       17D9  _read
       1842  _serial_write_complete
       190E  _setup_arch
       1915  _softirq_init
       1916  _do_softirq
       1943  _io_open
       194D  _GrGetNextEventTimeout
       1B46  _GrText
       1BAF  _mwlike8_server
       1BDC  _time_init
       1BE5  _timer_interrupt
       1CE4  _do_timer
       1CEE  _timer_softirq
       1EA9  _add_timer
       1F4C  __malarm
       1F64  _scr_open
       1F96  _wrTxLm6023
       1FD8  _GrBitmapDrv
       2087  _GrTextDrv
       2199  __plcall
       21A9  __divmod_16X16_16
       225E  __divmodu_16X16_16
       2307  __text_end

Area                               Addr   Size   Decimal Bytes (Attributes)
--------------------------------   ----   ----   ------- ----- ------------
               virtual_registers   0000   000F =     15. bytes (rel,con,ram)

       Addr  Global Symbol
      -----  --------------------------------
       0000  NO_SHADOW
       0000  _NO_SHADOW
       0000  __rZ
       0001  __rY
       0002  __rX
       0003  __r11
       0004  __r10
       0005  __r9
       0006  __r8
       0007  __r7
       0008  __r6
       0009  __r5
       000A  __r4
       000B  __r3
       000C  __r2
       000D  __r1
       000E  __r0
       000F  __virtual_registers_end

Area                               Addr   Size   Decimal Bytes (Attributes)
--------------------------------   ----   ----   ------- ----- ------------
                             bss   000F   0028 =     40. bytes (rel,con,ram)

       Addr  Global Symbol
      -----  --------------------------------
       000F  _gpio_var
       000F  __bss_start
       0010  _prev
       0012  _current_ptr
       0013  _task
       0017  _next_ptr
       0018  _wr_compl_var
       0019  _serial_tx
       001A  _serial_tx_buf
       001D  _serial_rx
       001E  _serial_rx_buf
       0027  _softirq_vec
       0028  _irq_stat
       002C  _io_bit_ctrl
       002D  _ptr_events_vect
       002E  _events_vect
       0030  _timer_vector
       0036  _jiffies
       0037  __bss_end

Area                               Addr   Size   Decimal Bytes (Attributes)
--------------------------------   ----   ----   ------- ----- ------------
                      Stack_Zone   0037   0010 =     16. bytes (rel,con,ram)

       Addr  Global Symbol
      -----  --------------------------------
       0037  _stack_start
       0047  __Stack_Zone_end

Files Linked      [ module(s) ]

./obj/boot.o	[ boot.asm ]
./obj/cirbuf.o	[ cirbuf.c ]
./obj/fork.o	[ fork.c ]
./obj/gpio.o	[ gpio.c ]
./obj/main.o	[ main.c ]
./obj/maink.o	[ maink.c ]
./obj/mw_pro1.o	[ mw_pro1.c ]
./obj/printf.o	[ printf.c ]
./obj/sched.o	[ sched.c ]
./obj/serial.o	[ serial.c ]
./obj/setup.o	[ setup.c ]
./obj/softirq.o	[ softirq.c ]
./obj/srvfunc.o	[ srvfunc.c ]
./obj/srvmain.o	[ srvmain.c ]
./obj/time.o	[ time.c ]
./obj/timer.o	[ timer.c ]
./obj/twlm6023.o	[ twlm6023.c ]
C:\PROGRA~1\CYPRES~2\PSOCDE~1/tools/stackzone.o	[ stackzone.asm ]
<library>	[ uart_1.asm, timer8_1.asm, psocconfigtbl.asm, psocconfig.asm, rZ.asm, rY.asm, rX.asm, r11.asm, r10.asm ]
[ r9.asm, r8.asm, r7.asm, r6.asm, r5.asm, r4.asm, r3.asm, r2.asm, r1.asm, r0.asm, plcall.s, divmod16.s, divmodu16.s ]


User Base Address Definitions

lit = 0x100
data = 0x0

