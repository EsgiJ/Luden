#include "Project/ProjectGenerator.h"
#include "IO/FileSystem.h"
#include "Scene/Scene.h"
#include "Scene/SceneSerializer.h"
#include "Physics2D/CollisionChannelRegistry.h"
#include "NativeScript/NativeScriptModuleGenerator.h"

#include <fstream>
#include <iostream>

namespace Luden
{
    bool ProjectGenerator::CreateProject(const std::string& projectName, const std::filesystem::path& path)
    {
        std::filesystem::path projectPath = path / projectName;

        if (!CreateDirectoryStructure(projectPath))
            return false;

        if (!CreateProjectFile(projectPath, projectName))
            return false;

        if (!CreatePremakeFile(projectPath, projectName))
            return false;

        if (!CreateGenerateBat(projectPath))
            return false;

		if (!CopyPremakeExecutable(projectPath))
			return false;

        std::filesystem::path resourcePath = projectPath / "Resources";

        if (!CreateDefaultScene(resourcePath))
            return false;

        if (!CreateCollisionChannels(resourcePath))
            return false;

        if (!CreateResourceRegistry(resourcePath))
            return false;

        if (!NativeScriptModuleGenerator::CreateNewScriptModule(projectName, projectPath))
            return false;

		std::filesystem::path buildScriptPath = projectPath / "build.bat";
		if (!CreateBuildScript(buildScriptPath, projectName))
			return false;

		std::filesystem::path buildReleasePath = projectPath / "build-release.bat";
		if (!CreateBuildReleaseScript(buildReleasePath))
			return false;

		std::filesystem::path buildAndRunPath = projectPath / "build-and-run.bat";
		if (!CreateBuildAndRunScript(buildAndRunPath))
			return false;

		std::filesystem::path cleanPath = projectPath / "clean.bat";
		if (!CreateCleanScript(cleanPath))
			return false;

        return true;
    }

    bool ProjectGenerator::CreateDirectoryStructure(const std::filesystem::path& projectPath)
    {
        std::vector<std::filesystem::path> directories = {
            projectPath,
            projectPath / "Resources",
            projectPath / "Resources" / "Animations",
            projectPath / "Resources" / "Fonts",
            projectPath / "Resources" / "Images",
            projectPath / "Resources" / "Scenes",
            projectPath / "Resources" / "Scripts",
            projectPath / "Resources" / "Sounds",
			projectPath / "Resources" / "Musics",
            projectPath / "Resources" / "Sprites",
			projectPath / "Resources" / "Prefabs",
            projectPath / "Source",
            projectPath / "Source" / "Scripts"
        };

        for (const auto& dir : directories)
        {
            if (!FileSystem::CreateDir(dir))
                return false;
        }

        return true;
    }

    bool ProjectGenerator::CreateProjectFile(const std::filesystem::path& projectPath, const std::string& name)
    {
        std::filesystem::path projectFile = projectPath / (name + ".lproject");

        std::ofstream out(projectFile);
        if (!out.is_open())
            return false;

        nlohmann::json jProject;
        jProject["Name"] = name;
        jProject["ProjectFileName"] = name + ".lproject";
        jProject["ProjectDirectory"] = projectPath.string();
        jProject["ResourceDirectory"] = "Resources";
        jProject["ResourceRegistry"] = "Resources/ResourceRegistry.lr";
        jProject["StartScene"] = "Resources/Scenes/DefaultScene.lscene";

        out << jProject.dump(4);
        return true;
    }

    bool ProjectGenerator::CreatePremakeFile(const std::filesystem::path& projectPath, const std::string& name)
    {
        std::filesystem::path premakeFile = projectPath / "premake5.lua";

        std::ofstream out(premakeFile);
        if (!out.is_open())
            return false;

        std::filesystem::path enginePath = std::filesystem::current_path().parent_path();
        std::string enginePathStr = enginePath.string();
        std::replace(enginePathStr.begin(), enginePathStr.end(), '\\', '/');

        out << R"(-- Auto-generated premake5.lua for )" << name << R"(

workspace ")" << name << R"("
    architecture "x64"
    configurations { "Debug", "Release" }
    startproject ")" << name << R"("

-- Engine path set by project generator
ENGINE_PATH = ")" << enginePathStr << R"("

project ")" << name << R"("
    location "."
    kind "SharedLib"
    language "C++"
    cppdialect "C++20"
    staticruntime "off"

    targetdir ("bin/%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}")
    objdir ("bin-int/%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}")

    dependson { "Engine" }

    defines { 
        "GAME_MODULE_EXPORTS",
        "SFML_DYNAMIC"
    }

    files {
        "Source/**.h",
        "Source/**.cpp",
        "**.h",
        "**.cpp"
    }

    -- Engine and external library includes
    includedirs {
        "Source",
        ENGINE_PATH .. "/Engine/include",
        ENGINE_PATH .. "/extern/Box2D/include",
        ENGINE_PATH .. "/extern/glm",
        ENGINE_PATH .. "/extern/SFML/include",
        ENGINE_PATH .. "/extern/json/include"
    }

    links { 
        "Engine"
    }

    filter "configurations:Debug"
        runtime "Debug"
        symbols "On"
        defines { "DEBUG" }
        
        -- Fix PDB locking during hot reload
        flags { "NoIncrementalLink" }
        editandcontinue "Off"
        
        linkoptions { 
            "/PDBALTPATH:%_PDB%",
            "/DEBUG:FASTLINK"
        }
        
        libdirs { 
            ENGINE_PATH .. "/Engine/bin/Debug-windows-x86_64",
            ENGINE_PATH .. "/extern/SFML/build/lib/Debug"
        }

        links { 
            "sfml-graphics-d", 
            "sfml-window-d", 
            "sfml-system-d", 
            "sfml-audio-d"
        }

    filter "configurations:Release"
        runtime "Release"
        optimize "On"
        defines { "NDEBUG" }
        
        libdirs { 
            ENGINE_PATH .. "/Engine/bin/Release-windows-x86_64",
            ENGINE_PATH .. "/extern/SFML/build/lib/Release"
        }

        links { 
            "sfml-graphics", 
            "sfml-window", 
            "sfml-system", 
            "sfml-audio"
        }
)";

