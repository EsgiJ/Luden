#include "TestFramework.h"
#include "Project/Project.h"
#include "Project/ProjectSerializer.h"
#include <iostream>
#include <filesystem>

void RunEntityMemoryPoolTests();
void RunSceneSerializationTests();
void RunResourceManagerTests();
void RunPhysicsTests();
void RunPerformanceTests();

int main(int argc, char** argv)
{
	using namespace Luden::Testing;

	std::cout << "\n";
	std::cout << "==========================================\n";
	std::cout << "   LUDEN ENGINE - TEST SUITE\n";
	std::cout << "==========================================\n";

	// Try to load test project
	std::cout << "\nInitializing test environment...\n";
	try
	{
		// Look for Tetris project
		std::filesystem::path projectPath = "Tetris/Tetris.lproject";

		if (std::filesystem::exists(projectPath))
		{
			auto activeProject = std::make_shared<Luden::Project>();
			Luden::ProjectSerializer serializer(activeProject);

			if (serializer.Deserialize(projectPath))
			{
				Luden::Project::SetActive(activeProject);
				std::cout << "Loaded Tetris project for testing.\n";
			}
			else
			{
				std::cout << "Failed to load project. Some tests may be skipped.\n";
			}
		}
		else
		{
			std::cout << "No test project found at: " << projectPath << "\n";
			std::cout << "Some tests (physics) will be skipped.\n";
		}
	}
	catch (const std::exception& e)
	{
		std::cout << "Could not load project: " << e.what() << "\n";
		std::cout << "Some tests will be skipped.\n";
	}

	Timer totalTimer;

	bool runAll = true;
	bool runQuick = false;
	bool runBench = false;

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
		else if (arg == "--all")
		{
			runAll = true;
		}
	}

	if (runQuick)
	{
		std::cout << "\n[QUICK TEST MODE]\n";
		RunEntityMemoryPoolTests();
		RunSceneSerializationTests();
	}
	else if (runBench)
	{
		std::cout << "\n[BENCHMARK MODE]\n";
		RunPerformanceTests();
	}
	else if (runAll)
	{
		std::cout << "\n[RUNNING ALL TESTS]\n";
		RunEntityMemoryPoolTests();
		RunSceneSerializationTests();
		RunResourceManagerTests();
		RunPhysicsTests();
		RunPerformanceTests();
	}

	float totalTime = totalTimer.ElapsedSeconds();

	std::cout << "\n";
	std::cout << "==========================================\n";
	std::cout << "   ALL TESTS COMPLETED\n";
	std::cout << "   Total Time: " << std::fixed << std::setprecision(2)
		<< totalTime << " seconds\n";
	std::cout << "==========================================\n";

	std::cout << "\nPress Enter to exit...";
	std::cin.get();

	return 0;
}