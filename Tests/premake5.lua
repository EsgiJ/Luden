project "EngineTests"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"
    staticruntime "off"
    
    targetdir ("../bin/" .. outputdir)
    objdir ("../bin-int/" .. outputdir)
    
    files {
        "**.cpp",
        "**.h"
    }
    
    includedirs {
        "../Engine/include",
        ".",
        "../extern/glm",
        "../extern/Box2D/include",
        "../extern/SFML/include",
        "../extern/json/include"
    }
    
    links { "Engine" }
    dependson { "Engine" }
    
    filter "system:windows"
        systemversion "latest"
    
    filter { "system:windows", "configurations:Debug" }
        runtime "Debug"
        symbols "On"
        libdirs { 
            "../extern/SFML/build/lib/Debug",
            "../Engine/bin/" .. outputdir 
        }
        links { 
            "sfml-graphics-d", "sfml-window-d", 
            "sfml-system-d", "sfml-audio-d", 
            "opengl32"
        }
        postbuildcommands {
            "{MKDIR} ../bin/" .. outputdir,
            "{COPYFILE} ../Engine/bin/" .. outputdir .. "/Engine.dll %{cfg.targetdir}/Engine.dll",
            "{COPYFILE} ../Engine/bin/" .. outputdir .. "/Engine.pdb %{cfg.targetdir}/Engine.pdb",
            "{COPYFILE} ../extern/SFML/build/bin/Debug/sfml-graphics-d-3.dll %{cfg.targetdir}/sfml-graphics-d-3.dll",
            "{COPYFILE} ../extern/SFML/build/bin/Debug/sfml-window-d-3.dll %{cfg.targetdir}/sfml-window-d-3.dll",
            "{COPYFILE} ../extern/SFML/build/bin/Debug/sfml-system-d-3.dll %{cfg.targetdir}/sfml-system-d-3.dll",
            "{COPYFILE} ../extern/SFML/build/bin/Debug/sfml-audio-d-3.dll %{cfg.targetdir}/sfml-audio-d-3.dll"
        }
    
    filter { "system:windows", "configurations:Release" }
        runtime "Release"
        optimize "On"
        libdirs { 
            "../extern/SFML/build/lib/Release",
            "../Engine/bin/" .. outputdir
        }
        links { 
            "sfml-graphics", "sfml-window", 
            "sfml-system", "sfml-audio", 
            "opengl32"
        }
        postbuildcommands {
            "{MKDIR} ../bin/" .. outputdir,
            "{COPYFILE} ../Engine/bin/" .. outputdir .. "/Engine.dll %{cfg.targetdir}/Engine.dll",
            "{COPYFILE} ../extern/SFML/build/bin/Release/sfml-graphics-3.dll %{cfg.targetdir}/sfml-graphics-3.dll",
            "{COPYFILE} ../extern/SFML/build/bin/Release/sfml-window-3.dll %{cfg.targetdir}/sfml-window-3.dll",
            "{COPYFILE} ../extern/SFML/build/bin/Release/sfml-system-3.dll %{cfg.targetdir}/sfml-system-3.dll",
            "{COPYFILE} ../extern/SFML/build/bin/Release/sfml-audio-3.dll %{cfg.targetdir}/sfml-audio-3.dll"
        }