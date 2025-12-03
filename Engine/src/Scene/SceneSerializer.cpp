#include "Scene/SceneSerializer.h"
#include "ECS/Components/Components.h"
#include "Resource/EditorResourceManager.h"
#include "Project/Project.h"

#include <fstream>
#include "glm/ext/vector_float3.hpp"

using json = nlohmann::json;

namespace Luden
{
	SceneSerializer::SceneSerializer(std::shared_ptr<Scene> scene)
		: m_Scene(scene)
	{
	}

	bool SceneSerializer::Serialize(const std::filesystem::path& path)
	{
		json jScene;
		if (!SerializeToJSON(jScene))
			return false;

		std::ofstream out(path);
		if (!out.is_open())
			return false;

		out << jScene.dump(4);
		return true;
	}

	bool SceneSerializer::Deserialize(const std::filesystem::path& path)
	{
		std::ifstream in(path);
		if (!in.is_open())
			return false;

		json jScene;
		in >> jScene;

		return DeserializeFromJSON(jScene);
	}

	bool SceneSerializer::SerializeToJSON(json& outJson)
	{
		outJson["UUID"] = static_cast<uint64_t>(m_Scene->Handle);
		outJson["Name"] = m_Scene->GetName();

		const auto& entities = m_Scene->GetEntityManager().GetEntities();
		json jEntities = json::array();

		for (const auto& e : entities)
		{
			json jEntity;
			jEntity["UUID"] = static_cast<uint64_t>(e.UUID());
			jEntity["Tag"] = e.Tag();

			if (e.Has<RelationshipComponent>())
			{
				const auto& c = e.Get<RelationshipComponent>();

				jEntity["RelationshipComponent"]["ParentHandle"] = static_cast<uint64_t>(c.ParentHandle);

				auto& jChildren = jEntity["RelationshipComponent"]["Children"];
				for (const auto childID : c.Children)
				{
					jChildren.push_back(static_cast<uint64_t>(childID));
				}

			}

			if (e.Has<DamageComponent>())
			{
				jEntity["DamageComponent"]["damage"] = e.Get<DamageComponent>().damage;
			}

			if (e.Has<DraggableComponent>())
			{
				jEntity["DraggableComponent"]["dragging"] = e.Get<DraggableComponent>().dragging;
			}

			if (e.Has<FollowPLayerComponent>())
			{
				const auto& c = e.Get<FollowPLayerComponent>();
				jEntity["FollowPLayerComponent"] = { {"home", {c.home.x, c.home.y}}, {"speed", c.speed} };
			}

			if (e.Has<GravityComponent>())
			{
				jEntity["GravityComponent"]["gravity"] = e.Get<GravityComponent>().gravity;
			}

			if (e.Has<HealthComponent>())
			{
				const auto& c = e.Get<HealthComponent>();
				jEntity["HealthComponent"] = { 
					{"max", c.max}, 
					{"current", c.current} 
				};
			}

			if (e.Has<InputComponent>())
			{
				const auto& c = e.Get<InputComponent>();
				jEntity["InputComponent"] = {
				};
			}

			if (e.Has<Camera2DComponent>())
			{
				const auto& c = e.Get<Camera2DComponent>();
				jEntity["Camera2DComponent"] = {
					{ "Type", Camera2D::CameraTypeToString(c.Camera.GetCameraType()) },
					{ "Primary", c.Primary }
				};
			}

			if (e.Has<RigidBody2DComponent>())
			{
				const auto& c = e.Get<RigidBody2DComponent>();
				jEntity["RigidBody2DComponent"] = {
					{"BodyType", static_cast<int>(c.BodyType)},
					{"FixedRotation", c.FixedRotation},
					{"Mass", c.Mass},
					{"LinearDrag", c.LinearDrag},
					{"AngularDrag", c.AngularDrag},
					{"GravityScale", c.GravityScale}
				};
			}

			if (e.Has<BoxCollider2DComponent>())
			{
				const auto& c = e.Get<BoxCollider2DComponent>();
				jEntity["BoxCollider2DComponent"] = {
					{"Offset", {c.Offset.x, c.Offset.y}},
					{"Size", {c.Size.x, c.Size.y}},
					{"Density", c.Density},
					{"Friction", c.Friction}
				};
			}

			if (e.Has<CircleCollider2DComponent>())
			{
				const auto& c = e.Get<CircleCollider2DComponent>();
				jEntity["CircleCollider2DComponent"] = {
					{"Offset", {c.Offset.x, c.Offset.y}},
					{"Radius", c.Radius},
					{"Density", c.Density},
					{"Friction", c.Friction}
				};
			}

			if (e.Has<NativeScriptComponent>())
			{
				const auto& c = e.Get<NativeScriptComponent>();
				jEntity["NativeScriptComponent"]["ScriptHandle"] = static_cast<int64_t>(c.ScriptHandle);
			}

			if (e.Has<Animation2DComponent>())
			{
				const auto& c = e.Get<Animation2DComponent>();

				json jAnimHandles = json::array();
				for (auto handle : c.animationHandles)
				{
					jAnimHandles.push_back(static_cast<uint64_t>(handle));
				}

				jEntity["Animation2DComponent"] = {
					{ "animationHandles", jAnimHandles },
					{ "currentAnimationIndex", c.currentAnimationIndex },
					{ "repeat", c.repeat },
					{ "speed", c.speed },
					{ "currentFrame", c.currentFrame },
					{ "frameTimer", c.frameTimer }
				};
			}

			if (e.Has<TextComponent>())
			{
				jEntity["TextComponent"]["fontHandle"] = static_cast<uint64_t>(e.Get<TextComponent>().fontHandle);
			}

			if (e.Has<TextureComponent>())
			{
				jEntity["TextureComponent"]["textureHandle"] = static_cast<uint64_t>(e.Get<TextureComponent>().textureHandle);
			}

			if (e.Has<InvincibilityComponent>())
			{
				jEntity["InvincibilityComponent"]["iframes"] = e.Get<InvincibilityComponent>().iframes;
			}

			if (e.Has<LifespanComponent>())
			{
				const auto& c = e.Get<LifespanComponent>();
				jEntity["LifespanComponent"] = { {"lifespan", c.lifespan}, {"frameCreated", c.frameCreated} };
			}

			if (e.Has<PatrolComponent>())
			{
				const auto& c = e.Get<PatrolComponent>();
				json path = json::array();
				for (auto& p : c.positions)
					path.push_back({ p.x, p.y });
				jEntity["PatrolComponent"] = { {"positions", path}, {"currentPosition", c.currentPosition}, {"speed", c.speed} };
			}

			if (e.Has<StateComponent>())
			{
				const auto& c = e.Get<StateComponent>();
				jEntity["StateComponent"] = {
					{"state", c.state},
					{"previousState", c.previousState},
					{"changeAnimation", c.changeAnimation}
				};
			}

			if (e.Has<TransformComponent>())
			{
				const auto& c = e.Get<TransformComponent>();
				jEntity["TransformComponent"] = {
					{"Translation", {c.Translation.x, c.Translation.y, c.Translation.z}},
					{"Scale", {c.Scale.x, c.Scale.y, c.Scale.y}},
					{"angle", c.angle}
				};
			}

			jEntities.push_back(jEntity);
		}
		outJson["Entities"] = jEntities;
		return true;
	}

