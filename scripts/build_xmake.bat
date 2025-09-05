@echo off
REM QtLucide XMake Build Script for Windows
REM Wrapper for build_xmake.py with common build configurations

setlocal enabledelayedexpansion

REM Get script directory
set SCRIPT_DIR=%~dp0
set PROJECT_ROOT=%SCRIPT_DIR%..

echo QtLucide XMake Build Script for Windows
echo ======================================

REM Check if Python is available
python --version >nul 2>&1
if errorlevel 1 (
    echo ERROR: Python is not installed or not in PATH
    echo Please install Python 3.6 or later
    exit /b 1
)

REM Check if XMake is available
xmake --version >nul 2>&1
if errorlevel 1 (
    echo ERROR: XMake is not installed or not in PATH
    echo Please install XMake from https://xmake.io
    exit /b 1
)

REM Parse command line arguments
set BUILD_MODE=debug
set ARCH=
set PLAT=
set EXAMPLES=true
set TESTS=true
set CLEAN=false
set REBUILD=false
set INSTALL=false
set RUN_TESTS=false
set GENERATE_RESOURCES=false
set JOBS=
set VERBOSE=false
set TARGET=

:parse_args
if "%~1"=="" goto end_parse
if /i "%~1"=="--help" goto show_help
if /i "%~1"=="-h" goto show_help
if /i "%~1"=="--debug" set BUILD_MODE=debug
if /i "%~1"=="--release" set BUILD_MODE=release
if /i "%~1"=="--releasedbg" set BUILD_MODE=releasedbg
if /i "%~1"=="--minsizerel" set BUILD_MODE=minsizerel
if /i "%~1"=="--arch" (
    shift
    set ARCH=%~1
)
if /i "%~1"=="--plat" (
    shift
    set PLAT=%~1
)
if /i "%~1"=="--target" (
    shift
    set TARGET=%~1
)
if /i "%~1"=="--no-examples" set EXAMPLES=false
if /i "%~1"=="--no-tests" set TESTS=false
if /i "%~1"=="--clean" set CLEAN=true
if /i "%~1"=="--rebuild" set REBUILD=true
if /i "%~1"=="--install" set INSTALL=true
if /i "%~1"=="--test" set RUN_TESTS=true
if /i "%~1"=="--generate-resources" set GENERATE_RESOURCES=true
if /i "%~1"=="--jobs" (
    shift
    set JOBS=%~1
)
if /i "%~1"=="--verbose" set VERBOSE=true
shift
goto parse_args

:end_parse

REM Build Python command
set PYTHON_CMD=python "%SCRIPT_DIR%build_xmake.py"
set PYTHON_CMD=%PYTHON_CMD% --mode %BUILD_MODE%

if not "%ARCH%"=="" (
    set PYTHON_CMD=%PYTHON_CMD% --arch %ARCH%
)

if not "%PLAT%"=="" (
    set PYTHON_CMD=%PYTHON_CMD% --plat %PLAT%
)

if not "%TARGET%"=="" (
    set PYTHON_CMD=%PYTHON_CMD% --target %TARGET%
)

if "%EXAMPLES%"=="false" (
    set PYTHON_CMD=%PYTHON_CMD% --no-examples
)

if "%TESTS%"=="false" (
    set PYTHON_CMD=%PYTHON_CMD% --no-tests
)

if "%CLEAN%"=="true" (
    set PYTHON_CMD=%PYTHON_CMD% --clean
)

if "%REBUILD%"=="true" (
    set PYTHON_CMD=%PYTHON_CMD% --rebuild
)

if "%INSTALL%"=="true" (
    set PYTHON_CMD=%PYTHON_CMD% --install
)

if "%RUN_TESTS%"=="true" (
    set PYTHON_CMD=%PYTHON_CMD% --test
)

if "%GENERATE_RESOURCES%"=="true" (
    set PYTHON_CMD=%PYTHON_CMD% --generate-resources
)

if not "%JOBS%"=="" (
    set PYTHON_CMD=%PYTHON_CMD% --jobs %JOBS%
)

if "%VERBOSE%"=="true" (
    set PYTHON_CMD=%PYTHON_CMD% --verbose
)

echo Build configuration:
echo   Build mode: %BUILD_MODE%
echo   Architecture: %ARCH%
echo   Platform: %PLAT%
echo   Examples: %EXAMPLES%
echo   Tests: %TESTS%
echo   Target: %TARGET%
echo.

REM Execute Python script
echo Executing: %PYTHON_CMD%
%PYTHON_CMD%

if errorlevel 1 (
    echo.
    echo BUILD FAILED!
    exit /b 1
)

echo.
echo BUILD COMPLETED SUCCESSFULLY!
goto end

:show_help
echo Usage: %~nx0 [options]
echo.
echo Options:
echo   --help, -h              Show this help message
echo   --debug                 Build in debug mode (default)
echo   --release               Build in release mode
echo   --releasedbg            Build in release with debug info mode
echo   --minsizerel            Build in minimum size release mode
echo   --arch ARCH             Target architecture (e.g., x86_64, x86)
echo   --plat PLAT             Target platform (e.g., windows, linux, macosx)
echo   --target TARGET         Specific target to build
echo   --no-examples           Don't build examples
echo   --no-tests              Don't build tests
echo   --clean                 Clean build first
echo   --rebuild               Rebuild all targets
echo   --install               Install after building
echo   --test                  Run tests after building
echo   --generate-resources    Generate resources before building
echo   --jobs N                Number of parallel jobs
echo   --verbose               Verbose output
echo.
echo Examples:
echo   %~nx0 --release --clean
echo   %~nx0 --debug --no-examples --test
echo   %~nx0 --target QtLucideExample --jobs 4

:end
endlocal
