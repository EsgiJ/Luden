#include "TestFramework.h"
#include "Project/Project.h"
#include "Project/ProjectSerializer.h"
#include "Core/RuntimeApplication.h"
#include <iostream>
#include <filesystem>

// Forward declarations
void RunEntityMemoryPoolTests();
void RunSceneSerializationTests();
void RunResourceManagerTests();
void RunPhysicsTests(Luden::RuntimeApplication* app);
void RunPerformanceTests(Luden::RuntimeApplication* app);

int main(int argc, char** argv)
{
	using namespace Luden;
	using namespace Luden::Testing;

	std::cout << "\n";
	std::cout << "==========================================\n";
	std::cout << "   LUDEN ENGINE - TEST SUITE\n";
	std::cout << "==========================================\n";
	std::cout << "\nInitializing test environment...\n";

	std::shared_ptr<Project> activeProject;
	try
	{
		std::filesystem::path projectPath = "Tetris/Tetris.lproject";
		if (std::filesystem::exists(projectPath))
		{
			activeProject = std::make_shared<Project>();
			ProjectSerializer serializer(activeProject);
			if (serializer.Deserialize(projectPath))
			{
				Project::SetActive(activeProject);
				std::cout << "Loaded Tetris project successfully\n";
				std::cout << "  Project: " << activeProject->GetConfig().Name << "\n";
			}
			else
			{
				std::cout << "Failed to deserialize project\n";
			}
		}
		else
		{
			std::cout << "Project not found at: " << projectPath << "\n";
			std::cout << "  Some tests will be skipped\n";
		}
	}
	catch (const std::exception& e)
	{
		std::cout << "Exception loading project: " << e.what() << "\n";
	}

	Timer totalTimer;

	bool runAll = true;
	bool runQuick = false;
	bool runBench = false;
	bool runIntegration = false;

	if (argc > 1)
	{
		std::string arg = argv[1];
		if (arg == "--quick")
		{
			runAll = false;
			runQuick = true;
		}
		else if (arg == "--bench")
		{
			runAll = false;
			runBench = true;
		}
		else if (arg == "--integration")
		{
			runAll = false;
			runIntegration = true;
		}
		else if (arg == "--all")
		{
			runAll = true;
		}
		else if (arg == "--help")
		{
			std::cout << "\nUsage: EngineTests [option]\n";
			std::cout << "Options:\n";
			std::cout << "  --all          Run all tests (default)\n";
			std::cout << "  --quick        Run only quick tests (no window)\n";
			std::cout << "  --bench        Run only performance benchmarks\n";
			std::cout << "  --integration  Run only integration tests\n";
			std::cout << "  --help         Show this help message\n";
			return 0;
		}
	}

	if (runQuick || runAll)
	{
		std::cout << "\n==========================================\n";
		std::cout << "   QUICK TESTS (No Window Required)\n";
		std::cout << "==========================================\n";

		try
		{
			RunEntityMemoryPoolTests();
			RunSceneSerializationTests();
			RunResourceManagerTests();
		}
		catch (const std::exception& e)
		{
			std::cout << "\nException in quick tests: " << e.what() << "\n";
		}
	}

	RuntimeApplication* app = nullptr;
	if ((runBench || runIntegration || runAll) && activeProject)
	{
		try
		{
			std::cout << "\n==========================================\n";
			std::cout << "   VISUAL TESTS (Window Required)\n";
			std::cout << "==========================================\n";
			std::cout << "Controls:\n";
			std::cout << "  ESC   - Skip current test\n";
			std::cout << "  Close - Exit all tests\n";
			std::cout << "==========================================\n\n";

			ApplicationSpecification spec;
			spec.Name = "Luden Test Suite";
			spec.WindowWidth = 1920;
			spec.WindowHeight = 1080;
			spec.VSync = false;  
			spec.ProjectPath = "Tetris/Tetris.lproject";

			app = new RuntimeApplication(spec);

			app->Init();

			if (runIntegration || runAll)
			{
				std::cout << "==========================================\n";
				std::cout << "   INTEGRATION TESTS\n";
				std::cout << "==========================================\n";
				try
				{
					RunPhysicsTests(app);
				}
				catch (const std::exception& e)
				{
					std::cout << "\nException in physics tests: " << e.what() << "\n";
				}
			}

			if (runBench || runAll)
			{
				std::cout << "\n==========================================\n";
				std::cout << "   PERFORMANCE BENCHMARKS\n";
				std::cout << "==========================================\n";
				try
				{
					RunPerformanceTests(app);
				}
				catch (const std::exception& e)
				{
					std::cout << "\nException in performance tests: " << e.what() << "\n";
				}
			}

			std::cout << "\n[RuntimeApplication] Shutting down...\n";
			app->Shutdown();
			delete app;
			std::cout << "[RuntimeApplication] Cleanup complete\n";
		}
		catch (const std::exception& e)
		{
			std::cout << "\nFatal exception in visual tests: " << e.what() << "\n";
			if (app)
			{
				app->Shutdown();
				delete app;
			}
		}
	}
	else if ((runBench || runIntegration || runAll) && !activeProject)
	{
		std::cout << "\n==========================================\n";
		std::cout << "   WARNING\n";
		std::cout << "==========================================\n";
		std::cout << "Cannot run visual tests without a valid project!\n";
		std::cout << "Please ensure Tetris/Tetris.lproject exists.\n";
		std::cout << "==========================================\n";
	}

	float totalTime = totalTimer.ElapsedSeconds();

	std::cout << "\n";
	std::cout << "==========================================\n";
	std::cout << "   TEST SUITE COMPLETED\n";
	std::cout << "==========================================\n";
	std::cout << "Total Time: " << std::fixed << std::setprecision(2)
		<< totalTime << " seconds\n";
	std::cout << "==========================================\n\n";

	return 0;
}