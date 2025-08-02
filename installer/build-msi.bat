@echo off
setlocal enabledelayedexpansion

echo ================================================================
echo ASIO4KRNL MSI Installer Build Script
echo ================================================================

rem Check for administrator privileges
net session >nul 2>&1
if %errorlevel% neq 0 (
    echo ERROR: This script must be run as Administrator.
    echo Right-click and select "Run as administrator"
    pause
    exit /b 1
)

rem Set variables
set "SCRIPT_DIR=%~dp0"
set "ROOT_DIR=%SCRIPT_DIR%.."
set "SRC_DIR=%ROOT_DIR%\src"
set "INSTALLER_DIR=%SCRIPT_DIR%"
set "BUILD_CONFIG=Release"
set "BUILD_PLATFORM=x64"

echo Script Directory: %SCRIPT_DIR%
echo Root Directory: %ROOT_DIR%
echo Source Directory: %SRC_DIR%

rem Locate MSBuild
for /f "delims=" %%I in ('where msbuild.exe 2^>nul') do set MSBUILD=%%I
if not defined MSBUILD (
    echo ERROR: MSBuild not found. Please install Visual Studio with Windows Driver Kit.
    pause
    exit /b 1
)
echo Found MSBuild: %MSBUILD%

rem Check for WiX Toolset
for /f "delims=" %%I in ('where candle.exe 2^>nul') do set WIX_CANDLE=%%I
if not defined WIX_CANDLE (
    echo ERROR: WiX Toolset not found. Please install WiX Toolset v3.11 or later.
    echo Download from: https://wixtoolset.org/releases/
    pause
    exit /b 1
)
echo Found WiX Toolset: %WIX_CANDLE%

echo.
echo ================================================================
echo Building ASIO4KRNL Components
echo ================================================================

rem Build the driver
echo Building ASIO driver...
"%MSBUILD%" "%SRC_DIR%\driver\ASIOUSB.vcxproj" /t:Build /p:Configuration=%BUILD_CONFIG% /p:Platform=%BUILD_PLATFORM% /nologo
if errorlevel 1 (
    echo ERROR: Driver build failed.
    pause
    exit /b %errorlevel%
)
echo Driver build completed successfully.

rem Build the GUI application
echo Building GUI application...
"%MSBUILD%" "%SRC_DIR%\gui\ASIO4KrnlGUI.vcxproj" /t:Build /p:Configuration=%BUILD_CONFIG% /p:Platform=%BUILD_PLATFORM% /nologo
if errorlevel 1 (
    echo ERROR: GUI build failed.
    pause
    exit /b %errorlevel%
)
echo GUI build completed successfully.

echo.
echo ================================================================
echo Building MSI Installer
echo ================================================================

rem Build the MSI installer
echo Building MSI installer...
"%MSBUILD%" "%INSTALLER_DIR%\ASIO4KrnlInstaller.wixproj" /t:Build /p:Configuration=%BUILD_CONFIG% /nologo
if errorlevel 1 (
    echo ERROR: MSI build failed.
    pause
    exit /b %errorlevel%
)

echo.
echo ================================================================
echo Build Completed Successfully!
echo ================================================================

rem Check if MSI was created
set "MSI_FILE=%INSTALLER_DIR%\bin\%BUILD_CONFIG%\ASIO4KRNL-%BUILD_CONFIG%.msi"
if exist "%MSI_FILE%" (
    echo MSI installer created: %MSI_FILE%
    echo File size: 
    for %%F in ("%MSI_FILE%") do echo   %%~zF bytes
    echo.
    
    choice /C YN /M "Do you want to open the installer directory"
    if !errorlevel! equ 1 (
        explorer "%INSTALLER_DIR%\bin\%BUILD_CONFIG%"
    )
) else (
    echo WARNING: MSI file not found at expected location: %MSI_FILE%
)

echo.
echo Build script completed.
pause