@echo off
REM ============================================
REM  Clean Build Directory
REM ============================================

echo.
echo ==========================================
echo     Cleaning Build Directory
echo ==========================================
echo.

set "PROJECT_DIR=%~dp0"
set "BUILD_DIR=%PROJECT_DIR%Build"

if exist "%BUILD_DIR%" (
    echo Removing build directory...
    rd /s /q "%BUILD_DIR%"
    echo [OK] Build directory cleaned: %BUILD_DIR%
) else (
    echo [INFO] No build directory found.
)

echo.
echo ==========================================
echo     Clean Complete
echo ==========================================
echo.

pause
