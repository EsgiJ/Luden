#include "TestFramework.h"
#include "Scene/Scene.h"
#include "Scene/SceneSerializer.h"
#include "ECS/Components/Components.h"
#include <filesystem>

using namespace Luden;
using namespace Luden::Testing;

void RunSceneSerializationTests()
{
	TestSuite suite("Scene Serialization");

	suite.AddTest("Save and Load Scene", []()
		{
			auto scene = std::make_shared<Scene>("TestScene");

			auto entity1 = scene->CreateEntity("Entity1");
			auto& transform1 = entity1.Get<TransformComponent>();
			transform1.Translation = glm::vec3(1, 2, 3);

			auto entity2 = scene->CreateEntity("Entity2");
			entity2.Add<SpriteRendererComponent>();

			scene->GetEntityManager().Update(0.0f);
			SceneSerializer serializer(scene);
			std::string filepath = "Tests/test_scene.lscene";
			serializer.Serialize(filepath);

			AssertTrue(std::filesystem::exists(filepath), "Scene file should exist");

			auto loadedScene = std::make_shared<Scene>("LoadedScene");
			SceneSerializer deserializer(loadedScene);
			deserializer.Deserialize(filepath);
			loadedScene->GetEntityManager().Update(0.0f);

			auto& entities = loadedScene->GetEntityManager().GetEntityVec();

			std::cout << "    Loaded entities: " << entities.size() << "\n";
			AssertTrue(entities.size() >= 2, "Should have at least 2 entities");

			std::filesystem::remove(filepath);
		});

	suite.AddTest("Data Integrity Check", []()
		{
			auto scene = std::make_shared<Scene>("IntegrityTest");
			auto entity = scene->CreateEntity("TestEntity");
			auto& transform = entity.Get<TransformComponent>();
			transform.Translation = glm::vec3(1.5f, 2.5f, 3.5f);
			transform.angle = 45.0f;
			transform.Scale = glm::vec3(2.0f, 2.0f, 2.0f);

			scene->GetEntityManager().Update(0.0f);
			std::string filepath = "Tests/integrity_test.lscene";
			SceneSerializer serializer(scene);
			serializer.Serialize(filepath);

			auto loadedScene = std::make_shared<Scene>("LoadedIntegrity");
			SceneSerializer deserializer(loadedScene);
			deserializer.Deserialize(filepath);
			loadedScene->GetEntityManager().Update(0.0f);

			auto& entities = loadedScene->GetEntityManager().GetEntities();

			AssertTrue(!entities.empty(), "Entities should be loaded");

			auto loaded = entities[0];
			auto& lt = loaded.Get<TransformComponent>();

			AssertEqual(lt.Translation.x, 1.5f, 0.001f, "Translation X");
			AssertEqual(lt.Translation.y, 2.5f, 0.001f, "Translation Y");
			AssertEqual(lt.Translation.z, 3.5f, 0.001f, "Translation Z");
			AssertEqual(lt.angle, 45.0f, 0.001f, "Rotation");

			std::filesystem::remove(filepath);
		});

	suite.AddBenchmark("Serialize 100 Entities", []()
		{
			auto scene = std::make_shared<Scene>("BenchScene");

			for (int i = 0; i < 100; i++)
			{
				auto entity = scene->CreateEntity("Entity" + std::to_string(i));
				entity.Get<TransformComponent>().Translation = glm::vec3(i, i, i);
			}

			SceneSerializer serializer(scene);
			serializer.Serialize("bench_scene.lscene");

			std::filesystem::remove("bench_scene.lscene");
		}, 10);

	suite.RunTests();
	suite.RunBenchmarks();
}