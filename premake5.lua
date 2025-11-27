workspace "LudenEngine"
    architecture "x64"
    configurations { "Debug", "Release" }
    startproject "Editor"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

filter "configurations:Debug"
    defines { "LUDEN_CONFIG_DEBUG" }
    symbols "On"

filter "configurations:Release"
    defines { "LUDEN_CONFIG_RELEASE" }
    optimize "On"
    
-- Include directories
IncludeDirs = {}
IncludeDirs["SFML"] = "extern/SFML/include"
IncludeDirs["Box2D"] = "extern/Box2D/include"
IncludeDirs["glm"] = "extern/glm"
IncludeDirs["json"] = "extern/json/include"
IncludeDirs["imgui"] = "extern/imgui"
IncludeDirs["ImGuiSFML"] = "extern/ImGui-SFML"
IncludeDirs["IconFontCpp"] = "extern/IconFontCppHeaders"
IncludeDirs["nfd"] = "extern/nativefiledialog-extended/src/include"

-- Include sub-projects
include "Engine"
include "GameModule"
include "Editor"