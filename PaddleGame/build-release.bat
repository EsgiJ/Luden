@echo off
REM ============================================
REM  Build Release Version
REM ============================================

echo.
echo ==========================================
echo     Building RELEASE Version
echo ==========================================
echo.

call build.bat Release

exit /b %ERRORLEVEL%