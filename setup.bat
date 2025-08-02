@echo off
setlocal

rem Build and optionally install the ASIO4KRNL driver.
rem Optimized version with parallel builds and better error handling.

net session >nul 2>&1
if %errorlevel% neq 0 (
    echo This script must be run as Administrator.
    pause
    exit /b
)

rem Locate msbuild.exe with improved search
for /f "delims=" %%I in ('where msbuild.exe 2^>nul') do set MSBUILD=%%I
if not defined MSBUILD (
    echo msbuild.exe not found. Please install Visual Studio with the Windows Driver Kit.
    exit /b 1
)

echo Building ASIO4KRNL driver...

rem Build with optimized settings and parallel processing
%MSBUILD% "%~dp0src\driver\ASIOUSB.vcxproj" /t:Build /p:Configuration=Release /p:Platform=x64 /m /p:CL_MPCount=4 /verbosity:minimal
if errorlevel 1 (
    echo Build failed.
    exit /b %errorlevel%
)

echo Build completed successfully.

rem Attempt to install the driver using devcon if available
for /f "delims=" %%I in ('where devcon.exe 2^>nul') do set DEVCON=%%I
if defined DEVCON (
    echo Installing driver...
    %DEVCON% install "%~dp0src\driver\ASIOUSB.inf" USB\VID_1397&PID_0001
    if errorlevel 1 (
        echo Driver installation failed.
        exit /b %errorlevel%
    )
    echo Driver installed successfully using devcon.
) else (
    echo Build succeeded. Use pnputil or devcon to install src\driver\ASIOUSB.inf.
)

endlocal
