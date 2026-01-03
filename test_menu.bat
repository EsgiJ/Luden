@echo off
setlocal enabledelayedexpansion

:menu
cls
echo ========================================
echo   LUDEN ENGINE - TEST MENU
echo ========================================
echo.
echo 1. Run All Tests (Debug)
echo 2. Run Quick Tests
echo 3. Run Benchmarks (Release)
echo 4. Build and Test
echo 5. Generate Report
echo 6. Clean Test Files
echo 7. Continuous Test Mode
echo 8. Exit
echo.
set /p choice="Select option (1-8): "

if "%choice%"=="1" (
    call run_tests.bat
    goto menu
)
if "%choice%"=="2" (
    call run_quick_tests.bat
    goto menu
)
if "%choice%"=="3" (
    call run_benchmarks.bat
    goto menu
)
if "%choice%"=="4" (
    call build_and_test.bat
    goto menu
)
if "%choice%"=="5" (
    call test_with_report.bat
    goto menu
)
if "%choice%"=="6" (
    call clean_tests.bat
    goto menu
)
if "%choice%"=="7" (
    call continuous_test.bat
    goto menu
)
if "%choice%"=="8" (
    exit /b 0
)

echo Invalid choice!
timeout /t 2 > nul
goto menu