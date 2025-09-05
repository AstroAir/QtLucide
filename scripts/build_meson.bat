@echo off
REM QtLucide Meson Build Script for Windows
REM Equivalent to the existing CMake and XMake build scripts

setlocal enabledelayedexpansion

REM Default values
set BUILDDIR=build_meson
set BUILDTYPE=debug
set EXAMPLES=true
set TESTS=true
set DEBUG_EXECUTABLES=false
set CLEAN=false
set INSTALL=false
set RUN_TESTS=false

REM Parse command line arguments
:parse_args
if "%~1"=="" goto :done_parsing
if "%~1"=="--builddir" (
    set BUILDDIR=%~2
    shift
    shift
    goto :parse_args
)
if "%~1"=="--buildtype" (
    set BUILDTYPE=%~2
    shift
    shift
    goto :parse_args
)
if "%~1"=="--no-examples" (
    set EXAMPLES=false
    shift
    goto :parse_args
)
if "%~1"=="--no-tests" (
    set TESTS=false
    shift
    goto :parse_args
)
if "%~1"=="--debug-executables" (
    set DEBUG_EXECUTABLES=true
    shift
    goto :parse_args
)
if "%~1"=="--clean" (
    set CLEAN=true
    shift
    goto :parse_args
)
if "%~1"=="--install" (
    set INSTALL=true
    shift
    goto :parse_args
)
if "%~1"=="--test" (
    set RUN_TESTS=true
    shift
    goto :parse_args
)
if "%~1"=="--help" (
    echo QtLucide Meson Build Script
    echo Usage: %0 [options]
    echo Options:
    echo   --builddir DIR        Build directory (default: build_meson)
    echo   --buildtype TYPE      Build type: debug, release, debugoptimized (default: debug)
    echo   --no-examples         Don't build examples
    echo   --no-tests            Don't build tests
    echo   --debug-executables   Build debug executables
    echo   --clean               Clean build directory first
    echo   --install             Install after building
    echo   --test                Run tests after building
    echo   --help                Show this help
    exit /b 0
)
shift
goto :parse_args

:done_parsing

echo QtLucide Meson Build Script
echo Build directory: %BUILDDIR%
echo Build type: %BUILDTYPE%
echo Examples: %EXAMPLES%
echo Tests: %TESTS%
echo Debug executables: %DEBUG_EXECUTABLES%

REM Get script directory and project root
set SCRIPT_DIR=%~dp0
set PROJECT_ROOT=%SCRIPT_DIR%..

REM Clean if requested
if "%CLEAN%"=="true" (
    if exist "%PROJECT_ROOT%\%BUILDDIR%" (
        echo Cleaning build directory...
        rmdir /s /q "%PROJECT_ROOT%\%BUILDDIR%"
    )
)

REM Setup build directory
if not exist "%PROJECT_ROOT%\%BUILDDIR%" (
    echo Setting up build directory...
    meson setup "%PROJECT_ROOT%\%BUILDDIR%" --buildtype=%BUILDTYPE% -Dexamples=%EXAMPLES% -Dtests=%TESTS% -Ddebug_executables=%DEBUG_EXECUTABLES%
    if errorlevel 1 (
        echo Error setting up build directory
        exit /b 1
    )
) else (
    echo Build directory exists, reconfiguring...
    meson configure "%PROJECT_ROOT%\%BUILDDIR%" --buildtype=%BUILDTYPE% -Dexamples=%EXAMPLES% -Dtests=%TESTS% -Ddebug_executables=%DEBUG_EXECUTABLES%
    if errorlevel 1 (
        echo Error reconfiguring build directory
        exit /b 1
    )
)

REM Build
echo Building...
meson compile -C "%PROJECT_ROOT%\%BUILDDIR%"
if errorlevel 1 (
    echo Build failed
    exit /b 1
)

REM Run tests if requested
if "%RUN_TESTS%"=="true" (
    if "%TESTS%"=="true" (
        echo Running tests...
        meson test -C "%PROJECT_ROOT%\%BUILDDIR%"
    )
)

REM Install if requested
if "%INSTALL%"=="true" (
    echo Installing...
    meson install -C "%PROJECT_ROOT%\%BUILDDIR%"
    if errorlevel 1 (
        echo Installation failed
        exit /b 1
    )
)

echo Build completed successfully!
echo.
echo Build Summary:
echo   Build type: %BUILDTYPE%
echo   Examples: %EXAMPLES%
echo   Tests: %TESTS%
echo   Debug executables: %DEBUG_EXECUTABLES%
echo   Build directory: %PROJECT_ROOT%\%BUILDDIR%
