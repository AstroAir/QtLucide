@echo off
REM QtLucide Unified Build Script for Windows
REM Automatically detects and uses the best available build system

setlocal enabledelayedexpansion

REM Get script directory
set SCRIPT_DIR=%~dp0
set PROJECT_ROOT=%SCRIPT_DIR%..

echo QtLucide Unified Build Script for Windows
echo =========================================

REM Check if Python is available
python --version >nul 2>&1
if errorlevel 1 (
    echo ERROR: Python is not installed or not in PATH
    echo Please install Python 3.6 or later
    exit /b 1
)

REM Parse command line arguments and pass them to Python script
set PYTHON_CMD=python "%SCRIPT_DIR%build.py"

REM Add all arguments to the Python command
:parse_args
if "%~1"=="" goto end_parse
set PYTHON_CMD=%PYTHON_CMD% "%~1"
shift
goto parse_args

:end_parse

REM Show help if no arguments provided
if "%PYTHON_CMD%"=="python "%SCRIPT_DIR%build.py"" (
    echo No arguments provided. Showing help:
    echo.
    %PYTHON_CMD% --help
    goto end
)

REM Execute Python script
echo Executing: %PYTHON_CMD%
echo.
%PYTHON_CMD%

if errorlevel 1 (
    echo.
    echo BUILD FAILED!
    exit /b 1
)

echo.
echo BUILD COMPLETED SUCCESSFULLY!

:end
endlocal
