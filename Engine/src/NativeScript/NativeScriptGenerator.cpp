#include "NativeScript/NativeScriptGenerator.h"
#include "IO/FileSystem.h"
#include "Resource/Resource.h"
#include "NativeScript/NativeScript.h"
#include "Project/Project.h"

#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>


namespace Luden
{
	ResourceHandle NativeScriptGenerator::CreateNewScript(const std::string& className, const std::string& baseClassName
		, const std::filesystem::path& sourcePath, const std::filesystem::path& headerPath)
	{
		if(FileSystem::Exists(headerPath))
		{
			std::cerr << "Script already exists: " << className << std::endl;
			return (ResourceHandle)0;
		}

		GenerateHeaderFile(headerPath, className, baseClassName);
		GenerateSourceFile(sourcePath, className);

		std::filesystem::path scriptsDir = Project::GetActiveResourceDirectory() / "Scripts";
		FileSystem::CreateDir(scriptsDir);
		std::filesystem::path lnsPath = scriptsDir / (className + ".lns");

		auto resourceManager = Project::GetEditorResourceManager();

		auto script = std::make_shared<NativeScript>(className, nullptr, nullptr);
		script->SetClassName(className);
		script->SetHeaderPath(resourceManager->GetRelativePath(headerPath));
		script->SetSourcePath(resourceManager->GetRelativePath(sourcePath));

		ResourceMetadata metadata;
		metadata.Handle = ResourceHandle();
		metadata.FilePath = resourceManager->GetRelativePath(lnsPath);
		metadata.Type = ResourceType::NativeScript;

		script->Handle = metadata.Handle;

		resourceManager->SetMetadata(metadata.Handle, metadata);
		auto& resources = resourceManager->GetLoadedResources();
		resources[metadata.Handle] = std::static_pointer_cast<Resource>(script);

		// Save .lns file
		std::ofstream out(lnsPath);
		nlohmann::json j;
		j["ClassName"] = className;
		j["HeaderPath"] = script->GetHeaderPath().string();
		j["SourcePath"] = script->GetSourcePath().string();
		j["Handle"] = static_cast<uint64_t>(script->Handle);
		out << j.dump(4);

		std::cout << "Script created: " << className << std::endl;
		std::cout << "  Header: " << headerPath << std::endl;
		std::cout << "  Source: " << sourcePath << std::endl;

		return metadata.Handle;
	}

	void NativeScriptGenerator::GenerateHeaderFile(const std::filesystem::path& path, const std::string& className, const std::string& baseClassName)
	{
		std::ofstream out(path);

		out << "#pragma once\n";
		out << "#include \"Luden.h\"\n\n";
		out << "namespace Luden\n{\n";
		out << "	class " << className << " : public " << baseClassName << "\n";
		out << "    {\n";
		out << "    public:\n";
		out << "        virtual void OnCreate() override;\n";
		out << "        virtual void OnUpdate(TimeStep ts) override;\n";
		out << "        virtual void OnDestroy() override;\n";
		out << "        virtual void OnCollisionBegin(const CollisionContact& contact) override;\n";
		out << "        virtual void OnCollisionEnd(const CollisionContact& contact) override;\n";
		out << "        virtual void OnCollisionHit(const CollisionContact& contact) override;\n";
		out << "    };\n";
		out << "}\n";

		out.close();
	}
	void NativeScriptGenerator::GenerateSourceFile(const std::filesystem::path& path, const std::string& className)
	{
		std::ofstream out(path);

		out << "#include \"" << className << ".h\"\n\n";
		out << "namespace Luden\n{\n";
		out << "    void " << className << "::OnCreate()\n";
		out << "    {\n";
		out << "        // TODO: Initialize\n";
		out << "    }\n\n";
		out << "    void " << className << "::OnUpdate(TimeStep ts)\n";
		out << "    {\n";
		out << "        // TODO: Update logic\n";
		out << "    }\n\n";
		out << "    void " << className << "::OnDestroy()\n";
		out << "    {\n";
		out << "        // TODO: Cleanup\n";
		out << "    }\n\n";
		out << "    void " << className << "::OnCollisionBegin(const CollisionContact& contact)\n";
		out << "    {\n";
		out << "        // TODO: On contact begin\n";
		out << "    }\n\n";
		out << "    void " << className << "::OnCollisionEnd(const CollisionContact& contact)\n";
		out << "    {\n";
		out << "        // TODO: On contact end\n";
		out << "    }\n\n";
		out << "    void " << className << "::OnCollisionHit(const CollisionContact& contact)\n";
		out << "    {\n";
		out << "        // TODO: On hit(high speed)\n";
		out << "    }\n\n";
		out << "}\n";

		out.close();

	}
}