workspace "LudenEngine"
    architecture "x64"
    configurations { "Debug", "Release" }
    startproject "Editor"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

--------------------------------------------------------------------
-- ENGINE PROJECT (DLL)
--------------------------------------------------------------------
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
        "extern/ImGui-SFML/*.h",
    }

    includedirs {
        "Engine/include",
        "extern/imgui",
        "extern/ImGui-SFML",
        "extern/SFML/include",
        "extern/sol2/single",
        "extern/json/include",
        "Tools",
        "extern/nativefiledialog-extended/src/include"
    }

    filter "system:windows"
        files { "extern/nativefiledialog-extended/src/nfd_win.cpp" }
        links { "comdlg32", "ole32", "uuid", "shell32", "user32" }

    filter "system:macosx"
        files { "extern/nativefiledialog-extended/src/nfd_cocoa.m" }
        links { "AppKit.framework", "UniformTypeIdentifiers.framework" }
        buildoptions { "-fobjc-arc" }

    filter "system:linux"
        files { "extern/nativefiledialog-extended/src/nfd_gtk.c" }
        links { "gtk-3", "gobject-2.0", "glib-2.0", "gio-2.0" }

    ----------------------------------------------------------------
    -- Debug config
    ----------------------------------------------------------------
    filter { "system:windows", "configurations:Debug" }
        runtime "Debug"
        symbols "On"
        libdirs { "extern/SFML/build/lib/Debug" }
        links { "sfml-graphics-d", "sfml-window-d", "sfml-system-d", "sfml-audio-d", "opengl32" }
        postbuildcommands {
            "{COPYFILE} ../extern/SFML/build/bin/Debug/sfml-graphics-d-3.dll %{cfg.targetdir}/sfml-graphics-d-3.dll",
            "{COPYFILE} ../extern/SFML/build/bin/Debug/sfml-window-d-3.dll %{cfg.targetdir}/sfml-window-d-3.dll",
            "{COPYFILE} ../extern/SFML/build/bin/Debug/sfml-system-d-3.dll %{cfg.targetdir}/sfml-system-d-3.dll",
            "{COPYFILE} ../extern/SFML/build/bin/Debug/sfml-audio-d-3.dll %{cfg.targetdir}/sfml-audio-d-3.dll"
        }

    ----------------------------------------------------------------
    -- Release config
    ----------------------------------------------------------------
    filter { "system:windows", "configurations:Release" }
        runtime "Release"
        optimize "On"
        libdirs { "extern/SFML/build/lib/Release" }
        links { "sfml-graphics", "sfml-window", "sfml-system", "sfml-audio", "opengl32" }
        postbuildcommands {
            "{COPYFILE} ../extern/SFML/build/bin/Release/sfml-graphics-3.dll %{cfg.targetdir}/sfml-graphics-3.dll",
            "{COPYFILE} ../extern/SFML/build/bin/Release/sfml-window-3.dll %{cfg.targetdir}/sfml-window-3.dll",
            "{COPYFILE} ../extern/SFML/build/bin/Release/sfml-system-3.dll %{cfg.targetdir}/sfml-system-3.dll",
            "{COPYFILE} ../extern/SFML/build/bin/Release/sfml-audio-3.dll %{cfg.targetdir}/sfml-audio-3.dll"
        }

--------------------------------------------------------------------
-- EDITOR PROJECT
--------------------------------------------------------------------
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
        "extern/json/include",
        "extern/IconFontCppHeaders",
        "Tools"
    }

    ----------------------------------------------------------------
    -- Debug config
    ----------------------------------------------------------------
    filter { "system:windows", "configurations:Debug" }
        runtime "Debug"
        symbols "On"
        libdirs { "extern/SFML/build/lib/Debug" }
        links { "Engine", "sfml-graphics-d", "sfml-window-d", "sfml-system-d", "sfml-audio-d", "opengl32" }
        postbuildcommands {
            "{MKDIR} \"%{cfg.targetdir}\"",
            "{COPYFILE} \"..\\extern\\SFML\\build\\bin\\Debug\\sfml-graphics-d-3.dll\" \"%{cfg.targetdir}\\sfml-graphics-d-3.dll\"",
            "{COPYFILE} \"..\\extern\\SFML\\build\\bin\\Debug\\sfml-window-d-3.dll\"  \"%{cfg.targetdir}\\sfml-window-d-3.dll\"",
            "{COPYFILE} \"..\\extern\\SFML\\build\\bin\\Debug\\sfml-system-d-3.dll\"  \"%{cfg.targetdir}\\sfml-system-d-3.dll\"",
            "{COPYFILE} \"..\\extern\\SFML\\build\\bin\\Debug\\sfml-audio-d-3.dll\"   \"%{cfg.targetdir}\\sfml-audio-d-3.dll\"",
            "{COPYFILE} ../bin/%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}/Engine/Engine.pdb %{cfg.targetdir}/Engine.pdb",
            "{COPYFILE} \"..\\bin\\" .. outputdir .. "\\Engine\\Engine.dll\" \"%{cfg.targetdir}\\Engine.dll\""
        }

    ----------------------------------------------------------------
    -- Release config
    ----------------------------------------------------------------
    filter { "system:windows", "configurations:Release" }
        runtime "Release"
        optimize "On"
        libdirs { "extern/SFML/build/lib/Release" }
        links { "Engine", "sfml-graphics", "sfml-window", "sfml-system", "sfml-audio", "opengl32" }
        postbuildcommands {
            "{MKDIR} \"%{cfg.targetdir}\"",
            "{COPYFILE} \"..\\extern\\SFML\\build\\bin\\Release\\sfml-graphics-3.dll\" \"%{cfg.targetdir}\\sfml-graphics-3.dll\"",
            "{COPYFILE} \"..\\extern\\SFML\\build\\bin\\Release\\sfml-window-3.dll\"  \"%{cfg.targetdir}\\sfml-window-3.dll\"",
            "{COPYFILE} \"..\\extern\\SFML\\build\\bin\\Release\\sfml-system-3.dll\"  \"%{cfg.targetdir}\\sfml-system-3.dll\"",
            "{COPYFILE} \"..\\extern\\SFML\\build\\bin\\Release\\sfml-audio-3.dll\"   \"%{cfg.targetdir}\\sfml-audio-3.dll\"",
            "{COPYFILE} ../bin/%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}/Engine/Engine.pdb %{cfg.targetdir}/Engine.pdb",
            "{COPYFILE} \"..\\bin\\" .. outputdir .. "\\Engine\\Engine.dll\" \"%{cfg.targetdir}\\Engine.dll\""
        }
