-- Editor/premake5.lua
project "Editor"
    location "."
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"
    staticruntime "off"
    
    targetdir ("bin/" .. outputdir)
    objdir ("bin-int/" .. outputdir)
    
    dependson { "Engine", "GameModule" }
    defines { "EDITOR_EXPORTS", "SFML_DYNAMIC" }

    disablewarnings { "4251" }

    files {
        "**.h",
        "**.cpp",
        "../extern/imgui/*.cpp",
        "../extern/ImGui-SFML/*.cpp",
        "../extern/ImGui-SFML/*.h"
    }

    includedirs {
        "include",
        "../Engine/include",
        "../%{IncludeDirs.Box2D}",
        "../%{IncludeDirs.imgui}",
        "../%{IncludeDirs.ImGuiSFML}",
        "../%{IncludeDirs.glm}",
        "../%{IncludeDirs.SFML}",
        "../%{IncludeDirs.json}",
        "../%{IncludeDirs.IconFontCpp}",
        "../Tools"
    }

    -- Only link Engine (GameModule loaded at runtime)
    links { "Engine" }

    filter { "system:windows", "configurations:Debug" }
        runtime "Debug"
        symbols "On"
        
        libdirs { 
            "../Engine/bin/" .. outputdir,
            "../extern/SFML/build/lib/Debug"
        }
        
        links { 
            "sfml-graphics-d", "sfml-window-d", 
            "sfml-system-d", "sfml-audio-d", 
            "opengl32" 
        }
        
        postbuildcommands {
            "{MKDIR} \"%{cfg.targetdir}\"",
            
            -- SFML DLLs
            "{COPYFILE} \"../extern/SFML/build/bin/Debug/sfml-graphics-d-3.dll\" \"%{cfg.targetdir}\"",
            "{COPYFILE} \"../extern/SFML/build/bin/Debug/sfml-window-d-3.dll\" \"%{cfg.targetdir}\"",
            "{COPYFILE} \"../extern/SFML/build/bin/Debug/sfml-system-d-3.dll\" \"%{cfg.targetdir}\"",
            "{COPYFILE} \"../extern/SFML/build/bin/Debug/sfml-audio-d-3.dll\" \"%{cfg.targetdir}\"",
            
            -- Engine DLL
            "{COPYFILE} \"../Engine/bin/" .. outputdir .. "/Engine.dll\" \"%{cfg.targetdir}\"",
            "{COPYFILE} \"../Engine/bin/" .. outputdir .. "/Engine.pdb\" \"%{cfg.targetdir}\"",
            
            -- GameModule DLL (initial copy)
            "{COPYFILE} \"../GameModule/bin/" .. outputdir .. "/GameModule.dll\" \"%{cfg.targetdir}\"",
            "{COPYFILE} \"../GameModule/bin/" .. outputdir .. "/GameModule.pdb\" \"%{cfg.targetdir}\""
        }

    filter { "system:windows", "configurations:Release" }
        runtime "Release"
        optimize "On"
        -- Similar to Debug...