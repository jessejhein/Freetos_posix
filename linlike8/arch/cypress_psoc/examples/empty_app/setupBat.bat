:: 
:: this windows batch file copys needed parameter file(s) to setup a working environment
:: ================================================================================================

:: configuration of this batch file
:: ================================================================================================
set OS_CORE_DIR=..\..\..\..
set COMPILE_SYS_DIR="C:\Program Files\Cypress MicroSystems\PSoC Designer\tools\include"

@REM Start 
@REM ================================================================================================
CLS

@REM os(linlike8)
@REM ================================================================================================

copy %OS_CORE_DIR%\config\config.h.demo %OS_CORE_DIR%\include\linlike8\config.h
@if ERRORLEVEL 1 goto ERR_EXIT

@REM PSoC
@REM ================================================================================================

mkdir %COMPILE_SYS_DIR%\linlike8

mkdir %COMPILE_SYS_DIR%\asm

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
