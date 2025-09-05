@echo off
REM QtLucide CMake Build Script for Windows
REM Wrapper for build_cmake.py with common build configurations

setlocal enabledelayedexpansion

REM Get script directory
set SCRIPT_DIR=%~dp0
set PROJECT_ROOT=%SCRIPT_DIR%..

echo QtLucide CMake Build Script for Windows
echo =====================================

REM Check if Python is available
python --version >nul 2>&1
if errorlevel 1 (
    echo ERROR: Python is not installed or not in PATH
    echo Please install Python 3.6 or later
    exit /b 1
)

REM Check if CMake is available
cmake --version >nul 2>&1
if errorlevel 1 (
    echo ERROR: CMake is not installed or not in PATH
    echo Please install CMake 3.16 or later
    exit /b 1
)

REM Parse command line arguments
set BUILD_TYPE=Debug
set BUILD_DIR=build_cmake
set EXAMPLES=true
set TESTS=true
set CLEAN=false
set INSTALL=false
set RUN_TESTS=false
set GENERATE_RESOURCES=false
set GENERATOR=
set JOBS=
set VERBOSE=false

:parse_args
if "%~1"=="" goto end_parse
if /i "%~1"=="--help" goto show_help
if /i "%~1"=="-h" goto show_help
if /i "%~1"=="--debug" set BUILD_TYPE=Debug
if /i "%~1"=="--release" set BUILD_TYPE=Release
if /i "%~1"=="--relwithdebinfo" set BUILD_TYPE=RelWithDebInfo
if /i "%~1"=="--minsizerel" set BUILD_TYPE=MinSizeRel
if /i "%~1"=="--builddir" (
    shift
    set BUILD_DIR=%~1
)
if /i "%~1"=="--generator" (
    shift
    set GENERATOR=%~1
)
if /i "%~1"=="--no-examples" set EXAMPLES=false
if /i "%~1"=="--no-tests" set TESTS=false
if /i "%~1"=="--clean" set CLEAN=true
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
set PYTHON_CMD=python "%SCRIPT_DIR%build_cmake.py"
set PYTHON_CMD=%PYTHON_CMD% --builddir "%BUILD_DIR%"
set PYTHON_CMD=%PYTHON_CMD% --buildtype %BUILD_TYPE%

if not "%GENERATOR%"=="" (
    set PYTHON_CMD=%PYTHON_CMD% --generator "%GENERATOR%"
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
echo   Build type: %BUILD_TYPE%
echo   Build directory: %BUILD_DIR%
echo   Examples: %EXAMPLES%
echo   Tests: %TESTS%
echo   Generator: %GENERATOR%
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
echo   --debug                 Build in Debug mode (default)
echo   --release               Build in Release mode
echo   --relwithdebinfo        Build in RelWithDebInfo mode
echo   --minsizerel            Build in MinSizeRel mode
echo   --builddir DIR          Build directory (default: build_cmake)
echo   --generator GEN         CMake generator (e.g., "Ninja", "Visual Studio 16 2019")
echo   --no-examples           Don't build examples
echo   --no-tests              Don't build tests
echo   --clean                 Clean build directory first
echo   --install               Install after building
echo   --test                  Run tests after building
echo   --generate-resources    Generate resources before building
echo   --jobs N                Number of parallel jobs
echo   --verbose               Verbose output
echo.
echo Examples:
echo   %~nx0 --release --clean
echo   %~nx0 --debug --no-examples --test
echo   %~nx0 --generator "Ninja" --jobs 4

:end
endlocal
