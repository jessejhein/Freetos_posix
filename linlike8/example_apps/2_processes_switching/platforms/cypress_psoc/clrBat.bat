:: Start 
::      same to cpBat
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

@REM module(s)
@REM ================================================================================================
:       keyboard
del %COMPILE_DIR%\kb.h
del %COMPILE_DIR%\gpio_kb_app.h

@REM app.
@REM ================================================================================================
del %COMPILE_DIR%\app.h
del %COMPILE_DIR%\process0.c
del %COMPILE_DIR%\process1.c
