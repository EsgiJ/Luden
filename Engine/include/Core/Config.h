#pragma once
#include <string>
#include <filesystem>
#include "Project/Project.h"

namespace Luden
{
	class Config
	{
	public:
		static constexpr const char* GetBuildConfig()
		{
#ifdef LUDEN_CONFIG_DEBUG
			return "Debug";
#elif LUDEN_CONFIG_RELEASE
			return "Release";
#endif
		}

		static constexpr const char* GetSystem()
		{
#ifdef _WIN32
			return "windows";
#elif __linux__
			return "linux";
#elif __APPLE__
			return "macosx";
#endif
		}

		static constexpr const char* GetArchitecture()
		{
#ifdef _WIN64
			return "x86_64";
#else
			return "x86";
#endif
		}

		static std::string GetOutputDir()
		{
			return std::string(GetBuildConfig()) + "-" +
				std::string(GetSystem()) + "-" +
				std::string(GetArchitecture());
		}

		static std::filesystem::path GetGameModulePath()
		{
			auto project = Project::GetActiveProject();
			if (!project)
				return "";

			std::filesystem::path projectDir = project->GetConfig().ProjectDirectory;
			std::string projectName = project->GetConfig().Name;
			std::string outputDir = GetOutputDir();

			return projectDir / "bin" / outputDir / (projectName + GetModuleExtension());
		}

		static constexpr const char* GetModuleExtension()
		{
#ifdef _WIN32
			return ".dll";
#elif __linux__
			return ".so";
#elif __APPLE__
			return ".dylib";
#endif
		}

		static constexpr const char* GetProjectExtension()
		{
			return ".lproject";
		}

		static constexpr const char* GetSceneExtension()
		{
			return ".lscene";
		}
	};
}