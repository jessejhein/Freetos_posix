:: 
:: this windows batch file copys needed source file(s) to PSoC directory, let C-Compile compiling them
::      mwlike8 Original for Cypress PSoC Arch
::      you can remove some un-used lib
::      modification from linlike8
::              over-write 
:: ================================================================================================

:: configuration of this batch file
:: ================================================================================================
set COMPILE_DIR=PSoCDesigner
set OS_CORE_DIR=I:\os\linlike8\backup\linlike8_0.01.03
set OS_DRIVER_DIR=%OS_CORE_DIR%\arch\cypress_psoc
set GUI_CORE_DIR=I:\gui\mwlike8\backup\mwlike8_0.01.01
set GUI_DRIVER_DIR=%GUI_CORE_DIR%\drivers
set LIB_DIR=I:\lib\ucliblike8\backup\ucliblike8_0.01.00
set APP_DIR=.

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

copy %OS_CORE_DIR%\lib\updownptr.c %COMPILE_DIR%\
@if ERRORLEVEL 1 goto ERR_EXIT

copy %OS_CORE_DIR%\include\linlike8\updownptr.h %COMPILE_DIR%\
@if ERRORLEVEL 1 goto ERR_EXIT

@REM os module(s)
@REM ================================================================================================
::      keyboard, hardware dependence
copy %APP_DIR%\arch\cypress_psoc\kb.h %COMPILE_DIR%\
@if ERRORLEVEL 1 goto ERR_EXIT

copy %GUI_CORE_DIR%\drivers\linlike8\gpio_kb_app.h %COMPILE_DIR%\
@if ERRORLEVEL 1 goto ERR_EXIT

@REM gui(mwlike8) drivers
@REM ================================================================================================

copy %GUI_DRIVER_DIR%\lcd_graphic\twlm6023.c %COMPILE_DIR%\
@if ERRORLEVEL 1 goto ERR_EXIT

copy %APP_DIR%\arch\cypress_psoc\twlm6023.h %COMPILE_DIR%\
@if ERRORLEVEL 1 goto ERR_EXIT

copy %APP_DIR%\arch\cypress_psoc\psoc_gpio_bit.h %COMPILE_DIR%\
@if ERRORLEVEL 1 goto ERR_EXIT

copy %GUI_CORE_DIR%\include\device.h %COMPILE_DIR%\
@if ERRORLEVEL 1 goto ERR_EXIT

@REM gui(mwlike8) core
@REM ================================================================================================
copy %GUI_CORE_DIR%\nanox\srvmain.c %COMPILE_DIR%\
@if ERRORLEVEL 1 goto ERR_EXIT

copy %GUI_CORE_DIR%\nanox\srvfunc.c %COMPILE_DIR%\
@if ERRORLEVEL 1 goto ERR_EXIT

copy %GUI_CORE_DIR%\include\nano-X.h %COMPILE_DIR%\
@if ERRORLEVEL 1 goto ERR_EXIT

copy %GUI_CORE_DIR%\include\mwtypes.h %COMPILE_DIR%\
@if ERRORLEVEL 1 goto ERR_EXIT

@REM lib(ucliblike8) core
@REM ================================================================================================
copy %LIB_DIR%\libc\stdio\printf.c %COMPILE_DIR%\
@if ERRORLEVEL 1 goto ERR_EXIT

copy %LIB_DIR%\include\printf.h %COMPILE_DIR%\
@if ERRORLEVEL 1 goto ERR_EXIT

@REM app.
@REM ================================================================================================
copy %APP_DIR%\app.h %COMPILE_DIR%\
@if ERRORLEVEL 1 goto ERR_EXIT

copy %APP_DIR%\mw_pro1.c %COMPILE_DIR%\
@if ERRORLEVEL 1 goto ERR_EXIT

:OK_EXIT
color
@ECHO It all completed
@goto END

:ERR_EXIT
color fc
@ECHO It has error(s), exit now

:END
@ECHO.| time | find /v "new"
