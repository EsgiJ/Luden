-- GameModule/premake5.lua
project "GameModule"
    location "."
    kind "SharedLib"
    language "C++"
    cppdialect "C++20"
    staticruntime "off"
    
    targetdir ("bin/" .. outputdir)
    objdir ("bin-int/" .. outputdir)
    
    dependson { "Engine" }
    defines { "GAME_MODULE_EXPORTS", "SFML_DYNAMIC" }

    files {
        "**.h",
        "**.cpp"
    }

    includedirs {
        "include",
        "../Engine/include",
        "../%{IncludeDirs.Box2D}",
        "../%{IncludeDirs.glm}",
        "../%{IncludeDirs.SFML}",
        "../%{IncludeDirs.json}"
    }

    links { "Engine" }

    filter { "system:windows", "configurations:Debug" }
        runtime "Debug"
        symbols "On"
        
        -- Engine'in DLL'ini bul
        libdirs { 
            "../Engine/bin/" .. outputdir,
            "../extern/SFML/build/lib/Debug",
            "../extern/Box2D/build/src/Debug"
        }
        
        links { 
            "sfml-graphics-d", "sfml-window-d", 
            "sfml-system-d", "sfml-audio-d", 
            "box2dd" 
        }

    filter { "system:windows", "configurations:Release" }
        runtime "Release"
        optimize "On"
        
        libdirs { 
            "../Engine/bin/" .. outputdir,
            "../extern/SFML/build/lib/Release",
            "../extern/Box2D/build/src/Release"
        }
        
        links { 
            "sfml-graphics", "sfml-window", 
            "sfml-system", "sfml-audio", 
            "box2d" 
        }