@echo off
setlocal

set CONFIG=Debug
set PLATFORM=windows-x86_64
set TEST_EXE=bin\%CONFIG%-%PLATFORM%\EngineTests.exe
set REPORT_FILE=TestReport_%date:~-4,4%%date:~-10,2%%date:~-7,2%_%time:~0,2%%time:~3,2%%time:~6,2%.txt

REM Remove colons from time
set REPORT_FILE=%REPORT_FILE::=%
REM Remove spaces
set REPORT_FILE=%REPORT_FILE: =0%

echo ========================================
echo   TEST WITH REPORT
echo ========================================
echo.

if not exist "%TEST_EXE%" (
    echo [ERROR] EngineTests.exe not found!
    pause
    exit /b 1
)

echo Running tests and generating report...
echo Output file: %REPORT_FILE%
echo.

REM Run tests and save output
"%TEST_EXE%" --all > %REPORT_FILE% 2>&1

REM Also display on screen
type %REPORT_FILE%

echo.
echo Report saved to: %REPORT_FILE%
echo.
pause