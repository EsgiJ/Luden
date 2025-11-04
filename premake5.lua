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
    defines { "ENGINE_EXPORTS", "SFML_DYNAMIC" }

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files {
        "Engine/src/**.cpp",
        "Engine/src/Generated/**.cpp",
        "Engine/include/**.h",
        "extern/imgui/*.cpp",
        "extern/ImGui-SFML/*.cpp",
        "extern/ImGui-SFML/*.h"
    }

    includedirs {
        "Engine/include",
        "extern/Box2D/include",
        "extern/imgui",
        "extern/ImGui-SFML",
        "extern/glm",
        "extern/SFML/include", 
        "extern/json/include",
        "extern/IconFontCppHeaders",
        "extern/nativefiledialog-extended/src/include",
        "Tools"
    }

    filter "system:windows"
        files { "extern/nativefiledialog-extended/src/nfd_win.cpp" }
        links {"comdlg32", "ole32", "uuid", "shell32", "user32"}

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
        libdirs { "extern/SFML/build/lib/Debug", "extern/Box2D/build/src/Debug" }
        links { "sfml-graphics-d", "sfml-window-d", "sfml-system-d", "sfml-audio-d", "opengl32", "box2dd" }
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
        libdirs { "extern/SFML/build/lib/Release", "extern/Box2D/build/src/Release" }
        links { "sfml-graphics", "sfml-window", "sfml-system", "sfml-audio", "opengl32", "box2d" }
        postbuildcommands {
            "{COPYFILE} ../extern/SFML/build/bin/Release/sfml-graphics-3.dll %{cfg.targetdir}/sfml-graphics-3.dll",
            "{COPYFILE} ../extern/SFML/build/bin/Release/sfml-window-3.dll %{cfg.targetdir}/sfml-window-3.dll",
            "{COPYFILE} ../extern/SFML/build/bin/Release/sfml-system-3.dll %{cfg.targetdir}/sfml-system-3.dll",
            "{COPYFILE} ../extern/SFML/build/bin/Release/sfml-audio-3.dll %{cfg.targetdir}/sfml-audio-3.dll"
        }

--------------------------------------------------------------------
-- GAME MODULE PROJECT (DLL - Hot Reloadable)
--------------------------------------------------------------------
project "GameModule"
    location "GameModule"
    kind "SharedLib"
    language "C++"
    cppdialect "C++20"
    staticruntime "off"
    dependson { "Engine" }
    defines { "GAME_MODULE_EXPORTS", "SFML_DYNAMIC" }

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files {
        "GameModule/**.h",
        "GameModule/**.cpp"
    }

    includedirs {
        "GameModule/include",
        "Engine/include",
        "extern/Box2D/include",
        "extern/glm",
        "extern/SFML/include",
        "extern/json/include"
    }

    links { "Engine" }

    ----------------------------------------------------------------
    -- Debug config
    ----------------------------------------------------------------
    filter { "system:windows", "configurations:Debug" }
        runtime "Debug"
        symbols "On"
        libdirs { 
            "bin/" .. outputdir .. "/Engine",
            "extern/SFML/build/lib/Debug",
            "extern/Box2D/build/src/Debug"
        }
        links { "sfml-graphics-d", "sfml-window-d", "sfml-system-d", "sfml-audio-d", "box2dd" }
        
        postbuildcommands {
            "{COPYFILE} %{cfg.targetdir}/GameModule.dll ../bin/" .. outputdir .. "/Editor/GameModule.dll",
            "{COPYFILE} %{cfg.targetdir}/GameModule.pdb ../bin/" .. outputdir .. "/Editor/GameModule.pdb"
        }

    ----------------------------------------------------------------
    -- Release config
    ----------------------------------------------------------------
    filter { "system:windows", "configurations:Release" }
        runtime "Release"
        optimize "On"
        libdirs { 
            "bin/" .. outputdir .. "/Engine",
            "extern/SFML/build/lib/Release",
            "extern/Box2D/build/src/Release"
        }
        links { "sfml-graphics", "sfml-window", "sfml-system", "sfml-audio", "box2d" }
        
        postbuildcommands {
            "{COPYFILE} %{cfg.targetdir}/GameModule.dll ../bin/" .. outputdir .. "/Editor/GameModule.dll",
            "{COPYFILE} %{cfg.targetdir}/GameModule.pdb ../bin/" .. outputdir .. "/Editor/GameModule.pdb"
        }

