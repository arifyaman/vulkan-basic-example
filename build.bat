@echo off
REM Vulkan Example CMake Build Script for Windows

setlocal enabledelayedexpansion

REM Get the script directory
set SCRIPT_DIR=%~dp0
set PROJECT_DIR=%SCRIPT_DIR%
set BUILD_DIR=%PROJECT_DIR%build
set INSTALL_DIR=%PROJECT_DIR%

REM Check if CMAKE is installed
cmake --version >nul 2>&1
if errorlevel 1 (
    echo Error: CMake is not installed or not in PATH
    echo Please install CMake from https://cmake.org/download/
    exit /b 1
)

REM Create build directory if it doesn't exist
if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"

REM Determine build type
set BUILD_TYPE=Debug
if "%1"=="release" set BUILD_TYPE=Release

echo.
echo ======================================
echo Building Vulkan Example (%BUILD_TYPE%)
echo ======================================
echo Project Dir: %PROJECT_DIR%
echo Build Dir: %BUILD_DIR%
echo.

REM Configure with CMake
echo Configuring CMake...
cd /d "%BUILD_DIR%"
cmake -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=%BUILD_TYPE% "%PROJECT_DIR%"

if errorlevel 1 (
    echo Error: CMake configuration failed
    exit /b 1
)

REM Build
echo.
echo Building project...
cmake --build . --config %BUILD_TYPE%

if errorlevel 1 (
    echo Error: Build failed
    exit /b 1
)

echo.
echo ======================================
echo Build completed successfully!
echo Output: %BUILD_DIR%\bin\vulkan_app.exe
echo ======================================
echo.

if "%2"=="run" (
    echo Running application...
    "%BUILD_DIR%\bin\%BUILD_TYPE%\vulkan_app.exe"
)

endlocal
