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
		std::cout << "[Runtime] Using project from command line: " << projectPath << "\n";
	}
	else
	{
		std::filesystem::path exePath = std::filesystem::path(argv[0]);
		std::filesystem::path exeDir = exePath.parent_path();

		std::cout << "[Runtime] Executable directory: " << exeDir << "\n";
		std::cout << "[Runtime] Searching for .lproject file...\n";

		for (const auto& entry : std::filesystem::directory_iterator(exeDir))
		{
			if (entry.is_regular_file() && entry.path().extension() == ".lproject")
			{
				projectPath = entry.path();
				break;
			}
		}

		if (projectPath.empty())
		{
			std::cerr << "\n[ERROR] No .lproject file found!\n\n";
			std::cout << "Press Enter to exit...\n";
			std::cin.get();
			return -1;
		}

		std::cout << "[Runtime] Found project: " << projectPath.filename() << "\n\n";
	}

	if (!std::filesystem::exists(projectPath))
	{
		std::cerr << "\n[ERROR] Project file not found: " << projectPath << "\n\n";
		std::cout << "Press Enter to exit...\n";
		std::cin.get();
		return -1;
	}

	try
	{
		std::filesystem::path projectDir = projectPath.parent_path();
		std::string projectName = projectPath.stem().string();

		std::cout << "===========================================\n";
		std::cout << "         Luden Runtime\n";
		std::cout << "===========================================\n";
		std::cout << "Project: " << projectPath.filename() << "\n";
		std::cout << "Directory: " << projectDir << "\n";
		std::cout << "===========================================\n\n";

		std::cout << "Loading project...\n";
		std::shared_ptr<Luden::Project> project = std::make_shared<Luden::Project>();
		Luden::ProjectSerializer serializer(project);

		if (!serializer.Deserialize(projectPath))
		{
			std::cerr << "\n[ERROR] Failed to deserialize project!\n\n";
			std::cout << "Press Enter to exit...\n";
			std::cin.get();
			return -1;
		}

		project->GetConfig().ProjectDirectory = projectDir.string();

		Luden::Project::SetActive(project);
		std::cout << "[OK] Project loaded: " << project->GetConfig().Name << "\n";
		std::cout << "[OK] Runtime directory set to: " << projectDir << "\n\n";

		std::filesystem::path modulePath = projectDir / (projectName + Luden::Config::GetModuleExtension());

		std::cout << "Loading module: " << modulePath.filename() << "\n";

		if (!std::filesystem::exists(modulePath))
		{
			std::cerr << "\n[ERROR] Module file not found!\n";
			std::cerr << "Expected: " << modulePath << "\n\n";
			std::cout << "Press Enter to exit...\n";
			std::cin.get();
			return -1;
		}

		Luden::NativeScriptModuleLoader moduleLoader;
		if (!moduleLoader.LoadModule(modulePath))
		{
			std::cerr << "\n[ERROR] Failed to load game module!\n\n";
			std::cout << "Press Enter to exit...\n";
			std::cin.get();
			return -1;
		}

		std::cout << "[OK] Module loaded!\n\n";

		Luden::ApplicationSpecification spec;
		spec.Name = projectName;
		spec.WindowWidth = 1920;
		spec.WindowHeight = 1080;
		spec.VSync = true;
		spec.ProjectPath = projectPath;
		spec.WorkingDirectory = projectDir.string();

		auto module = moduleLoader.GetModule();
		if (!module)
		{
			std::cerr << "\n[ERROR] Module is null!\n\n";
			std::cout << "Press Enter to exit...\n";
			std::cin.get();
			return -1;
		}

		std::cout << "Creating RuntimeApplication...\n";
		Luden::RuntimeApplication* runtimeApp = module->CreateRuntimeApplication(spec);

		if (!runtimeApp)
		{
			std::cerr << "\n[ERROR] Failed to create RuntimeApplication!\n\n";
			std::cout << "Press Enter to exit...\n";
			std::cin.get();
			return -1;
		}

		std::cout << "[OK] RuntimeApplication created!\n\n";
		std::cout << "Initializing...\n";
		runtimeApp->Init();

		std::cout << "Starting runtime...\n";
		std::cout << "===========================================\n\n";

		runtimeApp->Run();

		delete runtimeApp;

		std::cout << "\n===========================================\n";
		std::cout << "Runtime shutdown complete.\n";
		std::cout << "===========================================\n";
	}
	catch (const std::exception& e)
	{
		std::cerr << "\n[FATAL ERROR] " << e.what() << "\n\n";
		std::cout << "Press Enter to exit...\n";
		std::cin.get();
		return -1;
	}

	return 0;
}