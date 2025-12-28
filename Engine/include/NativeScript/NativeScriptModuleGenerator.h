#pragma once
#include "Resource/Resource.h"

#include <string>
#include <filesystem>

namespace Luden
{
	class ENGINE_API NativeScriptModuleGenerator
	{
	public:
		static bool CreateNewScriptModule(const std::string& projectName, const std::filesystem::path& path="");

	private:
		static bool GenerateHeaderFile(const std::string& projectName, const std::filesystem::path& includePath);

		static bool GenerateSourceFile(const std::string& projectName, const std::filesystem::path& sourcePath);
	};
}