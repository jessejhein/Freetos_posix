:: 
:: this windows batch file copys needed source file(s) to PSoC directory, let C-Compile compiling them
::      linlike8 Original for Cypress PSoC Arch
::
:: Module(s)
::      config. keyboard
:: ================================================================================================

:: configuration of this batch file
:: ================================================================================================
set COMPILE_DIR=PSoCDesigner
set OS_CORE_DIR=..\..
set OS_DRIVER_DIR=.
set APP_DIR=..\..\test_apps

@REM Start 
@REM ================================================================================================
CLS

@REM os(linlike8) drivers
@REM ================================================================================================

copy %OS_DRIVER_DIR%\boot\boot.c %COMPILE_DIR%\main.c
@if ERRORLEVEL 1 goto ERR_EXIT

copy %OS_DRIVER_DIR%\kernel\setup.c %COMPILE_DIR%\
@if ERRORLEVEL 1 goto ERR_EXIT

copy %OS_DRIVER_DIR%\kernel\time.c %COMPILE_DIR%\
@if ERRORLEVEL 1 goto ERR_EXIT

copy %OS_DRIVER_DIR%\..\..\include\linlike8\sched.h %COMPILE_DIR%\
@if ERRORLEVEL 1 goto ERR_EXIT

copy %OS_DRIVER_DIR%\..\..\include\linlike8\current.h %COMPILE_DIR%\
@if ERRORLEVEL 1 goto ERR_EXIT

copy %OS_DRIVER_DIR%\..\..\include\asm-cypress_psoc\system.h %COMPILE_DIR%\
@if ERRORLEVEL 1 goto ERR_EXIT

copy %OS_DRIVER_DIR%\..\..\include\asm-cypress_psoc\hardirq.h %COMPILE_DIR%\
@if ERRORLEVEL 1 goto ERR_EXIT

copy %OS_DRIVER_DIR%\..\..\include\asm-cypress_psoc\softirq.h %COMPILE_DIR%\
@if ERRORLEVEL 1 goto ERR_EXIT

copy %OS_DRIVER_DIR%\drivers\gpio.c %COMPILE_DIR%\
@if ERRORLEVEL 1 goto ERR_EXIT

copy %OS_DRIVER_DIR%\..\..\include\asm-cypress_psoc\gpio.h %COMPILE_DIR%\
@if ERRORLEVEL 1 goto ERR_EXIT

copy %OS_DRIVER_DIR%\drivers\serial.c %COMPILE_DIR%\
@if ERRORLEVEL 1 goto ERR_EXIT

copy %OS_DRIVER_DIR%\..\..\include\asm-cypress_psoc\serial.h %COMPILE_DIR%\
@if ERRORLEVEL 1 goto ERR_EXIT

copy %OS_DRIVER_DIR%\drivers\i2c.c %COMPILE_DIR%\
@if ERRORLEVEL 1 goto ERR_EXIT

copy %OS_DRIVER_DIR%\..\..\include\asm-cypress_psoc\i2c.h %COMPILE_DIR%\
@if ERRORLEVEL 1 goto ERR_EXIT

@REM os(linlike8) core
@REM ================================================================================================

copy %OS_CORE_DIR%\init\main.c %COMPILE_DIR%\maink.c
@if ERRORLEVEL 1 goto ERR_EXIT

copy %OS_CORE_DIR%\kernel\sched.c %COMPILE_DIR%\
@if ERRORLEVEL 1 goto ERR_EXIT

copy %OS_CORE_DIR%\kernel\fork.c %COMPILE_DIR%\
@if ERRORLEVEL 1 goto ERR_EXIT

copy %OS_CORE_DIR%\kernel\timer.c %COMPILE_DIR%\
@if ERRORLEVEL 1 goto ERR_EXIT

copy %OS_CORE_DIR%\kernel\softirq.c %COMPILE_DIR%\
@if ERRORLEVEL 1 goto ERR_EXIT

copy %OS_CORE_DIR%\include\linlike8\time.h %COMPILE_DIR%\
@if ERRORLEVEL 1 goto ERR_EXIT

copy %OS_CORE_DIR%\include\linlike8\timer.h %COMPILE_DIR%\
@if ERRORLEVEL 1 goto ERR_EXIT

copy %OS_CORE_DIR%\include\linlike8\interrupt.h %COMPILE_DIR%\
@if ERRORLEVEL 1 goto ERR_EXIT

copy %OS_CORE_DIR%\include\linlike8\irq_cpustat.h %COMPILE_DIR%\
@if ERRORLEVEL 1 goto ERR_EXIT

copy %OS_CORE_DIR%\lib\cirbuf.c %COMPILE_DIR%\
@if ERRORLEVEL 1 goto ERR_EXIT

copy %OS_CORE_DIR%\include\linlike8\cirbuf.h %COMPILE_DIR%\
@if ERRORLEVEL 1 goto ERR_EXIT

@REM os module(s)
@REM ================================================================================================
:       keyboard
copy ..\..\..\..\Apps\LsrCtrl\platform\benchtop\cypress_psoc\kb.h %COMPILE_DIR%\
@if ERRORLEVEL 1 goto ERR_EXIT

copy %APP_DIR%\gpio_kb_app.h %COMPILE_DIR%\
@if ERRORLEVEL 1 goto ERR_EXIT

@REM app.
@REM ================================================================================================
copy %APP_DIR%\app.h %COMPILE_DIR%\
@if ERRORLEVEL 1 goto ERR_EXIT

copy %APP_DIR%\process0.c %COMPILE_DIR%\
@if ERRORLEVEL 1 goto ERR_EXIT

copy %APP_DIR%\process1.c %COMPILE_DIR%\
@if ERRORLEVEL 1 goto ERR_EXIT

@REM exit
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
