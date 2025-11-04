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
			return Project::GetProjectDirectory() /
				std::filesystem::path("bin") /
				GetOutputDir() /
				std::filesystem::path(Project::GetActiveProject()->GetConfig().Name + ".dll");
		}

	};
}
