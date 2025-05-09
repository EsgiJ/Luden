workspace "LudenEngine"
    architecture "x64"
    configurations { "Debug", "Release" }
    startproject "Editor"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- ENGINE PROJECT (DLL)
project "Engine"
    location "Engine"
    kind "SharedLib"
    language "C++"
    cppdialect "C++20"
    staticruntime "off"
    defines { "ENGINE_EXPORTS" }

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files 
    {
        "Engine/src/**.cpp",
        "Engine/include/**.h",
        "extern/imgui/*.cpp",
        "extern/ImGui-SFML/*.cpp",
        "extern/ImGui-SFML/*.h"
    }

    includedirs {
        "Engine/include",
        "extern/imgui",
        "extern/ImGui-SFML",
        "extern/SFML/include",
        "extern/sol2/single"
    }

    libdirs {
        "extern/SFML/build/lib/Release"
    }

    links {
        "sfml-graphics",
        "sfml-window",
        "sfml-system",
        "opengl32"
    }

    vpaths 
    {
        -- Engine headers
        ["Headers/Asset"]     = "Engine/include/Asset/**.h",
        ["Headers/Core"]     = "Engine/include/Core/**.h",
        ["Headers/ECS"]      = "Engine/include/ECS/**.h",
        ["Headers/Graphics"] = "Engine/include/Graphics/**.h",
        ["Headers/Input"]    = "Engine/include/Input/**.h",
        ["Headers/Math"]     = "Engine/include/Math/**.h",
        ["Headers/Utils"]    = "Engine/include/Utils/**.h",
        ["Headers/Scene"]    = "Engine/include/Scene/**.h",
        ["Headers/Physics"]    = "Engine/include/Physics/**.h",
    
        -- Engine sources
        ["Source/Asset"]      = "Engine/src/Asset/**.cpp",
        ["Source/Core"]      = "Engine/src/Core/**.cpp",
        ["Source/ECS"]      = "Engine/src/ECS/**.cpp",
        ["Source/Graphics"]  = "Engine/src/Graphics/**.cpp",
        ["Source/Input"]     = "Engine/src/Input/**.cpp",
        ["Source/Math"]      = "Engine/src/Math/**.cpp",
        ["Source/Utils"]     = "Engine/src/Utils/**.cpp",
        ["Source/Physics"]     = "Engine/src/Physics/**.cpp",

    
        -- ImGui files
        ["Extern/ImGui/Source"]     = "extern/imgui/**.cpp",
        ["Extern/ImGui/Header"]     = "extern/imgui/**.h",
    
        -- ImGui-SFML files
        ["Extern/ImGui-SFML/Source"] = "extern/ImGui-SFML/**.cpp",
        ["Extern/ImGui-SFML/Header"] = "extern/ImGui-SFML/**.h"
    }

    filter "system:windows"
    postbuildcommands {
        "{COPYFILE} ../extern/SFML/build/bin/Release/sfml-graphics-3.dll %{cfg.targetdir}/sfml-graphics-3.dll",
        "{COPYFILE} ../extern/SFML/build/bin/Release/sfml-window-3.dll %{cfg.targetdir}/sfml-window-3.dll",
        "{COPYFILE} ../extern/SFML/build/bin/Release/sfml-system-3.dll %{cfg.targetdir}/sfml-system-3.dll"
    }

-- EDITOR PROJECT
project "Editor"
    location "Editor"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"
    staticruntime "off"
    dependson { "Engine" }

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files {
        "Editor/**.h",
        "Editor/**.cpp",
        "extern/imgui/*.cpp",
        "extern/ImGui-SFML/*.cpp",
        "extern/ImGui-SFML/*.h"
    }

    includedirs {
        "Editor",
        "Engine/include",
        "extern/imgui",
        "extern/ImGui-SFML",
        "extern/SFML/include",
        "extern/sol2/single",
        "extern/Lua"
    }

    libdirs {
        "extern/SFML/build/lib/Release"
    }

    links {
        "Engine",
        "sfml-graphics",
        "sfml-window",
        "sfml-system",
        "opengl32"
    }

    postbuildcommands {
        "{COPYFILE} ../extern/SFML/build/bin/Release/sfml-graphics-3.dll %{cfg.targetdir}/sfml-graphics-3.dll",
        "{COPYFILE} ../extern/SFML/build/bin/Release/sfml-window-3.dll %{cfg.targetdir}/sfml-window-3.dll",
        "{COPYFILE} ../extern/SFML/build/bin/Release/sfml-system-3.dll %{cfg.targetdir}/sfml-system-3.dll",
        "{COPYFILE} ../bin/" .. outputdir .. "/Engine/Engine.dll %{cfg.targetdir}/Engine.dll"
    }

-- GAME PROJECT
project "Game"
    location "Game"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"
    staticruntime "off"
    dependson { "Engine" }

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files {
        "Game/**.h",
        "Game/**.cpp"
    }

    includedirs {
        "Game",
        "Engine/include"
    }

    links {
        "Engine"
    }

    postbuildcommands {
        "{COPYFILE} ../bin/" .. outputdir .. "/Engine/Engine.dll %{cfg.targetdir}/Engine.dll"
    }
