#include "TestFramework.h"
#include "ECS/EntityMemoryPool.h"
#include "Scene/Scene.h"
#include "ECS/Components/Components.h"

using namespace Luden;
using namespace Luden::Testing;

void RunEntityMemoryPoolTests()
{
	TestSuite suite("EntityMemoryPool");

	std::cout << "\n========================================\n";
	std::cout << "Entity Memory Pool Tests\n";
	std::cout << "========================================\n\n";

	suite.AddTest("Basic Entity Creation", []()
		{
			std::cout << "    Testing entity creation...\n";

			auto& pool = EntityMemoryPool::Instance();
			auto scene = std::make_shared<Scene>("TestScene");

			auto entity1 = pool.AddEntity("TestEntity1", scene.get());
			auto entity2 = pool.AddEntity("TestEntity2", scene.get());

			AssertTrue(pool.Exists(entity1.UUID()), "Entity1 should exist");
			AssertTrue(pool.Exists(entity2.UUID()), "Entity2 should exist");

			std::cout << "    Created 2 entities successfully\n";
		});

	suite.AddTest("Component Add/Get/Remove", []()
		{
			std::cout << "    Testing component operations...\n";

			auto& pool = EntityMemoryPool::Instance();
			auto scene = std::make_shared<Scene>("TestScene");
			auto entity = pool.AddEntity("TestEntity", scene.get());

			// Add component
			pool.AddComponent<TransformComponent>(entity.UUID());
			AssertTrue(pool.HasComponent<TransformComponent>(entity.UUID()),
				"Entity should have TransformComponent");
			std::cout << "    Component added successfully\n";

			// Modify component
			auto& transform = pool.GetComponent<TransformComponent>(entity.UUID());
			transform.Translation = glm::vec3(1, 2, 3);
			transform.angle = 45.0f;
			transform.Scale = glm::vec3(2, 2, 1);

			// Verify data
			auto& retrieved = pool.GetComponent<TransformComponent>(entity.UUID());
			AssertEqual(retrieved.Translation.x, 1.0f, 0.001f, "Translation.x");
			AssertEqual(retrieved.Translation.y, 2.0f, 0.001f, "Translation.y");
			AssertEqual(retrieved.Translation.z, 3.0f, 0.001f, "Translation.z");
			AssertEqual(retrieved.angle, 45.0f, 0.001f, "Rotation");
			AssertEqual(retrieved.Scale.x, 2.0f, 0.001f, "Scale.x");
			std::cout << "    Component data verified\n";

			// Remove component
			pool.RemoveComponent<TransformComponent>(entity.UUID());
			AssertFalse(pool.HasComponent<TransformComponent>(entity.UUID()),
				"Entity should not have TransformComponent after removal");
			std::cout << "    Component removed successfully\n";
		});

	suite.AddTest("Multiple Components", []()
		{
			std::cout << "    Testing multiple components on single entity...\n";

			auto& pool = EntityMemoryPool::Instance();
			auto scene = std::make_shared<Scene>("TestScene");
			auto entity = pool.AddEntity("MultiComponentEntity", scene.get());

			// Add multiple components
			pool.AddComponent<TransformComponent>(entity.UUID());
			pool.AddComponent<SpriteRendererComponent>(entity.UUID());
			pool.AddComponent<RigidBody2DComponent>(entity.UUID());

			AssertTrue(pool.HasComponent<TransformComponent>(entity.UUID()),
				"Should have TransformComponent");
			AssertTrue(pool.HasComponent<SpriteRendererComponent>(entity.UUID()),
				"Should have SpriteRendererComponent");
			AssertTrue(pool.HasComponent<RigidBody2DComponent>(entity.UUID()),
				"Should have RigidBody2DComponent");

			std::cout << "    3 components added successfully\n";

			// Verify each component is independent
			auto& transform = pool.GetComponent<TransformComponent>(entity.UUID());
			transform.Translation = glm::vec3(10, 20, 30);

			auto& sprite = pool.GetComponent<SpriteRendererComponent>(entity.UUID());
			sprite.tint = sf::Color::Red;

			auto& rb = pool.GetComponent<RigidBody2DComponent>(entity.UUID());
			rb.GravityScale = 2.0f;

			// Verify modifications
			AssertEqual(pool.GetComponent<TransformComponent>(entity.UUID()).Translation.x, 10.0f, 0.001f,
				"Transform data preserved");
			AssertTrue(pool.GetComponent<SpriteRendererComponent>(entity.UUID()).tint == sf::Color::Red,
				"Sprite data preserved");
			AssertEqual(pool.GetComponent<RigidBody2DComponent>(entity.UUID()).GravityScale, 2.0f, 0.001f,
				"RigidBody data preserved");

			std::cout << "    All component data independent and correct\n";
		});

	suite.AddTest("Entity Active State", []()
		{
			std::cout << "    Testing entity active state...\n";

			auto& pool = EntityMemoryPool::Instance();
			auto scene = std::make_shared<Scene>("TestScene");
			auto entity = pool.AddEntity("TestEntity", scene.get());

			AssertTrue(pool.IsActive(entity.UUID()), "Entity should be active by default");
			std::cout << "    Entity active by default\n";

			pool.SetActive(entity.UUID(), false);
			AssertFalse(pool.IsActive(entity.UUID()), "Entity should be inactive");
			std::cout << "    Entity deactivated\n";

			pool.SetActive(entity.UUID(), true);
			AssertTrue(pool.IsActive(entity.UUID()), "Entity should be active again");
			std::cout << "    Entity reactivated\n";
		});

	suite.AddTest("Entity Deletion", []()
		{
			std::cout << "    Testing entity deletion...\n";

			auto& pool = EntityMemoryPool::Instance();
			auto scene = std::make_shared<Scene>("TestScene");

			auto entity = pool.AddEntity("ToBeDeleted", scene.get());
			UUID entityId = entity.UUID();

			pool.AddComponent<TransformComponent>(entityId);
			pool.AddComponent<SpriteRendererComponent>(entityId);

			AssertTrue(pool.Exists(entityId), "Entity should exist");
			AssertTrue(pool.HasComponent<TransformComponent>(entityId), "Should have components");

			pool.DestroyEntity(entityId);

			AssertFalse(pool.Exists(entityId), "Entity should not exist after deletion");
			std::cout << "    Entity and all components deleted\n";
		});

	std::cout << "\n========================================\n";
	std::cout << "Performance Benchmarks\n";
	std::cout << "========================================\n\n";

	suite.AddBenchmark("Create 1,000 Entities", []()
		{
			auto& pool = EntityMemoryPool::Instance();
			auto scene = std::make_shared<Scene>("BenchScene");

			for (int i = 0; i < 1000; i++)
			{
				pool.AddEntity("Entity" + std::to_string(i), scene.get());
			}
		}, 10);

	suite.AddBenchmark("Create 10,000 Entities", []()
		{
			auto& pool = EntityMemoryPool::Instance();
			auto scene = std::make_shared<Scene>("BenchScene");

			for (int i = 0; i < 10000; i++)
			{
				pool.AddEntity("Entity" + std::to_string(i), scene.get());
			}
		}, 5);

	suite.AddBenchmark("Add Single Component to 1,000 Entities", []()
		{
			auto& pool = EntityMemoryPool::Instance();
			auto scene = std::make_shared<Scene>("BenchScene");

			std::vector<UUID> entities;
			for (int i = 0; i < 1000; i++)
			{
				auto entity = pool.AddEntity("Entity" + std::to_string(i), scene.get());
				entities.push_back(entity.UUID());
			}

			for (auto& uuid : entities)
			{
				pool.AddComponent<TransformComponent>(uuid);
			}
		}, 10);

	suite.AddBenchmark("Add Multiple Components to 1,000 Entities", []()
		{
			auto& pool = EntityMemoryPool::Instance();
			auto scene = std::make_shared<Scene>("BenchScene");

			std::vector<UUID> entities;
			for (int i = 0; i < 1000; i++)
			{
				auto entity = pool.AddEntity("Entity" + std::to_string(i), scene.get());
				entities.push_back(entity.UUID());
			}

			for (auto& uuid : entities)
			{
				pool.AddComponent<TransformComponent>(uuid);
				pool.AddComponent<SpriteRendererComponent>(uuid);
				pool.AddComponent<RigidBody2DComponent>(uuid);
			}
		}, 10);

	suite.AddBenchmark("Component Access (1,000 iterations)", []()
		{
			auto& pool = EntityMemoryPool::Instance();
			auto scene = std::make_shared<Scene>("BenchScene");

			auto entity = pool.AddEntity("TestEntity", scene.get());
			pool.AddComponent<TransformComponent>(entity.UUID());

			for (int i = 0; i < 1000; i++)
			{
				auto& transform = pool.GetComponent<TransformComponent>(entity.UUID());
				transform.Translation.x += 0.1f;
			}
		}, 10);

	suite.RunTests();
	suite.RunBenchmarks();
}