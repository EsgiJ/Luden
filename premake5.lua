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
        "Engine/src/Generated/**.cpp",
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
        "extern/sol2/single",
        "extern/json/include"
    }

    libdirs {
        "extern/SFML/build/lib/Release"
    }

    links {
        "sfml-graphics",
        "sfml-window",
        "sfml-system",
        "sfml-audio",
        "opengl32"
    }

    vpaths 
    {
        -- Engine headers
        ["Headers/Asset"]       = "Engine/include/Asset/**.h",
        ["Headers/Core"]        = "Engine/include/Core/**.h",
        ["Headers/ECS"]         = "Engine/include/ECS/**.h",
        ["Headers/Graphics"]    = "Engine/include/Graphics/**.h",
        ["Headers/Input"]       = "Engine/include/Input/**.h",
        ["Headers/Math"]        = "Engine/include/Math/**.h",
        ["Headers/Utils"]       = "Engine/include/Utils/**.h",
        ["Headers/Scene"]       = "Engine/include/Scene/**.h",
        ["Headers/Physics"]     = "Engine/include/Physics/**.h",
        ["Headers/Reflection"]  = "Engine/include/Reflection/**.h",

    
        -- Engine sources
        ["Source/Asset"]        = "Engine/src/Asset/**.cpp",
        ["Source/Core"]         = "Engine/src/Core/**.cpp",
        ["Source/ECS"]          = "Engine/src/ECS/**.cpp",
        ["Source/Graphics"]     = "Engine/src/Graphics/**.cpp",
        ["Source/Input"]        = "Engine/src/Input/**.cpp",
        ["Source/Math"]         = "Engine/src/Math/**.cpp",
        ["Source/Utils"]        = "Engine/src/Utils/**.cpp",
        ["Source/Scene"]        = "Engine/src/Scene/**.cpp",
        ["Source/Physics"]      = "Engine/src/Physics/**.cpp",
        ["Source/Generated"]      = "Engine/src/Generated/**.gen.cpp",

    
        -- ImGui files
        ["Extern/ImGui/Source"] = "extern/imgui/**.cpp",
        ["Extern/ImGui/Header"] = "extern/imgui/**.h",
    
        -- ImGui-SFML files
        ["Extern/ImGui-SFML/Source"] = "extern/ImGui-SFML/**.cpp",
        ["Extern/ImGui-SFML/Header"] = "extern/ImGui-SFML/**.h"
    } 

    filter "system:windows"
    buildoptions { "/bigobj" }
    prebuildcommands {
        --"echo Generating RTTR Version Header...",
        --"python ../Tools/RTTRVersionHeaderGenerator/generate_version_header.py",
        --"echo Generating Reflection Metadata...",
        --"python ../Tools/ReflectionGenerator/reflection_generator.py"
    }

    filter "system:linux or system:macosx"
    prebuildcommands {
        --"echo Generating RTTR Version Header...",
        --"python ../Tools/RTTRVersionHeaderGenerator/generate_version_header.py",
        --"echo Generating Reflection Metadata...",
        --"python ../Tools/ReflectionGenerator/reflection_generator.py"
    }

    filter "system:windows"
    postbuildcommands {
        "{COPYFILE} ../extern/SFML/build/bin/Release/sfml-graphics-3.dll %{cfg.targetdir}/sfml-graphics-3.dll",
        "{COPYFILE} ../extern/SFML/build/bin/Release/sfml-window-3.dll %{cfg.targetdir}/sfml-window-3.dll",
        "{COPYFILE} ../extern/SFML/build/bin/Release/sfml-system-3.dll %{cfg.targetdir}/sfml-system-3.dll",
        "{COPYFILE} ../extern/SFML/build/bin/Release/sfml-audio-3.dll %{cfg.targetdir}/sfml-audio-3.dll"
    }

