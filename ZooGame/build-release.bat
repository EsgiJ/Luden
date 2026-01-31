@echo off
REM ============================================
REM  Build Release Version
REM ============================================

echo.
echo Building RELEASE version...
echo.

call build.bat Release

exit /b %ERRORLEVEL%
