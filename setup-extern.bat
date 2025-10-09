@echo off
REM === Setup Extern Libraries and Generate VS Solution ===
setlocal

REM Premake5 executable path
set PREMAKE_EXE=premake5.exe

REM SFML build settings
set SFML_DIR=extern\SFML
set SFML_BUILD_DIR=%SFML_DIR%\build
set CONFIGS=Debug Release
set PLATFORM=x64

echo.
echo === Cloning/updating extern libraries ===
echo.

REM --- SFML ---
if exist "%SFML_DIR%" (
    echo %SFML_DIR% already exists, pulling latest...
    git -C "%SFML_DIR%" pull
) else (
    echo Cloning SFML...
    git clone https://github.com/SFML/SFML.git "%SFML_DIR%"
)

REM --- ImGui (v1.91.9-docking branch) ---
if exist "extern\imgui" (
    echo extern\imgui already exists, pulling latest...
    git -C "extern\imgui" pull
    git -C "extern\imgui" checkout v1.91.9-docking
) else (
    echo Cloning ImGui v1.91.9-docking branch...
    git clone -b v1.91.9-docking https://github.com/ocornut/imgui.git "extern\imgui"
)


REM --- ImGui-SFML ---
if exist "extern\ImGui-SFML" (
    echo extern\ImGui-SFML already exists, pulling latest...
    git -C "extern\ImGui-SFML" pull
) else (
    echo Cloning ImGui-SFML...
    git clone https://github.com/SFML/imgui-sfml.git "extern\ImGui-SFML"
)

REM --- Sol2 ---
if exist "extern\sol2" (
    echo extern\sol2 already exists, pulling latest...
    git -C "extern\sol2" pull
) else (
    echo Cloning Sol2...
    git clone https://github.com/eliasdaler/sol2.git "extern\sol2"
)

REM --- JSON ---
if exist "extern\json" (
    echo extern\json already exists, pulling latest...
    git -C "extern\json" pull
) else (
    echo Cloning nlohmann/json...
    git clone https://github.com/nlohmann/json.git "extern\json"
)

REM --- IconFontCppHeaders ---
if exist "extern\IconFontCppHeaders" (
    echo extern\IconFontCppHeaders already exists, pulling latest...
    git -C "extern\IconFontCppHeaders" pull
) else (
    echo Cloning IconFontCppHeaders...
    git clone https://github.com/juliettef/IconFontCppHeaders.git "extern\IconFontCppHeaders"
)

REM --- Native File Dialog ---
if exist "extern\nfd" (
    echo extern\nfd already exists, pulling latest...
    git -C "extern\nfd" pull
) else (
    echo Cloning nativefiledialog-extended...
    git clone https://github.com/mlabbe/nativefiledialog-extended.git "extern\nfd"
)

echo.
echo === Building SFML ===
if not exist "%SFML_BUILD_DIR%" mkdir "%SFML_BUILD_DIR%"
pushd "%SFML_BUILD_DIR%"

for %%C in (%CONFIGS%) do (
    echo Building %%C configuration...
    cmake .. -G "Visual Studio 17 2022" -A %PLATFORM% -DCMAKE_BUILD_TYPE=%%C -DBUILD_SHARED_LIBS=ON
    cmake --build . --config %%C
)

popd

echo.
echo === Generating Visual Studio solution with Premake ===
%PREMAKE_EXE% vs2022

if %ERRORLEVEL% neq 0 (
    echo [ERROR] Premake5 execution failed.
    pause
    exit /b %ERRORLEVEL%
)

echo.
echo Setup complete!
pause
