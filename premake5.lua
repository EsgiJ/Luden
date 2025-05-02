workspace "LudenEngine"
    architecture "x64"
    configurations { "Debug", "Release" }
    startproject "Editor"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- ENGINE
project "Engine"
    location "Engine"
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"
    staticruntime "on"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files {
        "%{prj.name}/**.h",
        "%{prj.name}/**.cpp"
    }

    includedirs {
        "extern/SFML/include",
        "extern/ImGui",
        "extern/sol2/include"
    }

    links {
        "sfml-graphics", "sfml-window", "sfml-system"
    }

-- EDITOR
project "Editor"
    location "Editor"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"
    staticruntime "on"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files {
        "%{prj.name}/**.h",
        "%{prj.name}/**.cpp"
    }

    includedirs {
        "Engine",
        "extern/ImGui",
        "extern/SFML/include",
        "extern/sol2/include"
    }

    links {
        "Engine",
        "sfml-graphics", "sfml-window", "sfml-system",
        "ImGui-SFML"
    }

-- GAME
project "Game"
    location "Game"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"
    staticruntime "on"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files {
        "%{prj.name}/**.h",
        "%{prj.name}/**.cpp"
    }

    includedirs {
        "Engine"
    }

    links {
        "Engine"
    }
