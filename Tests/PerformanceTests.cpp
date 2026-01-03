#include "TestFramework.h"
#include "Scene/Scene.h"
#include "ECS/Components/Components.h"
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

private:
	Timer m_Timer;
	int m_FrameCount = 0;
};

void RunPerformanceTests()
{
	TestSuite suite("Performance Tests");

	auto MeasureFPS = [](int entityCount, float duration = 1.0f) -> float
		{
			auto scene = std::make_shared<Scene>("PerfTest");

			auto renderTexture = std::make_shared<sf::RenderTexture>();
			renderTexture->resize({ 800, 600 });

			for (int i = 0; i < entityCount; i++)
			{
				auto entity = scene->CreateEntity("Entity" + std::to_string(i));

				auto& transform = entity.Get<TransformComponent>();
				transform.Translation = glm::vec3(
					(rand() % 100) - 50.0f,
					(rand() % 100) - 50.0f,
					0
				);

				auto& sprite = entity.Add<SpriteRendererComponent>();
				sprite.tint = sf::Color(
					rand() % 255,
					rand() % 255,
					rand() % 255,
					255
				);
			}

			scene->OnRuntimeStart();

			FrameRateMeasurement measurement;
			measurement.StartMeasurement();

			float elapsed = 0.0f;
			const float frameTime = 1.0f / 60.0f;

			while (elapsed < duration)
			{
				scene->OnUpdateRuntime(TimeStep(frameTime), renderTexture);
				measurement.RecordFrame();
				elapsed += frameTime;
			}

			scene->OnRuntimeStop();

			return measurement.GetAverageFPS();
		};

	suite.AddBenchmark("FPS with 100 Entities", [&]()
		{
			float fps = MeasureFPS(100, 1.0f);
			std::cout << "    Measured FPS: " << std::fixed << std::setprecision(1) << fps << "\n";
		}, 1);

	suite.AddBenchmark("FPS with 500 Entities", [&]()
		{
			float fps = MeasureFPS(500, 1.0f);
			std::cout << "    Measured FPS: " << std::fixed << std::setprecision(1) << fps << "\n";
		}, 1);

	suite.AddBenchmark("FPS with 1000 Entities", [&]()
		{
			float fps = MeasureFPS(1000, 1.0f);
			std::cout << "    Measured FPS: " << std::fixed << std::setprecision(1) << fps << "\n";
		}, 1);

	suite.AddBenchmark("FPS with 2000 Entities", [&]()
		{
			float fps = MeasureFPS(2000, 1.0f);
			std::cout << "    Measured FPS: " << std::fixed << std::setprecision(1) << fps << "\n";
		}, 1);

	suite.AddBenchmark("FPS with 5000 Entities", [&]()
		{
			float fps = MeasureFPS(5000, 0.5f);
			std::cout << "    Measured FPS: " << std::fixed << std::setprecision(1) << fps << "\n";
		}, 1);

	suite.RunBenchmarks();
}