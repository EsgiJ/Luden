#include "NativeScript/NativeScriptModuleGenerator.h"

#include <fstream>

namespace Luden
{
	bool NativeScriptModuleGenerator::CreateNewScriptModule(const std::string& projectName, const std::filesystem::path& path)
	{
		std::filesystem::path moduleHeaderPath = path / (projectName + ".h");
		std::filesystem::path moduleSourcePath = path / (projectName + ".cpp");
		std::filesystem::path runtimeHeaderPath = path / (projectName + "RuntimeApp.h");
		std::filesystem::path runtimeSourcePath = path / (projectName + "RuntimeApp.cpp");

		if (!GenerateHeaderFile(projectName, moduleHeaderPath))
			return false;
		if (!GenerateSourceFile(projectName, moduleSourcePath))
			return false;
		if (!GenerateRuntimeAppHeader(projectName, runtimeHeaderPath))
			return false;
		if (!GenerateRuntimeAppSource(projectName, runtimeSourcePath))
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
		out << "\tclass " << projectName << " : public IScriptModule\n";
		out << "\t{\n";
		out << "\tpublic:\n";
		out << "\t\tvoid OnLoad() override;\n";
		out << "\t\tvoid OnUnload() override;\n";
		out << "\t\tvoid RegisterScripts(ResourceManagerBase* resourceManager) override;\n";
		out << "\t\tuint32_t GetVersion() const override;\n";
		out << "\t\tRuntimeApplication* CreateRuntimeApplication(const ApplicationSpecification& spec) override;\n";
		out << "\t};\n";
		out << "}\n";

		out.close();
		return true;
	}

	bool NativeScriptModuleGenerator::GenerateSourceFile(const std::string& projectName, const std::filesystem::path& sourcePath)
	{
		std::ofstream out(sourcePath);
		if (!out.is_open())
			return false;

		out << "#include \"" << projectName << ".h\"\n";
		out << "#include \"" << projectName << "RuntimeApp.h\"\n";
		out << "#include \"NativeScript/NativeScriptRegistry.h\"\n";
		out << "#include <iostream>\n\n";

		out << "namespace Luden\n";
		out << "{\n";

		out << "\tvoid " << projectName << "::OnLoad()\n";
		out << "\t{\n";
		out << "\t\tstd::cout << \"[" << projectName << "] Module Loaded!\" << std::endl;\n";
		out << "\t}\n\n";

		out << "\tvoid " << projectName << "::OnUnload()\n";
		out << "\t{\n";
		out << "\t\tstd::cout << \"[" << projectName << "] Module Unloaded!\" << std::endl;\n";
		out << "\t}\n\n";

		out << "\tvoid " << projectName << "::RegisterScripts(ResourceManagerBase* resourceManager)\n";
		out << "\t{\n";
		out << "\t\tstd::cout << \"[" << projectName << "] Registering scripts...\" << std::endl;\n";
		out << "\t\t\n";
		out << "\t\t// TODO: Register your native scripts here\n";
		out << "\t\t// Example:\n";
		out << "\t\t// REGISTER_SCRIPT(resourceManager, PlayerController);\n";
		out << "\t\t// REGISTER_SCRIPT(resourceManager, EnemyAI);\n";
		out << "\t}\n\n";

		out << "\tuint32_t " << projectName << "::GetVersion() const\n";
		out << "\t{\n";
		out << "\t\treturn 1;\n";
		out << "\t}\n\n";

		out << "\tRuntimeApplication* " << projectName << "::CreateRuntimeApplication(const ApplicationSpecification& spec)\n";
		out << "\t{\n";
		out << "\t\treturn new " << projectName << "RuntimeApp(spec);\n";
		out << "\t}\n\n";

		out << "\textern \"C\" GAME_MODULE_API IScriptModule* CreateScriptModule()\n";
		out << "\t{\n";
		out << "\t\treturn new " << projectName << "();\n";
		out << "\t}\n\n";

		out << "\textern \"C\" GAME_MODULE_API void DestroyScriptModule(IScriptModule* module)\n";
		out << "\t{\n";
		out << "\t\tdelete module;\n";
		out << "\t}\n";
		out << "}\n";

		out.close();
		return true;
	}

	bool NativeScriptModuleGenerator::GenerateRuntimeAppHeader(const std::string& projectName, const std::filesystem::path& includePath)
	{
		std::ofstream out(includePath);
		if (!out.is_open())
			return false;

		out << "#pragma once\n";
		out << "#include \"Core/RuntimeApplication.h\"\n\n";
		out << "namespace Luden\n";
		out << "{\n";
		out << "\tclass " << projectName << "RuntimeApp : public RuntimeApplication\n";
		out << "\t{\n";
		out << "\tpublic:\n";
		out << "\t\t" << projectName << "RuntimeApp(const ApplicationSpecification& spec);\n";
		out << "\t\t~" << projectName << "RuntimeApp() override = default;\n\n";
		out << "\t\tvoid OnUpdate(TimeStep ts) override;\n\n";
		out << "\tprivate:\n";
		out << "\t\t// Add your custom runtime application members here\n";
		out << "\t\tfloat m_UpdateTimer = 0.0f;\n";
		out << "\t};\n";
		out << "}\n";

		out.close();
		return true;
	}

	bool NativeScriptModuleGenerator::GenerateRuntimeAppSource(const std::string& projectName, const std::filesystem::path& sourcePath)
	{
		std::ofstream out(sourcePath);
		if (!out.is_open())
			return false;

		out << "#include \"" << projectName << "RuntimeApp.h\"\n";
		out << "#include <iostream>\n\n";
		out << "namespace Luden\n";
		out << "{\n";

		out << "\t" << projectName << "RuntimeApp::" << projectName << "RuntimeApp(const ApplicationSpecification& spec)\n";
		out << "\t\t: RuntimeApplication(spec)\n";
		out << "\t{\n";
		out << "\t\tstd::cout << \"[" << projectName << "RuntimeApp] Custom runtime application created!\" << std::endl;\n";
		out << "\t}\n\n";

		out << "\tvoid " << projectName << "RuntimeApp::OnUpdate(TimeStep ts)\n";
		out << "\t{\n";
		out << "\t\tRuntimeApplication::OnUpdate(ts);\n\n";
		out << "\t\tm_UpdateTimer += ts.GetSeconds();\n\n";
		out << "\t\t// TODO: Add your custom game loop logic here\n";
		out << "\t\t// Example:\n";
		out << "\t\t// if (m_UpdateTimer >= 1.0f)\n";
		out << "\t\t// {\n";
		out << "\t\t//     std::cout << \"One second passed!\" << std::endl;\n";
		out << "\t\t//     m_UpdateTimer = 0.0f;\n";
		out << "\t\t// }\n";
		out << "\t}\n";

		out << "}\n";

		out.close();
		return true;
	}
}