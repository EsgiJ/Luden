#include "TestFramework.h"
#include "ECS/EntityMemoryPool.h"
#include "Scene/Scene.h"
#include "ECS/Components/Components.h"

using namespace Luden;
using namespace Luden::Testing;

void RunEntityMemoryPoolTests()
{
	TestSuite suite("EntityMemoryPool");

	suite.AddTest("Basic Entity Creation", []()
		{
			auto& pool = EntityMemoryPool::Instance();
			auto scene = std::make_shared<Scene>("TestScene");

			auto entity1 = pool.AddEntity("TestEntity1", scene.get());
			auto entity2 = pool.AddEntity("TestEntity2", scene.get());

			AssertTrue(pool.Exists(entity1.UUID()), "Entity1 should exist");
			AssertTrue(pool.Exists(entity2.UUID()), "Entity2 should exist");
		});

	suite.AddTest("Component Add/Get/Remove", []()
		{
			auto& pool = EntityMemoryPool::Instance();
			auto scene = std::make_shared<Scene>("TestScene");

			auto entity = pool.AddEntity("TestEntity", scene.get());

			pool.AddComponent<TransformComponent>(entity.UUID());
			AssertTrue(pool.HasComponent<TransformComponent>(entity.UUID()),
				"Entity should have TransformComponent");

			auto& transform = pool.GetComponent<TransformComponent>(entity.UUID());
			transform.Translation = glm::vec3(1, 2, 3);

			auto& retrieved = pool.GetComponent<TransformComponent>(entity.UUID());
			AssertEqual(retrieved.Translation.x, 1.0f, 0.001f, "X should be 1");
			AssertEqual(retrieved.Translation.y, 2.0f, 0.001f, "Y should be 2");
			AssertEqual(retrieved.Translation.z, 3.0f, 0.001f, "Z should be 3");

			pool.RemoveComponent<TransformComponent>(entity.UUID());
			AssertFalse(pool.HasComponent<TransformComponent>(entity.UUID()),
				"Entity should not have TransformComponent after removal");
		});

	suite.AddTest("Entity Active State", []()
		{
			auto& pool = EntityMemoryPool::Instance();
			auto scene = std::make_shared<Scene>("TestScene");

			auto entity = pool.AddEntity("TestEntity", scene.get());

			AssertTrue(pool.IsActive(entity.UUID()), "Entity should be active by default");

			pool.SetActive(entity.UUID(), false);
			AssertFalse(pool.IsActive(entity.UUID()), "Entity should be inactive");

			pool.SetActive(entity.UUID(), true);
			AssertTrue(pool.IsActive(entity.UUID()), "Entity should be active again");
		});

	suite.AddBenchmark("Create 1,000 Entities", []()
		{
			auto& pool = EntityMemoryPool::Instance();
			auto scene = std::make_shared<Scene>("BenchScene");

			for (int i = 0; i < 1000; i++)
			{
				pool.AddEntity("Entity" + std::to_string(i), scene.get());
			}
		}, 10);

	suite.AddBenchmark("Add Components to 1,000 Entities", []()
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
			}
		}, 10);

	suite.RunTests();
	suite.RunBenchmarks();
}