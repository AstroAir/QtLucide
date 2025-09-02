@echo off
REM QtLucide - Build Resources (Windows)
REM Convenience batch script for Windows users

echo QtLucide - Building All Resources
echo ========================================

REM Check if Python is available
python --version >nul 2>&1
if errorlevel 1 (
    echo Error: Python is not installed or not in PATH
    echo Please install Python 3.6+ and add it to your PATH
    pause
    exit /b 1
)

REM Run the resource builder
python build_all_resources.py

if errorlevel 1 (
    echo.
    echo ❌ Resource build failed!
    pause
    exit /b 1
)

echo.
echo ========================================
echo ✅ Resource build completed successfully!
echo.
echo Next steps:
echo 1. Run 'cmake --build build' to compile the library
echo 2. All SVG icons will be embedded in the compiled library
echo 3. Use the generated enums and functions in your Qt application
echo.
pause
