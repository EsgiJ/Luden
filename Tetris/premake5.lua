-- Auto-generated premake5.lua for Tetris

workspace "Tetris"
    architecture "x64"
    configurations { "Debug", "Release" }
    startproject "Tetris"

-- Engine path set by project generator
ENGINE_PATH = "C:/GameProjects/Luden"

project "Tetris"
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
