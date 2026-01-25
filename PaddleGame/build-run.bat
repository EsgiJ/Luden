@echo off
REM ============================================
REM  Build and Run Game
REM ============================================

echo.
echo ==========================================
echo     Build and Run
echo ==========================================
echo.

call build.bat Debug run

exit /b %ERRORLEVEL%