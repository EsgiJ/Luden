#include "NativeScript/NativeScriptModuleGenerator.h"

#include <fstream>

namespace Luden
{
	bool NativeScriptModuleGenerator::CreateNewScriptModule(const std::string& projectName, const std::filesystem::path& path)
	{
		std::filesystem::path includePath = path / (projectName + ".h");
		std::filesystem::path sourcePath = path / (projectName + ".cpp");

		if (GenerateHeaderFile(projectName, includePath) && GenerateSourceFile(projectName, sourcePath))
			return false;
		
		return true;
	}

	bool NativeScriptModuleGenerator::GenerateHeaderFile(const std::string& projectName, const std::filesystem::path& includePath)
	{
		std::ofstream out(includePath);
		if (!out.is_open())
			return false;

		out << "#pragma once\n";
		out << "#include \"NativeScript/IScriptModule.h\"\n\n";

		out << "namespace Luden\n";
		out << "{\n";
		out << "	class " << projectName << " : public IScriptModule\n";
		out << "	{\n";
		out << "	public:\n";
		out << "		void OnLoad() override;\n";
		out << "		void OnUnload() override;\n";
		out << "		void RegisterScripts(ResourceManagerBase* resourceManager) override;\n";
		out << "		uint32_t GetVersion() const override;\n";
		out << "	};\n";
		out << "}\n";

		return true;
	}

	bool NativeScriptModuleGenerator::GenerateSourceFile(const std::string& projectName, const std::filesystem::path& sourcePath)
	{
		std::ofstream out(sourcePath);
		if (!out.is_open())
			return false;

		out << "#include \"" << projectName << ".h\"\n";
		out << "#include \"NativeScript/NativeScriptRegistry.h\"\n\n";

		out << "namespace Luden\n";
		out << "{\n";
		out << "	void " << projectName << "::OnLoad()\n";
		out << "	{\n";
		out << "		std::cout << \"Game Module Loaded!\" << std::endl;\n";
		out << "	}\n\n";
		out << "	void " << projectName << "::OnUnload()\n";
		out << "	{\n";
		out << "		std::cout << \"Game Module Unloaded!\" << std::endl;\n";
		out << "	}\n\n";
		out << "	void " << projectName << "::RegisterScripts(ResourceManagerBase* resourceManager)\n";
		out << "	{\n";
		out << "		std::cout << \"Registering scripts...\" << std::endl;\n";
		out << "	}\n\n";
		out << "	uint32_t " << projectName << "::GetVersion() const\n";
		out << "	{\n";
		out << "		return 1;\n";
		out << "	}\n\n";
		out << "	extern \"C\" GAME_MODULE_API IScriptModule* CreateScriptModule()\n";
		out << "	{\n";
		out << "		return new " << projectName << "();\n";
		out << "	}\n\n";
		out << "	extern \"C\" GAME_MODULE_API void DestroyScriptModule(IScriptModule* module)\n";
		out << "	{\n";
		out << "		delete module;\n";
		out << "	}\n";
		out << "}\n";

		return true;
	}
}