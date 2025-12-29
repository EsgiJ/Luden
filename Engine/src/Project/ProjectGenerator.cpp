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

        if (NativeScriptModuleGenerator::CreateNewScriptModule(projectName, projectPath))
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
            projectPath / "Resources" / "Sprites",
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

        // Get engine path (go up from Editor to Luden root)
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
        "Source/**.cpp"
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
}