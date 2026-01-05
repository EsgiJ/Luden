@echo off
setlocal enabledelayedexpansion

REM Set colors (if supported)
color 0A

:menu
cls
echo.
echo ============================================================
echo               LUDEN ENGINE - TEST MENU
echo ============================================================
echo.
echo   QUICK TESTS (No Window)
echo   ------------------------
echo   1. Run Quick Tests              (Fast unit tests)
echo.
echo   VISUAL TESTS (Window Required)
echo   ------------------------------
echo   2. Run Integration Tests        (Physics, Rendering)
echo   3. Run Performance Benchmarks   (FPS measurements)
echo   4. Run All Tests                (Everything)
echo.
echo   BUILD OPTIONS
echo   -------------
echo   5. Build and Test (Debug)       (Rebuild + run all)
echo   6. Build and Benchmark (Release)(Optimized benchmarks)
echo.
echo   UTILITIES
echo   ---------
echo   7. Generate Test Report         (Save results to file)
echo   8. Clean Test Files             (Remove temp files)
echo   9. Continuous Test Mode         (Auto-rerun on changes)
echo.
echo   0. Exit
echo.
echo ============================================================
set /p choice="Select option (0-9): "

if "%choice%"=="1" goto quick_tests
if "%choice%"=="2" goto integration_tests
if "%choice%"=="3" goto benchmarks
if "%choice%"=="4" goto all_tests
if "%choice%"=="5" goto build_test
if "%choice%"=="6" goto build_benchmark
if "%choice%"=="7" goto generate_report
if "%choice%"=="8" goto clean_files
if "%choice%"=="9" goto continuous_mode
if "%choice%"=="0" goto exit_menu

echo.
echo [ERROR] Invalid choice! Please enter 0-9.
timeout /t 2 > nul
goto menu

:quick_tests
cls
echo.
echo ==========================================
echo   QUICK TESTS
echo ==========================================
echo.
call run_quick_tests.bat
goto menu

:integration_tests
cls
echo.
echo ==========================================
echo   INTEGRATION TESTS
echo ==========================================
echo.
call run_integration_tests.bat
goto menu

:benchmarks
cls
echo.
echo ==========================================
echo   PERFORMANCE BENCHMARKS
echo ==========================================
echo.
call run_benchmarks.bat
goto menu

:all_tests
cls
echo.
echo ==========================================
echo   ALL TESTS
echo ==========================================
echo.
call run_tests.bat
goto menu

:build_test
cls
echo.
echo ==========================================
echo   BUILD AND TEST
echo ==========================================
echo.
call build_and_test.bat
goto menu

:build_benchmark
cls
echo.
echo ==========================================
echo   BUILD RELEASE AND BENCHMARK
echo ==========================================
echo.
call build_release_and_benchmark.bat
goto menu

:generate_report
cls
echo.
echo ==========================================
echo   GENERATE TEST REPORT
echo ==========================================
echo.
call test_with_report.bat
goto menu

:clean_files
cls
echo.
echo ==========================================
echo   CLEAN TEST FILES
echo ==========================================
echo.
call clean_tests.bat
goto menu

:continuous_mode
cls
echo.
echo ==========================================
echo   CONTINUOUS TEST MODE
echo ==========================================
echo.
echo This will run quick tests continuously.
echo Press Ctrl+C to stop.
echo.
pause
call continuous_test.bat
goto menu

:exit_menu
cls
echo.
echo ==========================================
echo   Exiting Test Menu
echo ==========================================
echo.
echo Thank you for using Luden Engine Test Suite!
echo.
timeout /t 2 > nul
exit /b 0