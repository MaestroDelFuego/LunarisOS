@echo off
setlocal enabledelayedexpansion

:: Get current directory (where the BAT file is located)
set "BASEDIR=%~dp0"
cd /d "%BASEDIR%"

:: Output file name
set "OUTPUT=source_dump.txt"
echo Generating source code dump in: %OUTPUT%
echo Source Code Dump - NEO98 > "%OUTPUT%"
echo Base directory: %BASEDIR% >> "%OUTPUT%"
echo =================================== >> "%OUTPUT%"

:: List of source file extensions
set EXTENSIONS=.c .h .s .asm .ld

:: Loop through each extension and each matching file
for %%E in (%EXTENSIONS%) do (
    for /R "%BASEDIR%" %%F in (*%%E) do (
        echo.>> "%OUTPUT%"
        echo =================================== >> "%OUTPUT%"
        echo File: %%~fF >> "%OUTPUT%"
        echo ----------------------------------- >> "%OUTPUT%"
        type "%%~fF" >> "%OUTPUT%"
    )
)

echo Done. Output saved to %OUTPUT%
pause