	bool SceneSerializer::DeserializeFromJSON(const json& inJson)
	{
		if (!inJson.contains("UUID") || !inJson.contains("Name") || !inJson.contains("Entities"))
			return false;

		m_Scene->Handle = inJson["UUID"].get<uint64_t>();
		m_Scene->SetName(inJson["Name"].get<std::string>());

		auto& entityManager = m_Scene->GetEntityManager();
		entityManager.Clear();

		for (const auto& jEntity : inJson["Entities"])
		{
			Entity e = m_Scene->CreateEntity(jEntity["Tag"].get<std::string>(), jEntity["UUID"].get<uint64_t>());

			if (jEntity.contains("RelationshipComponent"))
			{
				const auto& jRel = jEntity["RelationshipComponent"];

				auto& relComp = e.Get<RelationshipComponent>();

				UUID parentID = jRel["ParentHandle"].get<uint64_t>();
				relComp.ParentHandle = parentID;

				if (jRel.contains("Children"))
				{
					for (const auto& childID : jRel["Children"])
					{
						relComp.Children.push_back(childID.get<uint64_t>());
					}
				}
			}

			if (jEntity.contains("DamageComponent"))
			{
				e.Add<DamageComponent>(jEntity["DamageComponent"]["damage"]);
			}

			if (jEntity.contains("DraggableComponent"))
			{
				e.Add<DraggableComponent>(jEntity["DraggableComponent"]["dragging"].get<bool>());
			}

			if (jEntity.contains("FollowPLayerComponent"))
			{
				auto p = jEntity["FollowPLayerComponent"]["home"];
				e.Add<FollowPLayerComponent>(glm::vec2(p[0], p[1]), jEntity["FollowPLayerComponent"]["speed"]);
			}

			if (jEntity.contains("GravityComponent"))
			{
				e.Add<GravityComponent>(jEntity["GravityComponent"]["gravity"]);

			}

			if (jEntity.contains("HealthComponent"))
			{
				e.Add<HealthComponent>(jEntity["HealthComponent"]["max"], jEntity["HealthComponent"]["current"]);
			}

			if (jEntity.contains("InputComponent"))
			{
				e.Add<InputComponent>();
				auto& inputComponent = e.Get<InputComponent>();
			}

			if (jEntity.contains("Camera2DComponent"))
			{
				e.Add<Camera2DComponent>();
				auto& camera2DComponent = e.Get<Camera2DComponent>();

				Camera2D::Type type = Camera2D::CameraTypeFromString(jEntity["Camera2DComponent"]["Type"]);
				camera2DComponent.Camera.SetCameraType(type);
				camera2DComponent.Primary = jEntity["Camera2DComponent"]["Primary"];
			}

			if (jEntity.contains("RigidBody2DComponent"))
			{
				e.Add<RigidBody2DComponent>();
				auto& c = e.Get<RigidBody2DComponent>();
				c.BodyType = static_cast<RigidBody2DComponent::Type>(jEntity["RigidBody2DComponent"]["BodyType"].get<int>());
				c.FixedRotation = jEntity["RigidBody2DComponent"]["FixedRotation"].get<bool>();
				c.Mass = jEntity["RigidBody2DComponent"]["Mass"].get<float>();
				c.LinearDrag = jEntity["RigidBody2DComponent"]["LinearDrag"].get<float>();
				c.AngularDrag = jEntity["RigidBody2DComponent"]["AngularDrag"].get<float>();
				c.GravityScale = jEntity["RigidBody2DComponent"]["GravityScale"].get<float>();
			}

			if (jEntity.contains("BoxCollider2DComponent"))
			{
				e.Add<BoxCollider2DComponent>();
				auto& c = e.Get<BoxCollider2DComponent>();

				auto offset = jEntity["BoxCollider2DComponent"]["Offset"];
				auto size = jEntity["BoxCollider2DComponent"]["Size"];

				c.Offset = { offset[0].get<float>(), offset[1].get<float>() };
				c.Size = { size[0].get<float>(), size[1].get<float>() };
				c.Density = jEntity["BoxCollider2DComponent"]["Density"].get<float>();
				c.Friction = jEntity["BoxCollider2DComponent"]["Friction"].get<float>();
			}

			if (jEntity.contains("CircleCollider2DComponent"))
			{
				e.Add<CircleCollider2DComponent>();
				auto& c = e.Get<CircleCollider2DComponent>();

				auto offset = jEntity["CircleCollider2DComponent"]["Offset"];
				c.Offset = { offset[0].get<float>(), offset[1].get<float>() };
				c.Radius = jEntity["CircleCollider2DComponent"]["Radius"].get<float>();
				c.Density = jEntity["CircleCollider2DComponent"]["Density"].get<float>();
				c.Friction = jEntity["CircleCollider2DComponent"]["Friction"].get<float>();
			}

			if (jEntity.contains("InvincibilityComponent"))
			{
				e.Add<InvincibilityComponent>(jEntity["InvincibilityComponent"]["iframes"]);
			}

			if (jEntity.contains("LifespanComponent"))
			{
				e.Add<LifespanComponent>(jEntity["LifespanComponent"]["lifespan"], jEntity["LifespanComponent"]["frameCreated"]);
			}

			if (jEntity.contains("PatrolComponent"))
			{
				std::vector<glm::vec2> points;
				for (const auto& p : jEntity["PatrolComponent"]["positions"])
					points.emplace_back(p[0], p[1]);

				auto& c = e.Add<PatrolComponent>(points, jEntity["PatrolComponent"]["speed"]);
				c.currentPosition = jEntity["PatrolComponent"]["currentPosition"];
			}

			if (jEntity.contains("StateComponent"))
			{
				e.Add<StateComponent>(
					jEntity["StateComponent"]["state"]
				);
				auto stateComponent = e.Get<StateComponent>();
				stateComponent.previousState = jEntity["StateComponent"]["previousState"],
				stateComponent.changeAnimation = jEntity["StateComponent"]["changeAnimation"];
			}

			if (jEntity.contains("TransformComponent"))
			{
				auto translation = jEntity["TransformComponent"]["Translation"];
				auto scale = jEntity["TransformComponent"]["Scale"];

				e.Add<TransformComponent>(
					glm::vec3(translation[0], translation[1], translation[2]),
					glm::vec3(scale[0], scale[1], scale[2]),
					jEntity["TransformComponent"]["angle"]
				);
			}

			if (jEntity.contains("NativeScriptComponent"))
			{
				auto& c = e.Add<NativeScriptComponent>();
				c.ScriptHandle = jEntity["NativeScriptComponent"]["ScriptHandle"].get<uint64_t>();

				if (c.ScriptHandle != 0)
					c.BindFromHandle(c.ScriptHandle);
			}

			if (jEntity.contains("Animation2DComponent"))
			{
				const auto& jAnim = jEntity["Animation2DComponent"];

				auto& c = e.Add<Animation2DComponent>();

				if (jAnim.contains("animationHandles"))
				{
					for (const auto& handleJson : jAnim["animationHandles"])
					{
						ResourceHandle handle = handleJson.get<uint64_t>();
						c.animationHandles.push_back(handle);
					}
				}

				c.currentAnimationIndex = jAnim.value("currentAnimationIndex", 0);
				c.repeat = jAnim.value("repeat", true);
				c.speed = jAnim.value("speed", 1);
				c.currentFrame = jAnim.value("currentFrame", 0);
				c.frameTimer = jAnim.value("frameTimer", 0);
			}

			if (jEntity.contains("TextComponent"))
			{
				e.Add<TextComponent>(jEntity["TextComponent"]["fontHandle"].get<uint64_t>());
			}

			if (jEntity.contains("TextureComponent"))
			{
				e.Add<TextureComponent>(jEntity["TextureComponent"]["textureHandle"].get<uint64_t>());
			}
		}

		return true;
	}

	bool SceneSerializer::SerializeToResourcePack(FileStreamWriter& stream, ResourceSerializationInfo& outInfo)
	{
		json outJson;
		SerializeToJSON(outJson);

		outInfo.Offset = stream.GetStreamPosition();
		std::string jsonStr = outJson.dump();
		stream.WriteString(jsonStr);
		outInfo.Size = stream.GetStreamPosition() - outInfo.Offset;
		return true;
	}

	bool SceneSerializer::DeserializeFromResourcePack(FileStreamReader& stream, const ResourcePackFile::SceneInfo& sceneInfo)
	{
		stream.SetStreamPosition(sceneInfo.PackedOffset);

		std::string jsonStr;
		stream.ReadString(jsonStr);

		json inJson = json::parse(jsonStr);
		return DeserializeFromJSON(inJson);
	}

}

