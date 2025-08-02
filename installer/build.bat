@echo off
:: ASIO4KRNL Installer Build Script (Batch version)
:: Enhanced build script with error handling and validation

setlocal enabledelayedexpansion

echo.
echo ===============================================================
echo                 ASIO4KRNL Installer Build                   
echo                     Developer Build                         
echo ===============================================================
echo.
echo WARNING: This script is for developers building the installer package.
echo.
echo END USERS: Download the pre-built installer from the releases page instead.
echo.

:: Check for WiX Toolset
echo Checking build prerequisites...
where candle.exe >nul 2>&1
if errorlevel 1 (
    echo ERROR: WiX Toolset not found.
    echo Please install WiX Toolset v3.11+ from https://wixtoolset.org/
    echo.
    pause
    exit /b 1
)

where light.exe >nul 2>&1
if errorlevel 1 (
    echo ERROR: WiX Toolset light.exe not found.
    echo Please install WiX Toolset v3.11+ from https://wixtoolset.org/
    echo.
    pause
    exit /b 1
)

echo ✓ WiX Toolset found

:: Check required files
set "MISSING_FILES="
set "CHECK_FILES=..\src\driver\x64\Release\ASIOUSB.sys ..\src\gui\x64\Release\ASIO4KrnlGUI.exe ..\src\driver\ASIOUSB.inf AppIcon.ico installer_banner.bmp installer_header.bmp README.txt"

for %%f in (%CHECK_FILES%) do (
    if not exist "%%f" (
        set "MISSING_FILES=!MISSING_FILES! %%f"
    )
)

if not "!MISSING_FILES!"=="" (
    echo ERROR: Missing required files:
    for %%f in (!MISSING_FILES!) do echo   • %%f
    echo.
    echo Please ensure all required files are built and present.
    pause
    exit /b 1
)

echo ✓ All required files present

:: Create output directory
if not exist "Output" mkdir "Output"

:: Clean previous build
if exist "Output\*.wixobj" del "Output\*.wixobj"
if exist "Output\*.msi" del "Output\*.msi"
if exist "Output\*.log" del "Output\*.log"

:: Define build paths
set "WXS_FILE=ASIO4KrnlInstaller.wxs"
set "WIXOBJ_FILE=Output\ASIO4KrnlInstaller.wixobj"
set "OUTPUT_MSI=Output\ASIO4KRNL-Setup.msi"

echo.
echo Building installer...
echo.

:: Compile WiX source
echo Compiling WiX source files...
candle.exe -arch x64 -out "%WIXOBJ_FILE%" -ext WixUtilExtension -v "%WXS_FILE%" > "Output\candle.log" 2> "Output\candle.err"

if errorlevel 1 (
    echo ERROR: candle.exe compilation failed
    echo Check Output\candle.err for details:
    echo.
    type "Output\candle.err"
    echo.
    pause
    exit /b 1
)

echo ✓ WiX compilation completed

:: Link installer
echo Linking installer package...
light.exe -ext WixUIExtension -ext WixUtilExtension -cultures:en-US -out "%OUTPUT_MSI%" -v "%WIXOBJ_FILE%" > "Output\light.log" 2> "Output\light.err"

if errorlevel 1 (
    echo ERROR: light.exe linking failed
    echo Check Output\light.err for details:
    echo.
    type "Output\light.err"
    echo.
    pause
    exit /b 1
)

echo ✓ Installer linking completed

:: Validate output
if not exist "%OUTPUT_MSI%" (
    echo ERROR: Installer file was not created
    pause
    exit /b 1
)

:: Get file size
for %%f in ("%OUTPUT_MSI%") do set "FILE_SIZE=%%~zf"
set /a "FILE_SIZE_KB=!FILE_SIZE!/1024"

echo.
echo ===============================================================
echo                      BUILD COMPLETE                         
echo ===============================================================
echo.
echo ✓ Installer created successfully:
echo   • File: %OUTPUT_MSI%
echo   • Size: !FILE_SIZE_KB! KB
echo   • Date: %DATE% %TIME%
echo.
echo Next steps:
echo   1. Test the installer on a clean Windows system
echo   2. Verify all components install correctly
echo   3. Test both installation and uninstallation
echo   4. Sign the installer for distribution
echo.

:: Optional: Open output directory
set /p "OPEN_DIR=Open output directory? (y/n): "
if /i "!OPEN_DIR!"=="y" (
    explorer "Output"
)

echo Build completed successfully!
pause