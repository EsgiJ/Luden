@echo off
REM ============================================
REM  Build and Run Game
REM ============================================

echo.
echo Building and running game...
echo.

call build.bat Debug run

exit /b %ERRORLEVEL%
