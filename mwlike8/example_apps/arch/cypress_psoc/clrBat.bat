:: Start 
::      mwlike8 Original for Cypress PSoC Arch
:: ================================================================================================
CLS

:: configuration of this batch file
:: ================================================================================================
set COMPILE_DIR=PSoCDesigner

@REM os(linlike8) drivers
@REM ================================================================================================
del %COMPILE_DIR%\main.c
del %COMPILE_DIR%\setup.c
del %COMPILE_DIR%\time.c
del %COMPILE_DIR%\sched.h
del %COMPILE_DIR%\current.h
del %COMPILE_DIR%\system.h
del %COMPILE_DIR%\hardirq.h
del %COMPILE_DIR%\softirq.h
del %COMPILE_DIR%\gpio.c
del %COMPILE_DIR%\gpio.h
del %COMPILE_DIR%\serial.c
del %COMPILE_DIR%\serial.h
del %COMPILE_DIR%\i2c.c
del %COMPILE_DIR%\i2c.h

@REM os(linlike8) core
@REM ================================================================================================
del %COMPILE_DIR%\maink.c
del %COMPILE_DIR%\sched.c
del %COMPILE_DIR%\fork.c
del %COMPILE_DIR%\timer.c
del %COMPILE_DIR%\time.h
del %COMPILE_DIR%\timer.h
del %COMPILE_DIR%\softirq.c
del %COMPILE_DIR%\interrupt.h
del %COMPILE_DIR%\irq_cpustat.h
del %COMPILE_DIR%\cirbuf.c
del %COMPILE_DIR%\cirbuf.h
del %COMPILE_DIR%\updownptr.c
del %COMPILE_DIR%\updownptr.h

@REM library(ucliblike8)
@REM ================================================================================================
del %COMPILE_DIR%\printf.c
del %COMPILE_DIR%\printf.h

@REM gui(mwlike8) drivers
@REM ================================================================================================
del %COMPILE_DIR%\device.h
del %COMPILE_DIR%\twlm6023.c
del %COMPILE_DIR%\twlm6023.h
del %COMPILE_DIR%\psoc_gpio_bit.h

@REM gui(mwlike8) core
@REM ================================================================================================
del %COMPILE_DIR%\srvmain.c
del %COMPILE_DIR%\srvfunc.c
del %COMPILE_DIR%\nano-X.h
del %COMPILE_DIR%\mwtypes.h

@REM module(s)
@REM ================================================================================================
:       keyboard
del %COMPILE_DIR%\kb.h
del %COMPILE_DIR%\gpio_kb_app.h

@REM app.
@REM ================================================================================================
del %COMPILE_DIR%\app.h
del %COMPILE_DIR%\mw_pro1.c
