:: Start 
::      same to cpBat
:: ================================================================================================

:: configuration of this batch file
:: ================================================================================================
set COMPILE_DIR=PSoCDesigner
set COMPILE_SYS_DIR="C:\Program Files\Cypress MicroSystems\PSoC Designer\tools\include"

@REM Start 
@REM ================================================================================================
CLS

@REM os(linlike8) drivers
@REM ================================================================================================

del %COMPILE_DIR%\main.c
@if ERRORLEVEL 1 goto ERR_EXIT

del %COMPILE_DIR%\setup.c
@if ERRORLEVEL 1 goto ERR_EXIT

del %COMPILE_DIR%\time.c
@if ERRORLEVEL 1 goto ERR_EXIT

del %COMPILE_DIR%\sched.h
@if ERRORLEVEL 1 goto ERR_EXIT

del %COMPILE_DIR%\current.h
@if ERRORLEVEL 1 goto ERR_EXIT

del %COMPILE_SYS_DIR%\asm\system.h
@if ERRORLEVEL 1 goto ERR_EXIT

del %COMPILE_SYS_DIR%\asm\hardirq.h
@if ERRORLEVEL 1 goto ERR_EXIT

del %COMPILE_SYS_DIR%\asm\softirq.h
@if ERRORLEVEL 1 goto ERR_EXIT

del %COMPILE_DIR%\gpio.c
@if ERRORLEVEL 1 goto ERR_EXIT

del %COMPILE_DIR%\gpio.h
@if ERRORLEVEL 1 goto ERR_EXIT

del %COMPILE_DIR%\serial.c
@if ERRORLEVEL 1 goto ERR_EXIT

del %COMPILE_DIR%\uart.h
@if ERRORLEVEL 1 goto ERR_EXIT

del %COMPILE_DIR%\i2c.c
@if ERRORLEVEL 1 goto ERR_EXIT

del %COMPILE_DIR%\i2c.h
@if ERRORLEVEL 1 goto ERR_EXIT

@REM os(linlike8) core
@REM ================================================================================================

del %COMPILE_SYS_DIR%\linlike8\config.h
@if ERRORLEVEL 1 goto ERR_EXIT

del %COMPILE_DIR%\maink.c
@if ERRORLEVEL 1 goto ERR_EXIT

del %COMPILE_DIR%\sched.c
@if ERRORLEVEL 1 goto ERR_EXIT

del %COMPILE_DIR%\fork.c
@if ERRORLEVEL 1 goto ERR_EXIT

del %COMPILE_DIR%\timer.c
@if ERRORLEVEL 1 goto ERR_EXIT

del %COMPILE_DIR%\softirq.c
@if ERRORLEVEL 1 goto ERR_EXIT

del %COMPILE_DIR%\time.h
@if ERRORLEVEL 1 goto ERR_EXIT

del %COMPILE_DIR%\timer.h
@if ERRORLEVEL 1 goto ERR_EXIT

del %COMPILE_DIR%\interrupt.h
@if ERRORLEVEL 1 goto ERR_EXIT

del %COMPILE_SYS_DIR%\linlike8\irq_cpustat.h
@if ERRORLEVEL 1 goto ERR_EXIT

del %COMPILE_DIR%\cirbuf.c
@if ERRORLEVEL 1 goto ERR_EXIT

del %COMPILE_DIR%\cirbuf.h
@if ERRORLEVEL 1 goto ERR_EXIT

del %COMPILE_DIR%\updownptr.c
@if ERRORLEVEL 1 goto ERR_EXIT

del %COMPILE_DIR%\updownptr.h
@if ERRORLEVEL 1 goto ERR_EXIT

del %COMPILE_DIR%\unistd.c
@if ERRORLEVEL 1 goto ERR_EXIT

del %COMPILE_SYS_DIR%\unistd.h
@if ERRORLEVEL 1 goto ERR_EXIT

del %COMPILE_DIR%\sem.c
@if ERRORLEVEL 1 goto ERR_EXIT

del %COMPILE_SYS_DIR%\semaphore.h
@if ERRORLEVEL 1 goto ERR_EXIT

@REM os module(s)
@REM ================================================================================================

@REM app.
@REM ================================================================================================
del %COMPILE_DIR%\srvmain.c
@if ERRORLEVEL 1 goto ERR_EXIT

del %COMPILE_SYS_DIR%\pin_define.h
@if ERRORLEVEL 1 goto ERR_EXIT

del %COMPILE_DIR%\app.h 
@if ERRORLEVEL 1 goto ERR_EXIT

@REM intermediate files
@REM ================================================================================================

rmdir %COMPILE_SYS_DIR%\linlike8\
@if ERRORLEVEL 1 goto ERR_EXIT

rmdir %COMPILE_SYS_DIR%\asm\
@if ERRORLEVEL 1 goto ERR_EXIT

del %COMPILE_DIR%\lib\obj\*.lis

del/Q %COMPILE_DIR%\backup\*.*

del %COMPILE_DIR%\obj\*.lis

del %COMPILE_DIR%\*.i

del %COMPILE_DIR%\*.s

@REM Exit
@REM ================================================================================================

:OK_EXIT
color
@ECHO It all completed
@goto END

:ERR_EXIT
color fc
@ECHO It has error(s), exit now

:END
@ECHO.| time | find /v "new"

::copy %LSR_CTRL_ALL_DIR%\ %COMPILE_DIR%\
::@if ERRORLEVEL 1 goto ERR_EXIT