        return true;
    }

	bool ProjectGenerator::CreateGenerateBat(const std::filesystem::path& projectPath)
	{
		std::filesystem::path batFile = projectPath / "generate.bat";

		std::ofstream out(batFile);
		if (!out.is_open())
			return false;

		std::filesystem::path enginePath = std::filesystem::current_path().parent_path();
		std::string enginePathStr = enginePath.string();

		out << R"(@echo off
REM === Generate Visual Studio 2022 Solution ===
echo Generating Visual Studio 2022 solution for project...

REM Set LUDEN_ENGINE_PATH for this session
set LUDEN_ENGINE_PATH=)" << enginePathStr << R"(

echo Using Engine Path: %LUDEN_ENGINE_PATH%

REM Run premake from project directory
premake5.exe vs2022

if %ERRORLEVEL% neq 0 (
    echo [ERROR] Premake5 execution failed.
    pause
    exit /b %ERRORLEVEL%
)

echo Done! Solution generated successfully.
pause
)";

		return true;
	}

	bool ProjectGenerator::CopyPremakeExecutable(const std::filesystem::path& projectPath)
	{
		std::filesystem::path sourcePremake = "premake5.exe";
		std::filesystem::path targetPremake = projectPath / "premake5.exe";

		if (!FileSystem::Exists(sourcePremake))
		{
			std::cerr << "Warning: premake5.exe not found in editor directory!" << std::endl;
			return false;
		}

		return FileSystem::Copy(sourcePremake, targetPremake);
	}

    bool ProjectGenerator::CreateDefaultScene(const std::filesystem::path& resourcePath)
    {
        std::filesystem::path scenePath = resourcePath / "Scenes" / "DefaultScene.lscene";

        auto defaultScene = std::make_shared<Scene>("Default Scene");

        SceneSerializer serializer(defaultScene);
        return serializer.Serialize(scenePath);
    }

    bool ProjectGenerator::CreateCollisionChannels(const std::filesystem::path& resourcePath)
    {
        std::filesystem::path channelsPath = resourcePath / "CollisionChannels.dat";

        CollisionChannelRegistry::Instance().ResetToDefaults();
        return CollisionChannelRegistry::Instance().Serialize(channelsPath);
    }

    bool ProjectGenerator::CreateResourceRegistry(const std::filesystem::path& resourcePath)
    {
        std::filesystem::path registryPath = resourcePath / "ResourceRegistry.lr";

        std::ofstream out(registryPath);
        if (!out.is_open())
            return false;

        nlohmann::json jRegistry;
        jRegistry["Resources"] = nlohmann::json::array();

        out << jRegistry.dump(4);
        return true;
    }

	bool ProjectGenerator::CreateBuildScript(const std::filesystem::path& path, const std::string& projectName)
	{
		std::ofstream file(path);
		if (!file.is_open())
		{
			std::cerr << "Failed to create build.bat" << std::endl;
			return false;
		}

		std::filesystem::path engineDir = std::filesystem::current_path().parent_path();
		std::string enginePath = engineDir.string();
		std::replace(enginePath.begin(), enginePath.end(), '/', '\\');

		file << R"(@echo off
setlocal enabledelayedexpansion

REM ============================================
REM  Luden Engine - Project Build Script
REM ============================================

echo.
echo ==========================================
echo     Luden Engine - Project Builder
echo ==========================================
echo.

set "PROJECT_DIR=%~dp0"
cd /d "%PROJECT_DIR%"
set "PROJECT_NAME=)" << projectName << R"("

echo Project: %PROJECT_NAME%
echo.

set "BUILD_CONFIG=%1"
if "%BUILD_CONFIG%"=="" set "BUILD_CONFIG=Debug"

echo Build Configuration: %BUILD_CONFIG%
echo.

REM Dynamic paths
set "GAME_DLL_DIR=%PROJECT_DIR%bin\%BUILD_CONFIG%-windows-x86_64"
set "ENGINE_DIR=)" << enginePath << R"("
set "RUNTIME_DIR=%ENGINE_DIR%\Runtime\bin\%BUILD_CONFIG%-windows-x86_64"
set "RESOURCES_DIR=%PROJECT_DIR%Resources"
set "BUILD_DIR=%PROJECT_DIR%Build"

echo Engine Directory: %ENGINE_DIR%
echo Runtime Directory: %RUNTIME_DIR%
echo.

echo [1/6] Preparing build directory...
if exist "%BUILD_DIR%" rd /s /q "%BUILD_DIR%"
mkdir "%BUILD_DIR%"
echo [OK] Build directory created
echo.

echo [2/6] Copying game module...
if not exist "%GAME_DLL_DIR%\%PROJECT_NAME%.dll" (
    echo [ERROR] Game DLL not found!
    echo Path: %GAME_DLL_DIR%\%PROJECT_NAME%.dll
    echo Please build your project first in Visual Studio!
    pause
    exit /b 1
)
copy /y "%GAME_DLL_DIR%\%PROJECT_NAME%.dll" "%BUILD_DIR%\" > nul
echo [OK] %PROJECT_NAME%.dll copied
echo.

echo [3/6] Copying Runtime files...
if not exist "%RUNTIME_DIR%\Runtime.exe" (
    echo [ERROR] Runtime.exe not found!
    echo Path: %RUNTIME_DIR%\Runtime.exe
    echo.
    echo Please ensure Runtime project is built!
    pause
    exit /b 1
)
copy /y "%RUNTIME_DIR%\Runtime.exe" "%BUILD_DIR%\" > nul
copy /y "%RUNTIME_DIR%\Engine.dll" "%BUILD_DIR%\" > nul
echo [OK] Runtime files copied
echo.

echo [4/6] Copying dependencies...
copy /y "%RUNTIME_DIR%\*.dll" "%BUILD_DIR%\" > nul 2>&1
echo [OK] Dependencies copied
echo.

echo [5/6] Copying project file...
copy /y "%PROJECT_DIR%%PROJECT_NAME%.lproject" "%BUILD_DIR%\" > nul
echo [OK] Project file copied
echo.

echo [6/6] Copying resources...
if exist "%RESOURCES_DIR%" (
    xcopy /e /i /y /q "%RESOURCES_DIR%" "%BUILD_DIR%\Resources\" > nul
    echo [OK] Resources copied
) else (
    echo [WARNING] Resources folder not found
)
echo.

echo ==========================================
echo     BUILD SUCCESSFUL!
echo ==========================================
echo Build location: %BUILD_DIR%
echo.

if "%2"=="run" (
    echo ==========================================
    echo     Launching Game
    echo ==========================================
    echo.
    cd /d "%BUILD_DIR%"
    
    Runtime.exe
    
    echo.
    echo ==========================================
    echo     Game Closed - Exit Code: %ERRORLEVEL%
    echo ==========================================
    
    if %ERRORLEVEL% neq 0 (
        echo [ERROR] Game exited with error code: %ERRORLEVEL%
        echo.
    )
    
    pause
)

if "%2" NEQ "run" (
    pause
)

exit /b 0
)";

		file.close();
		std::cout << "Created build.bat" << std::endl;
		return true;
	}

	bool ProjectGenerator::CreateBuildReleaseScript(const std::filesystem::path& path)
	{
		std::ofstream file(path);
		if (!file.is_open())
		{
			std::cerr << "Failed to create build-release.bat" << std::endl;
			return false;
		}

		file << R"(@echo off
REM ============================================
REM  Build Release Version
REM ============================================

echo.
echo Building RELEASE version...
echo.

call build.bat Release

exit /b %ERRORLEVEL%
)";

		file.close();
		std::cout << "Created build-release.bat" << std::endl;
		return true;
	}

	bool ProjectGenerator::CreateBuildAndRunScript(const std::filesystem::path& path)
	{
		std::ofstream file(path);
		if (!file.is_open())
		{
			std::cerr << "Failed to create build-and-run.bat" << std::endl;
			return false;
		}

		file << R"(@echo off
REM ============================================
REM  Build and Run Game
REM ============================================

echo.
echo Building and running game...
echo.

call build.bat Debug run

exit /b %ERRORLEVEL%
)";

		file.close();
		std::cout << "Created build-and-run.bat" << std::endl;
		return true;
	}

	bool ProjectGenerator::CreateCleanScript(const std::filesystem::path& path)
	{
		std::ofstream file(path);
		if (!file.is_open())
		{
			std::cerr << "Failed to create clean.bat" << std::endl;
			return false;
		}

		file << R"(@echo off
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
)";

		file.close();
		std::cout << "Created clean.bat" << std::endl;
		return true;
	}
}