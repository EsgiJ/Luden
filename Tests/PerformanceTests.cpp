#include "TestFramework.h"
#include "Scene/Scene.h"
#include "ECS/Components/Components.h"
#include "Resource/ResourceManager.h"
#include "Graphics/Sprite.h"
#include "Project/Project.h"
#include "Core/RuntimeApplication.h"
#include <SFML/Graphics.hpp>

using namespace Luden;
using namespace Luden::Testing;

class FrameRateMeasurement
{
public:
	void StartMeasurement()
	{
		m_FrameCount = 0;
		m_Timer.Reset();
	}

	void RecordFrame()
	{
		m_FrameCount++;
	}

	float GetAverageFPS()
	{
		float elapsed = m_Timer.ElapsedSeconds();
		if (elapsed == 0.0f) return 0.0f;
		return m_FrameCount / elapsed;
	}

	int GetFrameCount() const { return m_FrameCount; }

private:
	Timer m_Timer;
	int m_FrameCount = 0;
};

void RunPerformanceTests(RuntimeApplication* app)
{
	TestSuite suite("Performance Tests");

	ResourceHandle whiteBlockHandle = 0;
	if (Project::GetActiveProject())
	{
		auto sprite = ResourceManager::GetResource<Sprite>(9840798825938959016);
		if (sprite)
		{
			whiteBlockHandle = sprite->Handle;
			std::cout << "    Loaded WhiteBlock sprite for testing\n";
		}
		else
		{
			std::cout << "    WhiteBlock sprite not found, using colored rectangles\n";
		}
	}

	auto previousScene = app->GetCurrentScene();
	if (previousScene)
	{
		std::cout << "    Pausing current scene for performance tests...\n";
		previousScene->OnRuntimeStop();
	}

	auto MeasureFPS = [app, whiteBlockHandle](int entityCount, float targetDuration) -> float
		{
			std::cout << "    Creating scene with " << entityCount << " entities...\n";

			auto scene = std::make_shared<Scene>("PerfTest");

			auto cameraEntity = scene->CreateEntity("MainCamera");
			auto& camera = cameraEntity.Add<Camera2DComponent>();
			camera.Camera.SetType(Luden::Camera2D::Type::FollowXY);
			camera.Primary = true;

			for (int i = 0; i < entityCount; i++)
			{
				auto entity = scene->CreateEntity("Entity" + std::to_string(i));
				auto& transform = entity.Get<TransformComponent>();

				transform.Translation = glm::vec3(
					(i % 40) * 48.0f - 960.0f,  
					(i / 40) * 48.0f - 540.0f,  
					0
				);
				transform.Scale = glm::vec3(1.0f, 1.0f, 1.0f);

				auto& sprite = entity.Add<SpriteRendererComponent>();

				if (whiteBlockHandle != 0)
				{
					sprite.spriteHandle = whiteBlockHandle;
				}

				sprite.tint = sf::Color(
					(i * 37) % 255,
					(i * 67) % 255,
					(i * 97) % 255,
					255
				);
			}

			scene->SetViewportSize(1920, 1080);
			scene->OnRuntimeStart();

			FrameRateMeasurement measurement;
			measurement.StartMeasurement();

			Timer realTimer;
			sf::Clock frameClock;

			auto& window = app->GetWindow();
			auto renderTexture = app->GetRenderTexture();

			std::cout << "    Testing for " << targetDuration << " seconds...\n";

			bool testSkipped = false;
			int lastSecond = -1;

			while (realTimer.ElapsedSeconds() < targetDuration && window.isOpen())
			{
				while (const std::optional event = window.pollEvent())
				{
					if (event->is<sf::Event::Closed>())
					{
						window.close();
						testSkipped = true;
						break;
					}
					if (event->is<sf::Event::KeyPressed>())
					{
						auto keyEvent = event->getIf<sf::Event::KeyPressed>();
						if (keyEvent && keyEvent->code == sf::Keyboard::Key::Escape)
						{
							std::cout << "    Test skipped by user (ESC pressed)\n";
							testSkipped = true;
							break;
						}
					}
				}

				if (testSkipped) break;

				sf::Time dt = frameClock.restart();
				TimeStep timestep(dt.asSeconds());

				scene->OnUpdateRuntime(timestep, renderTexture);

				window.clear(sf::Color(32, 32, 32));

				sf::Sprite displaySprite(renderTexture->getTexture());
				window.draw(displaySprite);

				window.display();

				measurement.RecordFrame();

				int currentSecond = (int)realTimer.ElapsedSeconds();
				if (currentSecond != lastSecond && currentSecond > 0)
				{
					float currentFPS = measurement.GetAverageFPS();
					std::cout << "    Second " << currentSecond << " - Current FPS: "
						<< std::fixed << std::setprecision(1) << currentFPS << "\n";
					lastSecond = currentSecond;
				}
			}

			scene->OnRuntimeStop();

			float finalFPS = measurement.GetAverageFPS();
			std::cout << "    Total frames: " << measurement.GetFrameCount() << "\n";
			return finalFPS;
		};

	std::cout << "\n========================================\n";
	std::cout << "Performance Benchmarks (Visual Rendering)\n";
	std::cout << "========================================\n";
	std::cout << "Press ESC to skip test | Close window to exit\n";
	std::cout << "----------------------------------------\n\n";

	suite.AddBenchmark("FPS with 100 Entities", [&]()
		{
			float fps = MeasureFPS(100, 3.0f);
			std::cout << "    >>> Average FPS: " << std::fixed << std::setprecision(1)
				<< fps << " <<<\n\n";
		}, 1);

	suite.AddBenchmark("FPS with 500 Entities", [&]()
		{
			float fps = MeasureFPS(500, 3.0f);
			std::cout << "    >>> Average FPS: " << std::fixed << std::setprecision(1)
				<< fps << " <<<\n\n";
		}, 1);

	suite.AddBenchmark("FPS with 1000 Entities", [&]()
		{
			float fps = MeasureFPS(1000, 3.0f);
			std::cout << "    >>> Average FPS: " << std::fixed << std::setprecision(1)
				<< fps << " <<<\n\n";
		}, 1);

	suite.AddBenchmark("FPS with 2000 Entities", [&]()
		{
			float fps = MeasureFPS(2000, 3.0f);
			std::cout << "    >>> Average FPS: " << std::fixed << std::setprecision(1)
				<< fps << " <<<\n\n";
		}, 1);

	suite.AddBenchmark("FPS with 5000 Entities", [&]()
		{
			float fps = MeasureFPS(5000, 3.0f);
			std::cout << "    >>> Average FPS: " << std::fixed << std::setprecision(1)
				<< fps << " <<<\n\n";
		}, 1);

	suite.RunBenchmarks();

	if (previousScene)
	{
		std::cout << "\n    Restoring previous scene...\n";
		previousScene->OnRuntimeStart();
	}
}