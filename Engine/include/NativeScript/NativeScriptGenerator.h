#pragma once
#include "Resource/Resource.h"

#include <string>
#include <filesystem>

namespace Luden
{
	class ENGINE_API NativeScriptGenerator
	{
	public:
		static ResourceHandle CreateNewScript(const std::string& className, const std::string& baseClassName = "ScriptableEntity", const std::filesystem::path& sourcePath = "", const std::filesystem::path& headerPath = "");

	private:
		static void GenerateHeaderFile(const std::filesystem::path& path, const std::string& className, const std::string& baseClassName);

		static void GenerateSourceFile(const std::filesystem::path& path, const std::string& className);
	};
}