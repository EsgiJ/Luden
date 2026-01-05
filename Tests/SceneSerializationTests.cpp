#include "TestFramework.h"
#include "Scene/Scene.h"
#include "Scene/SceneSerializer.h"
#include "ECS/Components/Components.h"
#include "Resource/ResourceManager.h"
#include "Graphics/Sprite.h"
#include "Project/Project.h"
#include <filesystem>

using namespace Luden;
using namespace Luden::Testing;

void RunSceneSerializationTests()
{
	TestSuite suite("Scene Serialization");

	std::cout << "\n========================================\n";
	std::cout << "Scene Serialization Tests\n";
	std::cout << "========================================\n\n";

	std::filesystem::create_directories("Tests");

	suite.AddTest("Save and Load Scene", []()
		{
			std::cout << "    Creating test scene...\n";

			auto scene = std::make_shared<Scene>("TestScene");

			auto entity1 = scene->CreateEntity("Entity1");
			auto& transform1 = entity1.Get<TransformComponent>();
			transform1.Translation = glm::vec3(1, 2, 3);
			transform1.angle = 45.0f;
			transform1.Scale = glm::vec3(2, 2, 1);

			auto entity2 = scene->CreateEntity("Entity2");
			auto& sprite = entity2.Add<SpriteRendererComponent>();
			sprite.tint = sf::Color::Red;

			scene->GetEntityManager().Update(0.0f);

			std::cout << "    Serializing scene...\n";
			SceneSerializer serializer(scene);
			std::string filepath = "Tests/test_scene.lscene";
			serializer.Serialize(filepath);

			AssertTrue(std::filesystem::exists(filepath), "Scene file should exist");
			std::cout << "    Scene saved to: " << filepath << "\n";

			std::cout << "    Deserializing scene...\n";
			auto loadedScene = std::make_shared<Scene>("LoadedScene");
			SceneSerializer deserializer(loadedScene);
			deserializer.Deserialize(filepath);

			loadedScene->GetEntityManager().Update(0.0f);

			auto& entities = loadedScene->GetEntityManager().GetEntityVec();
			std::cout << "    Loaded " << entities.size() << " entities\n";

			AssertTrue(entities.size() >= 2, "Should have at least 2 entities");

			std::filesystem::remove(filepath);
			std::cout << "    Test cleanup complete\n";
		});

	suite.AddTest("Data Integrity Check", []()
		{
			std::cout << "    Creating scene with specific data...\n";

			auto scene = std::make_shared<Scene>("IntegrityTest");

			auto entity = scene->CreateEntity("TestEntity");
			auto& transform = entity.Get<TransformComponent>();
			transform.Translation = glm::vec3(1.5f, 2.5f, 3.5f);
			transform.angle = 45.0f;
			transform.Scale = glm::vec3(2.0f, 2.0f, 2.0f);

			auto& sprite = entity.Add<SpriteRendererComponent>();
			sprite.tint = sf::Color(128, 64, 255, 200);

			scene->GetEntityManager().Update(0.0f);

			std::string filepath = "Tests/integrity_test.lscene";

			std::cout << "    Saving scene...\n";
			SceneSerializer serializer(scene);
			serializer.Serialize(filepath);

			std::cout << "    Loading scene...\n";
			auto loadedScene = std::make_shared<Scene>("LoadedIntegrity");
			SceneSerializer deserializer(loadedScene);
			deserializer.Deserialize(filepath);

			loadedScene->GetEntityManager().Update(0.0f);

			auto& entities = loadedScene->GetEntityManager().GetEntities();
			AssertTrue(!entities.empty(), "Entities should be loaded");

			std::cout << "    Verifying data integrity...\n";
			auto loaded = entities[0];

			auto& lt = loaded.Get<TransformComponent>();
			AssertEqual(lt.Translation.x, 1.5f, 0.001f, "Translation.x");
			AssertEqual(lt.Translation.y, 2.5f, 0.001f, "Translation.y");
			AssertEqual(lt.Translation.z, 3.5f, 0.001f, "Translation.z");
			AssertEqual(lt.angle, 45.0f, 0.001f, "Rotation");
			AssertEqual(lt.Scale.x, 2.0f, 0.001f, "Scale.x");
			AssertEqual(lt.Scale.y, 2.0f, 0.001f, "Scale.y");

			auto& ls = loaded.Get<SpriteRendererComponent>();
			AssertTrue(ls.tint.r == 128, "Sprite color R");
			AssertTrue(ls.tint.g == 64, "Sprite color G");
			AssertTrue(ls.tint.b == 255, "Sprite color B");
			AssertTrue(ls.tint.a == 200, "Sprite color A");

			std::cout << "    All data verified successfully\n";

			std::filesystem::remove(filepath);
		});

	suite.AddTest("Complex Scene with Multiple Component Types", []()
		{
			std::cout << "    Creating complex scene...\n";

			auto scene = std::make_shared<Scene>("ComplexScene");

			// Camera entity
			auto camera = scene->CreateEntity("MainCamera");
			auto& cam = camera.Add<Camera2DComponent>();
			cam.Primary = true;

			auto player = scene->CreateEntity("Player");
			player.Get<TransformComponent>().Translation = glm::vec3(100, 200, 0);
			player.Add<SpriteRendererComponent>().tint = sf::Color::Blue;
			auto& playerRb = player.Add<RigidBody2DComponent>();
			playerRb.BodyType = RigidBody2DComponent::Type::Dynamic;
			playerRb.GravityScale = 1.5f;
			auto& playerCol = player.Add<BoxCollider2DComponent>();
			playerCol.Size = glm::vec2(32, 32);
			playerCol.Density = 1.0f;

			auto ground = scene->CreateEntity("Ground");
			ground.Get<TransformComponent>().Translation = glm::vec3(0, 400, 0);
			ground.Add<SpriteRendererComponent>().tint = sf::Color::Green;
			ground.Add<RigidBody2DComponent>().BodyType = RigidBody2DComponent::Type::Static;
			ground.Add<BoxCollider2DComponent>().Size = glm::vec2(800, 50);

			scene->GetEntityManager().Update(0.0f);

			std::cout << "    Created " << scene->GetEntityManager().GetEntityVec().size() << " entities\n";

			std::string filepath = "Tests/complex_scene.lscene";

			std::cout << "    Serializing...\n";
			SceneSerializer serializer(scene);
			serializer.Serialize(filepath);

			std::cout << "    Deserializing...\n";
			auto loadedScene = std::make_shared<Scene>("LoadedComplex");
			SceneSerializer deserializer(loadedScene);
			deserializer.Deserialize(filepath);

			loadedScene->GetEntityManager().Update(0.0f);

			auto& entities = loadedScene->GetEntityManager().GetEntityVec();
			std::cout << "    Loaded " << entities.size() << " entities\n";

			AssertTrue(entities.size() >= 3, "Should have at least 3 entities");

			Entity loadedPlayer = loadedScene->TryGetEntityWithTag("Player");
			AssertTrue(loadedPlayer.IsValid(), "Player entity should exist");
			AssertTrue(loadedPlayer.Has<TransformComponent>(), "Player should have Transform");
			AssertTrue(loadedPlayer.Has<SpriteRendererComponent>(), "Player should have Sprite");
			AssertTrue(loadedPlayer.Has<RigidBody2DComponent>(), "Player should have RigidBody");
			AssertTrue(loadedPlayer.Has<BoxCollider2DComponent>(), "Player should have Collider");

			auto& loadedRb = loadedPlayer.Get<RigidBody2DComponent>();
			AssertEqual(loadedRb.GravityScale, 1.5f, 0.001f, "GravityScale preserved");

			std::cout << "    Complex scene verified successfully\n";

			std::filesystem::remove(filepath);
		});

	suite.AddTest("Empty Scene Serialization", []()
		{
			std::cout << "    Testing empty scene...\n";

			auto scene = std::make_shared<Scene>("EmptyScene");
			std::string filepath = "Tests/empty_scene.lscene";

			SceneSerializer serializer(scene);
			serializer.Serialize(filepath);

			auto loadedScene = std::make_shared<Scene>("LoadedEmpty");
			SceneSerializer deserializer(loadedScene);
			deserializer.Deserialize(filepath);

			AssertTrue(loadedScene->GetEntityManager().GetEntityVec().size() == 0,
				"Empty scene should have 0 entities");

			std::cout << "    Empty scene handled correctly\n";

			std::filesystem::remove(filepath);
		});

	std::cout << "\n========================================\n";
	std::cout << "Performance Benchmarks\n";
	std::cout << "========================================\n\n";

	suite.AddBenchmark("Serialize Scene with 10 Entities", []()
		{
			auto scene = std::make_shared<Scene>("BenchScene");

			for (int i = 0; i < 10; i++)
			{
				auto entity = scene->CreateEntity("Entity" + std::to_string(i));
				entity.Get<TransformComponent>().Translation = glm::vec3(i, i, 0);
				entity.Add<SpriteRendererComponent>();
			}

			SceneSerializer serializer(scene);
			serializer.Serialize("Tests/bench_10.lscene");
			std::filesystem::remove("Tests/bench_10.lscene");
		}, 10);

	suite.AddBenchmark("Serialize Scene with 100 Entities", []()
		{
			auto scene = std::make_shared<Scene>("BenchScene");

			for (int i = 0; i < 100; i++)
			{
				auto entity = scene->CreateEntity("Entity" + std::to_string(i));
				entity.Get<TransformComponent>().Translation = glm::vec3(i, i, 0);
				entity.Add<SpriteRendererComponent>();
			}

			SceneSerializer serializer(scene);
			serializer.Serialize("Tests/bench_100.lscene");
			std::filesystem::remove("Tests/bench_100.lscene");
		}, 10);

	suite.AddBenchmark("Serialize Scene with 1000 Entities", []()
		{
			auto scene = std::make_shared<Scene>("BenchScene");

			for (int i = 0; i < 1000; i++)
			{
				auto entity = scene->CreateEntity("Entity" + std::to_string(i));
				entity.Get<TransformComponent>().Translation = glm::vec3(i, i, 0);
				entity.Add<SpriteRendererComponent>();
			}

			SceneSerializer serializer(scene);
			serializer.Serialize("Tests/bench_1000.lscene");
			std::filesystem::remove("Tests/bench_1000.lscene");
		}, 5);

	suite.AddBenchmark("Deserialize Scene with 100 Entities", []()
		{
			auto scene = std::make_shared<Scene>("BenchScene");
			for (int i = 0; i < 100; i++)
			{
				auto entity = scene->CreateEntity("Entity" + std::to_string(i));
				entity.Get<TransformComponent>().Translation = glm::vec3(i, i, 0);
			}
			SceneSerializer serializer(scene);
			serializer.Serialize("Tests/bench_deserialize.lscene");

			auto loadedScene = std::make_shared<Scene>("Loaded");
			SceneSerializer deserializer(loadedScene);
			deserializer.Deserialize("Tests/bench_deserialize.lscene");

			std::filesystem::remove("Tests/bench_deserialize.lscene");
		}, 10);

	suite.RunTests();
	suite.RunBenchmarks();
}