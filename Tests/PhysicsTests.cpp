#include "TestFramework.h"
#include "Scene/Scene.h"
#include "ECS/Components/Components.h"
#include "Project/Project.h"
#include "Core/RuntimeApplication.h"
#include "Resource/ResourceManager.h"
#include "Graphics/Sprite.h"
#include <SFML/Graphics.hpp>

using namespace Luden;
using namespace Luden::Testing;

void RunPhysicsTests(RuntimeApplication* app)
{
	TestSuite suite("Physics Integration");

	ResourceHandle whiteBlockHandle = 0;
	if (Project::GetActiveProject())
	{
		auto sprite = ResourceManager::GetResource<Sprite>(9840798825938959016);
		if (sprite)
		{
			whiteBlockHandle = sprite->Handle;
			std::cout << "    Loaded WhiteBlock sprite for physics tests\n";
		}
	}

	suite.AddTest("Physics World Creation", [app]()
		{
			if (!Project::GetActiveProject())
			{
				std::cout << "    No active project - skipping physics test\n";
				return;
			}

			auto previousScene = app->GetCurrentScene();
			if (previousScene)
			{
				std::cout << "    Pausing current scene...\n";
				previousScene->OnRuntimeStop();
			}

			auto scene = std::make_shared<Scene>("PhysicsTest");
			auto entity = scene->CreateEntity("PhysicsEntity");
			entity.Get<TransformComponent>().Translation = glm::vec3(0, 10, 0);

			auto& rb = entity.Add<RigidBody2DComponent>();
			rb.BodyType = RigidBody2DComponent::Type::Dynamic;

			scene->SetViewportSize(1920, 1080);
			scene->OnRuntimeStart();

			AssertTrue(entity.Has<RigidBody2DComponent>(), "Entity should have rigidbody");

			scene->OnRuntimeStop();

			if (previousScene)
			{
				std::cout << "    Restoring previous scene...\n";
				previousScene->OnRuntimeStart();
			}

			std::cout << "    Physics world created successfully\n";
		});

	suite.AddTest("Gravity Simulation", [app, whiteBlockHandle]()
		{
			if (!Project::GetActiveProject())
			{
				std::cout << "    No active project - skipping gravity test\n";
				return;
			}

			std::cout << "    Setting up gravity simulation scene...\n";

			auto previousScene = app->GetCurrentScene();
			if (previousScene)
			{
				std::cout << "    Pausing current scene...\n";
				previousScene->OnRuntimeStop();
			}

			auto scene = std::make_shared<Scene>("GravityTest");

			auto cameraEntity = scene->CreateEntity("MainCamera");
			auto& cameraComp = cameraEntity.Add<Camera2DComponent>();
			cameraComp.Primary = true;

			auto fallingEntity = scene->CreateEntity("FallingBox");
			auto& transform = fallingEntity.Get<TransformComponent>();
			transform.Translation = glm::vec3(0, -400, 0);  
			transform.Scale = glm::vec3(2.0f, 2.0f, 1.0f);

			auto& rb = fallingEntity.Add<RigidBody2DComponent>();
			rb.BodyType = RigidBody2DComponent::Type::Dynamic;
			rb.GravityScale = 1.0f;

			auto& collider = fallingEntity.Add<BoxCollider2DComponent>();
			collider.Size = glm::vec2(48.0f, 48.0f); 
			collider.Density = 1.0f;
			collider.Friction = 0.3f;
			collider.CategoryBits = 0x0001;
			collider.MaskBits = 0xFFFF;
			collider.GroupIndex = 0;

			auto& sprite = fallingEntity.Add<SpriteRendererComponent>();
			if (whiteBlockHandle != 0)
			{
				sprite.spriteHandle = whiteBlockHandle;
			}
			sprite.tint = sf::Color::Red;

			auto groundEntity = scene->CreateEntity("Ground");
			auto& groundTransform = groundEntity.Get<TransformComponent>();
			groundTransform.Translation = glm::vec3(0, 500, 0);  
			groundTransform.Scale = glm::vec3(20.0f, 2.0f, 1.0f);

			auto& groundRb = groundEntity.Add<RigidBody2DComponent>();
			groundRb.BodyType = RigidBody2DComponent::Type::Static;

			auto& groundCollider = groundEntity.Add<BoxCollider2DComponent>();
			groundCollider.Size = glm::vec2(960.0f, 96.0f); 
			groundCollider.Friction = 0.5f;
			groundCollider.CategoryBits = 0x0002;
			groundCollider.MaskBits = 0xFFFF;
			groundCollider.GroupIndex = 0;

			auto& groundSprite = groundEntity.Add<SpriteRendererComponent>();
			if (whiteBlockHandle != 0)
			{
				groundSprite.spriteHandle = whiteBlockHandle;
			}
			groundSprite.tint = sf::Color::Green;

			scene->SetViewportSize(1920, 1080);
			scene->OnRuntimeStart();

			Entity entity = scene->TryGetEntityWithTag("FallingBox");
			if (!entity.IsValid())
			{
				std::cout << "    ERROR: Entity not found after runtime start!\n";
				AssertTrue(false, "Entity not found");
				scene->OnRuntimeStop();

				if (previousScene)
				{
					previousScene->OnRuntimeStart();
				}
				return;
			}

			auto& window = app->GetWindow();
			auto renderTexture = app->GetRenderTexture();

			Timer testTimer;
			sf::Clock frameClock;

			float initialY = entity.Get<TransformComponent>().Translation.y;
			int frameCount = 0;

			std::cout << "    Running gravity simulation (5 seconds)...\n";
			std::cout << "    Initial Y position: " << initialY << "\n";

			bool hitGround = false;
			float lastY = initialY;

			while (testTimer.ElapsedSeconds() < 5.0f && window.isOpen())
			{
				while (const std::optional event = window.pollEvent())
				{
					if (event->is<sf::Event::Closed>())
					{
						window.close();
						scene->OnRuntimeStop();
						return;
					}
					if (event->is<sf::Event::KeyPressed>())
					{
						auto keyEvent = event->getIf<sf::Event::KeyPressed>();
						if (keyEvent && keyEvent->code == sf::Keyboard::Key::Escape)
						{
							std::cout << "    Test skipped by user (ESC)\n";
							scene->OnRuntimeStop();
							return;
						}
					}
				}

				sf::Time dt = frameClock.restart();
				TimeStep timestep(dt.asSeconds());

				scene->OnUpdateRuntime(timestep, renderTexture);

				// Render
				window.clear(sf::Color(32, 32, 32));

				sf::Sprite displaySprite(renderTexture->getTexture());
				window.draw(displaySprite);

				window.display();

				if (frameCount % 30 == 0)
				{
					float currentY = entity.Get<TransformComponent>().Translation.y;
					std::cout << "    Frame " << frameCount << " - Y: "
						<< std::fixed << std::setprecision(2) << currentY;

					if (frameCount > 0)
					{
						float velocity = (currentY - lastY) / (30.0f / 60.0f);
						std::cout << " | Velocity: " << velocity << " units/sec";
					}
					std::cout << "\n";

					if (frameCount > 60 && std::abs(currentY - lastY) < 0.1f)
					{
						if (!hitGround)
						{
							std::cout << "    >>> Object hit the ground and stopped! <<<\n";
							hitGround = true;
						}
					}

					lastY = currentY;
				}

				frameCount++;
			}

			float finalY = entity.Get<TransformComponent>().Translation.y;
			std::cout << "    Final Y position: " << finalY << "\n";
			std::cout << "    Total distance fallen: " << (finalY - initialY) << " units\n";

			AssertTrue(finalY > initialY, "Entity should have fallen due to gravity");
			AssertTrue(hitGround, "Entity should have hit the ground and stopped");

			scene->OnRuntimeStop();

			if (previousScene)
			{
				std::cout << "    Restoring previous scene...\n";
				previousScene->OnRuntimeStart();
			}
		});

	suite.RunTests();
}