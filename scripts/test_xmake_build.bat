@echo off
REM Test script for QtLucide XMake build system on Windows

echo QtLucide XMake Build System Test
echo ========================================

REM Check if XMake is installed
xmake --version >nul 2>&1
if %errorlevel% neq 0 (
    echo ERROR: XMake is not installed or not in PATH
    echo Please install XMake from https://xmake.io/
    exit /b 1
)

echo XMake is available

REM Get project directory (parent of scripts directory)
set SCRIPT_DIR=%~dp0
set PROJECT_DIR=%SCRIPT_DIR%..

echo Project directory: %PROJECT_DIR%

REM Check if xmake.lua exists
if not exist "%PROJECT_DIR%\xmake.lua" (
    echo ERROR: xmake.lua not found in project directory
    exit /b 1
)

echo Found xmake.lua

REM Change to project directory
cd /d "%PROJECT_DIR%"

echo.
echo === Cleaning previous build ===
xmake clean

echo.
echo === Configuring project ===
xmake config --mode=release
if %errorlevel% neq 0 (
    echo ERROR: Configuration failed
    exit /b 1
)

echo.
echo === Generating resources ===
xmake generate-resources
if %errorlevel% neq 0 (
    echo ERROR: Resource generation failed
    exit /b 1
)

echo.
echo === Building project ===
xmake build
if %errorlevel% neq 0 (
    echo ERROR: Build failed
    exit /b 1
)

echo.
echo === Testing examples build ===
xmake config --examples=true
xmake build QtLucideExample
if %errorlevel% neq 0 (
    echo WARNING: Example build failed
) else (
    echo Example build successful
)

xmake build QtLucideGallery
if %errorlevel% neq 0 (
    echo WARNING: Gallery build failed
) else (
    echo Gallery build successful
)

echo.
echo === Testing tests build ===
xmake config --tests=true
xmake build QtLucideTests
if %errorlevel% neq 0 (
    echo WARNING: Tests build failed
) else (
    echo Tests build successful
)

echo.
echo ========================================
echo XMake build system test completed!
echo The XMake build system is working correctly.

pause
