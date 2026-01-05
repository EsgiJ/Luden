#include "Core/RuntimeApplication.h"
#include "NativeScript/NativeScriptModuleLoader.h"
#include "Project/Project.h"
#include "Project/ProjectSerializer.h"
#include "Core/Config.h"
#include <iostream>
#include <filesystem>

int main(int argc, char** argv)
{
	std::filesystem::path projectPath;

	if (argc > 1)
	{
		projectPath = argv[1];
	}
	else
	{
		projectPath = "C:/GameProjects/Luden/Tetris/Tetris.lproject";
	}

	if (!std::filesystem::exists(projectPath))
	{
		std::cerr << "Error: Project file not found: " << projectPath << "\n";
		return -1;
	}

	if (projectPath.extension() != Luden::Config::GetProjectExtension())
	{
		std::cerr << "Error: Invalid project file. Expected "
			<< Luden::Config::GetProjectExtension() << " file\n";
		return -1;
	}

	try
	{
		std::filesystem::path projectDir = projectPath.parent_path();
		std::string projectName = projectPath.stem().string();

		std::cout << "===========================================\n";
		std::cout << "         Luden Runtime\n";
		std::cout << "===========================================\n";
		std::cout << "Project: " << projectPath << "\n";
		std::cout << "===========================================\n\n";

		// 1. ÖNCE Projeyi yükle
		std::cout << "Loading project...\n";
		std::shared_ptr<Luden::Project> project = std::make_shared<Luden::Project>();
		Luden::ProjectSerializer serializer(project);

		if (!serializer.Deserialize(projectPath))
		{
			std::cerr << "[ERROR] Failed to deserialize project!\n";
			return -1;
		}

		Luden::Project::SetActive(project);
		std::cout << "[OK] Project loaded: " << project->GetConfig().Name << "\n\n";

		// 2. SONRA Modülü yükle
		std::string outputDir = Luden::Config::GetOutputDir();
		std::filesystem::path modulePath = projectDir / "bin" / outputDir /
			(projectName + Luden::Config::GetModuleExtension());

		std::cout << "Loading module: " << modulePath << "\n";

		if (!std::filesystem::exists(modulePath))
		{
			std::cerr << "[ERROR] Module file not found!\n";
			std::cerr << "Expected at: " << modulePath << "\n";
			return -1;
		}

		Luden::NativeScriptModuleLoader moduleLoader;
		if (!moduleLoader.LoadModule(modulePath))
		{
			std::cerr << "[ERROR] Failed to load game module!\n";
			return -1;
		}

		std::cout << "[OK] Module loaded!\n\n";

		// 3. ApplicationSpec oluþtur
		Luden::ApplicationSpecification spec;
		spec.Name = projectName;
		spec.WindowWidth = 1280;
		spec.WindowHeight = 720;
		spec.VSync = true;
		spec.ProjectPath = projectPath;
		spec.WorkingDirectory = projectDir.string();

		// 4. Module'den RuntimeApplication oluþtur
		auto module = moduleLoader.GetModule();
		if (!module)
		{
			std::cerr << "[ERROR] Module is null!\n";
			return -1;
		}

		std::cout << "Creating RuntimeApplication...\n";
		Luden::RuntimeApplication* runtimeApp = module->CreateRuntimeApplication(spec);

		if (!runtimeApp)
		{
			std::cerr << "[ERROR] Failed to create RuntimeApplication!\n";
			return -1;
		}

		std::cout << "[OK] RuntimeApplication created!\n\n";

		// 5. Initialize ve Run
		std::cout << "Initializing...\n";
		runtimeApp->Init();

		std::cout << "Starting runtime...\n";
		std::cout << "===========================================\n\n";
		runtimeApp->Run();

		// Cleanup
		delete runtimeApp;
		std::cout << "\n===========================================\n";
		std::cout << "Runtime shutdown complete.\n";
		std::cout << "===========================================\n";
	}
	catch (const std::exception& e)
	{
		std::cerr << "\n[FATAL ERROR] " << e.what() << "\n";
		return -1;
	}

	return 0;
}