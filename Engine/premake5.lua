project "Engine"
    location "."
    kind "SharedLib"
    language "C++"
    cppdialect "C++20"
    staticruntime "off"
    
    targetdir ("bin/" .. outputdir)
    objdir ("bin-int/" .. outputdir)
    
    defines { "ENGINE_EXPORTS", "SFML_DYNAMIC" }
    disablewarnings { "4251" }
    
    files {
        "src/**.cpp",
        "src/Generated/**.cpp",
        "include/**.h",
        "../extern/imgui/*.cpp",
        "../extern/ImGui-SFML/*.cpp",
        "../extern/ImGui-SFML/*.h"
    }

    includedirs {
        "include",
        "../%{IncludeDirs.Box2D}",
        "../%{IncludeDirs.imgui}",
        "../%{IncludeDirs.ImGuiSFML}",
        "../%{IncludeDirs.glm}",
        "../%{IncludeDirs.SFML}",
        "../%{IncludeDirs.json}",
        "../%{IncludeDirs.IconFontCpp}",
        "../%{IncludeDirs.nfd}",
        "../Tools"
    }

    filter "system:windows"
        files { "../extern/nfd/src/nfd_win.cpp" }
        links {"comdlg32", "ole32", "uuid", "shell32", "user32"}

    filter { "system:windows", "configurations:Debug" }
        runtime "Debug"
        symbols "On"
        
        libdirs { 
            "../extern/SFML/build/lib/Debug",
            "../extern/Box2D/build/src/Debug"
        }
        links { 
            "sfml-graphics-d", "sfml-window-d", 
            "sfml-system-d", "sfml-audio-d", 
            "opengl32", "box2dd" 
        }
        postbuildcommands {
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
            "../extern/Box2D/build/src/Release"
        }
        links { 
            "sfml-graphics", "sfml-window", 
            "sfml-system", "sfml-audio", 
            "opengl32", "box2d"
        }
        postbuildcommands {
            "{COPYFILE} ../extern/SFML/build/bin/Release/sfml-graphics-3.dll %{cfg.targetdir}/sfml-graphics-3.dll",
            "{COPYFILE} ../extern/SFML/build/bin/Release/sfml-window-3.dll %{cfg.targetdir}/sfml-window-3.dll",
            "{COPYFILE} ../extern/SFML/build/bin/Release/sfml-system-3.dll %{cfg.targetdir}/sfml-system-3.dll",
            "{COPYFILE} ../extern/SFML/build/bin/Release/sfml-audio-3.dll %{cfg.targetdir}/sfml-audio-3.dll"
        }

    filter {}