--------------------------------------------------------------------
-- EDITOR PROJECT (EXE)
--------------------------------------------------------------------
project "Editor"
    location "Editor"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"
    staticruntime "off"
    dependson { "Engine", "GameModule" } 
    defines { "EDITOR_EXPORTS", "SFML_DYNAMIC" }

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
        "extern/Box2D/include",
        "extern/imgui",            
        "extern/ImGui-SFML",    
        "extern/glm",  
        "extern/SFML/include",    
        "extern/json/include",
        "extern/IconFontCppHeaders",
        "Tools"
    }

    -- Only link the engine, game module will be linked on runtime
    links { "Engine" }

    ----------------------------------------------------------------
    -- Debug config
    ----------------------------------------------------------------
    filter { "system:windows", "configurations:Debug" }
        runtime "Debug"
        symbols "On"
        libdirs { "extern/SFML/build/lib/Debug" }
        links { "sfml-graphics-d", "sfml-window-d", "sfml-system-d", "sfml-audio-d", "opengl32" }
        postbuildcommands {
            "{MKDIR} \"%{cfg.targetdir}\"",
            -- SFML DLL
            "{COPYFILE} \"..\\extern\\SFML\\build\\bin\\Debug\\sfml-graphics-d-3.dll\" \"%{cfg.targetdir}\\sfml-graphics-d-3.dll\"",
            "{COPYFILE} \"..\\extern\\SFML\\build\\bin\\Debug\\sfml-window-d-3.dll\"  \"%{cfg.targetdir}\\sfml-window-d-3.dll\"",
            "{COPYFILE} \"..\\extern\\SFML\\build\\bin\\Debug\\sfml-system-d-3.dll\"  \"%{cfg.targetdir}\\sfml-system-d-3.dll\"",
            "{COPYFILE} \"..\\extern\\SFML\\build\\bin\\Debug\\sfml-audio-d-3.dll\"   \"%{cfg.targetdir}\\sfml-audio-d-3.dll\"",
            -- Engine DLL
            "{COPYFILE} \"..\\bin\\" .. outputdir .. "\\Engine\\Engine.dll\" \"%{cfg.targetdir}\\Engine.dll\"",
            "{COPYFILE} \"..\\bin\\" .. outputdir .. "\\Engine\\Engine.pdb\" \"%{cfg.targetdir}\\Engine.pdb\"",
            -- GameModule DLL (copy for the first time, then copy for hot reload)
            "{COPYFILE} \"..\\bin\\" .. outputdir .. "\\GameModule\\GameModule.dll\" \"%{cfg.targetdir}\\GameModule.dll\"",
            "{COPYFILE} \"..\\bin\\" .. outputdir .. "\\GameModule\\GameModule.pdb\" \"%{cfg.targetdir}\\GameModule.pdb\""
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
            "{MKDIR} \"%{cfg.targetdir}\"",
            -- SFML DLL'leri
            "{COPYFILE} \"..\\extern\\SFML\\build\\bin\\Release\\sfml-graphics-3.dll\" \"%{cfg.targetdir}\\sfml-graphics-3.dll\"",
            "{COPYFILE} \"..\\extern\\SFML\\build\\bin\\Release\\sfml-window-3.dll\"  \"%{cfg.targetdir}\\sfml-window-3.dll\"",
            "{COPYFILE} \"..\\extern\\SFML\\build\\bin\\Release\\sfml-system-3.dll\"  \"%{cfg.targetdir}\\sfml-system-3.dll\"",
            "{COPYFILE} \"..\\extern\\SFML\\build\\bin\\Release\\sfml-audio-3.dll\"   \"%{cfg.targetdir}\\sfml-audio-3.dll\"",
            -- Engine DLL
            "{COPYFILE} \"..\\bin\\" .. outputdir .. "\\Engine\\Engine.dll\" \"%{cfg.targetdir}\\Engine.dll\"",
            "{COPYFILE} \"..\\bin\\" .. outputdir .. "\\Engine\\Engine.pdb\" \"%{cfg.targetdir}\\Engine.pdb\"",
            -- GameModule DLL
            "{COPYFILE} \"..\\bin\\" .. outputdir .. "\\GameModule\\GameModule.dll\" \"%{cfg.targetdir}\\GameModule.dll\"",
            "{COPYFILE} \"..\\bin\\" .. outputdir .. "\\GameModule\\GameModule.pdb\" \"%{cfg.targetdir}\\GameModule.pdb\""
        }

