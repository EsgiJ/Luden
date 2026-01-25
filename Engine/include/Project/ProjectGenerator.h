#pragma once
#include "EngineAPI.h"
#include <filesystem>

namespace Luden
{
	class ENGINE_API ProjectGenerator
	{
	public:
		static bool CreateProject(const std::string& projectName = "MyGame", const std::filesystem::path& path = "");

	private:
		static bool CreateDirectoryStructure(const std::filesystem::path& projectPath);
		static bool CreateProjectFile(const std::filesystem::path& projectPath, const std::string& name);
		static bool CreatePremakeFile(const std::filesystem::path& projectPath, const std::string& name);
		static bool CreateGenerateBat(const std::filesystem::path& projectPath);
		static bool CreateDefaultScene(const std::filesystem::path& resourcePath);
		static bool CreateCollisionChannels(const std::filesystem::path& resourcePath);
		static bool CreateResourceRegistry(const std::filesystem::path& resourcePath);
		static bool CopyPremakeExecutable(const std::filesystem::path& projectPath);

		static bool CreateBuildScript(const std::filesystem::path& path, const std::string& projectName);
		static bool CreateBuildReleaseScript(const std::filesystem::path& path);
		static bool CreateBuildAndRunScript(const std::filesystem::path& path);
		static bool CreateCleanScript(const std::filesystem::path& path);
	};
}