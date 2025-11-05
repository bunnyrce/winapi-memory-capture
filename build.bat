@echo off
REM Build script for Windows API Memory Scanner
REM //madebybunnyrce
REM Requires Visual Studio and Windows SDK

if not defined VSINSTALLDIR (
    echo Visual Studio environment not found. Run "Developer Command Prompt for VS" first.
    goto :eof
)

echo Building Windows API Memory Scanner...

if not exist bin mkdir bin

cl /EHsc /W3 /MD "capture.cpp" /Fe:bin\capture.exe /link psapi.lib ws2_32.lib
if errorlevel 1 (
    echo Capture build failed
    goto :eof
)

cl /EHsc /W3 /MD "receiver.cpp" /Fe:bin\receiver.exe /link ws2_32.lib  
if errorlevel 1 (
    echo Receiver build failed
    goto :eof
)

echo Build completed successfully!
echo Run bin\receiver.exe first, then bin\capture.exe
echo Note: capture.exe requires administrator privileges for memory access
pause