-- EDITOR PROJECT
project "Editor"
    location "Editor"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"
    staticruntime "off"
    dependson { "Engine" }
    defines { "EDITOR_EXPORTS" }

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
        "Editor/include",
        "Engine/include",
        "extern/imgui",
        "extern/ImGui-SFML",
        "extern/SFML/include",
        "extern/sol2/single",
        "extern/Lua",
        "extern/rttr/src",
        "extern/json/include",

    }
    
    libdirs {
        "extern/SFML/build/lib/Release"
    }

    links {
        "Engine",
        "sfml-graphics",
        "sfml-window",
        "sfml-system",
        "sfml-audio",
        "opengl32"
    }

        vpaths 
    {
        -- Engine headers
        ["Headers/Core"]        = "Editor/include/Core/**.h",
        ["Headers/GUI"]         = "Editor/include/GUI/**.h",
        ["Headers/Panels"]      = "Editor/include/Panels/**.h",
        ["Headers/Utils"]       = "Editor/include/Utils/**.h",

        -- Engine sources
        ["Source/Core"]         = "Editor/src/Core/**.cpp",
        ["Source/GUI"]          = "Editor/src/GUI/**.cpp",
        ["Source/Panels"]       = "Editor/src/Panels/**.cpp",
        ["Source/Utils"]        = "Editor/src/Utils/**.cpp",

        -- ImGui files
        ["Extern/ImGui/Source"] = "extern/imgui/**.cpp",
        ["Extern/ImGui/Header"] = "extern/imgui/**.h",
    
        -- ImGui-SFML files
        ["Extern/ImGui-SFML/Source"] = "extern/ImGui-SFML/**.cpp",
        ["Extern/ImGui-SFML/Header"] = "extern/ImGui-SFML/**.h"
    } 

    filter "system:windows"
    prebuildcommands {
        --"echo Generating RTTR Version Header...",
        --"python ../Tools/RTTRVersionHeaderGenerator/generate_version_header.py",
        --"echo Generating Reflection Metadata...",
        --"python ../Tools/ReflectionGenerator/reflection_generator.py"
    }

    filter "system:linux or system:macosx"
    prebuildcommands {
        --"echo Generating RTTR Version Header...",
        --"python ../Tools/RTTRVersionHeaderGenerator/generate_version_header.py",
        --"echo Generating Reflection Metadata...",
        --"python ../Tools/ReflectionGenerator/reflection_generator.py"
    }

filter "system:windows"
  postbuildcommands {
    "{MKDIR} \"%{cfg.targetdir}\"",

    -- copy SFML DLLs
    "{COPYFILE} \"..\\extern\\SFML\\build\\bin\\Release\\sfml-graphics-3.dll\" \"%{cfg.targetdir}\\sfml-graphics-3.dll\"",
    "{COPYFILE} \"..\\extern\\SFML\\build\\bin\\Release\\sfml-window-3.dll\"  \"%{cfg.targetdir}\\sfml-window-3.dll\"",
    "{COPYFILE} \"..\\extern\\SFML\\build\\bin\\Release\\sfml-system-3.dll\"  \"%{cfg.targetdir}\\sfml-system-3.dll\"",
    "{COPYFILE} \"..\\extern\\SFML\\build\\bin\\Release\\sfml-audio-3.dll\"   \"%{cfg.targetdir}\\sfml-audio-3.dll\"",

    "{COPYFILE} \"..\\bin\\" .. outputdir .. "\\Engine\\Engine.dll\" \"%{cfg.targetdir}\\Engine.dll\""
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

project "ReflectionGenerator"
    location "Tools/ReflectionGenerator"
    kind "Utility"
    language "C++"
    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files {
        "Tools/ReflectionGenerator/**.py"
    }

project "RTTRVersionHeaderGenerator"
    location "Tools/RTTRVersionHeaderGenerator"
    kind "Utility"
    language "C++"
    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files {
        "Tools/RTTRVersionHeaderGenerator/**.py"
    }    