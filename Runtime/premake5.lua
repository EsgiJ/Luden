project "Runtime"
    location "."
    kind "ConsoleApp" 
    language "C++"
    cppdialect "C++20"
    staticruntime "off"
    
    targetdir ("bin/" .. outputdir)
    objdir ("bin-int/" .. outputdir)
    
    dependson { "Engine" }
    defines { "SFML_DYNAMIC" }
    disablewarnings { "4251" }

    files {
        "src/**.h",
        "src/**.cpp"
    }

    includedirs {
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
            "{COPYFILE} \"../extern/SFML/build/bin/Debug/sfml-graphics-d-3.dll\" \"%{cfg.targetdir}\"",
            "{COPYFILE} \"../extern/SFML/build/bin/Debug/sfml-window-d-3.dll\" \"%{cfg.targetdir}\"",
            "{COPYFILE} \"../extern/SFML/build/bin/Debug/sfml-system-d-3.dll\" \"%{cfg.targetdir}\"",
            "{COPYFILE} \"../extern/SFML/build/bin/Debug/sfml-audio-d-3.dll\" \"%{cfg.targetdir}\"",
            "{COPYFILE} \"../Engine/bin/" .. outputdir .. "/Engine.dll\" \"%{cfg.targetdir}\"",
            "{COPYFILE} \"../Engine/bin/" .. outputdir .. "/Engine.pdb\" \"%{cfg.targetdir}\""
        }

    filter { "system:windows", "configurations:Release" }
        runtime "Release"
        optimize "On"
        
        libdirs { 
            "../Engine/bin/" .. outputdir,
            "../extern/SFML/build/lib/Release"
        }
        
        links { 
            "sfml-graphics", "sfml-window", 
            "sfml-system", "sfml-audio", 
            "opengl32"
        }
        
        postbuildcommands {
            "{MKDIR} \"%{cfg.targetdir}\"",
            "{COPYFILE} \"../extern/SFML/build/bin/Release/sfml-graphics-3.dll\" \"%{cfg.targetdir}\"",
            "{COPYFILE} \"../extern/SFML/build/bin/Release/sfml-window-3.dll\" \"%{cfg.targetdir}\"",
            "{COPYFILE} \"../extern/SFML/build/bin/Release/sfml-system-3.dll\" \"%{cfg.targetdir}\"",
            "{COPYFILE} \"../extern/SFML/build/bin/Release/sfml-audio-3.dll\" \"%{cfg.targetdir}\"",
            "{COPYFILE} \"../Engine/bin/" .. outputdir .. "/Engine.dll\" \"%{cfg.targetdir}\""
        }

    filter {}