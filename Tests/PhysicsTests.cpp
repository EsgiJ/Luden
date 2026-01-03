#include "TestFramework.h"
#include "Scene/Scene.h"
#include "ECS/Components/Components.h"
#include "Project/Project.h"
#include <SFML/Graphics.hpp>

using namespace Luden;
using namespace Luden::Testing;

void RunPhysicsTests()
{
	TestSuite suite("Physics Integration");

	suite.AddTest("Physics World Creation", []()
		{
			if (!Luden::Project::GetActiveProject())
			{
				std::cout << "    No active project - skipping physics test\n";
				std::cout << "    (Tip: Place Tetris.lproject in Tetris/ folder)\n";
				return;
			}

			auto scene = std::make_shared<Scene>("PhysicsTest");

			auto renderTexture = std::make_shared<sf::RenderTexture>();
			renderTexture->resize({ 800, 600 });

			scene->OnRuntimeStart();

			auto entity = scene->CreateEntity("PhysicsEntity");
			entity.Get<TransformComponent>().Translation = glm::vec3(0, 10, 0);

			auto& rb = entity.Add<RigidBody2DComponent>();
			rb.BodyType = RigidBody2DComponent::Type::Dynamic;

			AssertTrue(entity.Has<RigidBody2DComponent>(), "Entity should have rigidbody");

			scene->OnRuntimeStop();
		});

	suite.AddTest("Gravity Simulation", []()
		{
			if (!Luden::Project::GetActiveProject())
			{
				std::cout << "    No active project - skipping gravity test\n";
				return;
			}

			auto scene = std::make_shared<Scene>("GravityTest");
			auto renderTexture = std::make_shared<sf::RenderTexture>();
			renderTexture->resize({ 800, 600 });

			auto cameraEntity = scene->CreateEntity("MainCamera");
			auto& cameraComp = cameraEntity.Add<Camera2DComponent>();
			cameraComp.Primary = true;  

			auto initialEntity = scene->CreateEntity("FallingBox");
			initialEntity.Get<TransformComponent>().Translation = glm::vec3(400, 300, 0);

			auto& rbSetup = initialEntity.Add<RigidBody2DComponent>();
			rbSetup.BodyType = RigidBody2DComponent::Type::Dynamic;
			rbSetup.GravityScale = 1.0f;

			auto& collider = initialEntity.Add<BoxCollider2DComponent>();
			collider.Size = glm::vec2(50.0f, 50.0f);
			collider.Density = 1.0f;

			scene->SetViewportSize(800, 600);

			scene->OnRuntimeStart();

			Entity entity = scene->TryGetEntityWithTag("FallingBox");

			if (!entity.IsValid())
			{
				std::cout << "  ERROR: Entity not found!\n";
				AssertTrue(false, "Entity not found");
				return;
			}

			auto& rb = entity.Get<RigidBody2DComponent>();
			auto& transform = entity.Get<TransformComponent>();

			float initialY = transform.Translation.y;

			for (int i = 0; i < 60; i++)
			{
				scene->OnUpdateRuntime(TimeStep(1.0f / 60.0f), renderTexture);

				if (i % 10 == 0)
				{
					float currentY = entity.Get<TransformComponent>().Translation.y;
				}
			}

			float finalY = transform.Translation.y;

			AssertTrue(finalY > initialY, "Entity should have fallen due to gravity");

			scene->OnRuntimeStop();
		});

	suite.RunTests();
}
