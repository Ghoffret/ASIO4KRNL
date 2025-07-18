@echo off
setlocal

rem Build and optionally install the ASIO4KRNL driver.

rem Locate msbuild.exe
for /f "delims=" %%I in ('where msbuild.exe 2^>nul') do set MSBUILD=%%I
if not defined MSBUILD (
    echo msbuild.exe not found. Please install Visual Studio with the Windows Driver Kit.
    exit /b 1
)

%MSBUILD% "%~dp0src\driver\ASIOUSB.vcxproj" /t:Build /p:Configuration=Release /p:Platform=x64
if errorlevel 1 (
    echo Build failed.
    exit /b %errorlevel%
)

rem Attempt to install the driver using devcon if available
for /f "delims=" %%I in ('where devcon.exe 2^>nul') do set DEVCON=%%I
if defined DEVCON (
    %DEVCON% install "%~dp0src\driver\ASIOUSB.inf" USB\VID_1397&PID_0001
    echo Driver installed using devcon.
) else (
    echo Build succeeded. Use pnputil or devcon to install src\driver\ASIOUSB.inf.
)

endlocal
