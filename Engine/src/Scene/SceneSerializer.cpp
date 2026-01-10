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
					{ "Type", Camera2D::CameraTypeToString(c.Camera.GetType()) },
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
					{"Friction", c.Friction},
					{"Restitution", c.Restitution},
					{"CategoryBits", c.CategoryBits},  
					{"MaskBits", c.MaskBits},          
					{"GroupIndex", c.GroupIndex}
				};
			}

			if (e.Has<CircleCollider2DComponent>())
			{
				const auto& c = e.Get<CircleCollider2DComponent>();
				jEntity["CircleCollider2DComponent"] = {
					{"Offset", {c.Offset.x, c.Offset.y}},
					{"Radius", c.Radius},
					{"Density", c.Density},
					{"Friction", c.Friction},
					{"Restitution", c.Restitution},
					{"CategoryBits", c.CategoryBits},  
					{"MaskBits", c.MaskBits},          
					{"GroupIndex", c.GroupIndex}
				};
			}

			if (e.Has<PrefabComponent>())
			{
				const auto& c = e.Get<PrefabComponent>();
				jEntity["PrefabComponent"] = {
					{"PrefabID", static_cast<uint64_t>(c.PrefabID)},
					{"EntityID", static_cast<uint64_t>(c.EntityID)}
				};
			}

			if (e.Has<NativeScriptComponent>())
			{
				const auto& c = e.Get<NativeScriptComponent>();
				jEntity["NativeScriptComponent"]["ScriptHandle"] = static_cast<int64_t>(c.ScriptHandle);
			}

			if (e.Has<SpriteRendererComponent>())
			{
				const auto& c = e.Get<SpriteRendererComponent>();
				jEntity["SpriteRendererComponent"] = {
					{"spriteHandle", static_cast<uint64_t>(c.spriteHandle)},
					{"tint", {c.tint.r, c.tint.g, c.tint.b, c.tint.a}}
				};
			}

			if (e.Has<SpriteAnimatorComponent>())
			{
				const auto& c = e.Get<SpriteAnimatorComponent>();

				json jAnimHandles = json::array();
				for (auto handle : c.animationHandles)
				{
					jAnimHandles.push_back(static_cast<uint64_t>(handle));
				}

				jEntity["SpriteAnimatorComponent"] = {
					{"animationHandles", jAnimHandles},
					{"currentAnimationIndex", c.currentAnimationIndex},
					{"currentFrame", c.currentFrame},
					{"frameTimer", c.frameTimer},
					{"playbackSpeed", c.playbackSpeed},
					{"tint", {c.tint.r, c.tint.g, c.tint.b, c.tint.a}}
				};
			}

			if (e.Has<TextComponent>())
			{
				const auto& c = e.Get<TextComponent>();
				jEntity["TextComponent"] = {
					{"fontHandle", static_cast<uint64_t>(c.fontHandle)},
					{"text", c.text},
					{"characterSize", c.characterSize},
					{"fillColor", {c.fillColor.r, c.fillColor.g, c.fillColor.b, c.fillColor.a}},
					{"outlineColor", {c.outlineColor.r, c.outlineColor.g, c.outlineColor.b, c.outlineColor.a}},
					{"outlineThickness", c.outlineThickness},
					{"letterSpacing", c.letterSpacing},
					{"lineSpacing", c.lineSpacing},
					{"style", c.style},
					{"lineAlignment", static_cast<int>(c.lineAlignment)},
					{"textOrientation", static_cast<int>(c.textOrientation)}
				};
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
			Entity e = m_Scene->CreateEntityImmediate(jEntity["Tag"].get<std::string>(), jEntity["UUID"].get<uint64_t>());

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
				camera2DComponent.Camera.SetType(type);
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
				c.Restitution = jEntity["BoxCollider2DComponent"]["Restitution"].get<float>();
				c.CategoryBits = jEntity["BoxCollider2DComponent"].value("CategoryBits", 0x0001);
				c.MaskBits = jEntity["BoxCollider2DComponent"].value("MaskBits", 0xFFFF);
				c.GroupIndex = jEntity["BoxCollider2DComponent"].value("GroupIndex", 0);
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
				c.Restitution = jEntity["CircleCollider2DComponent"]["Restitution"].get<float>();
				c.CategoryBits = jEntity["CircleCollider2DComponent"].value("CategoryBits", 0x0001);
				c.MaskBits = jEntity["CircleCollider2DComponent"].value("MaskBits", 0xFFFF);
				c.GroupIndex = jEntity["CircleCollider2DComponent"].value("GroupIndex", 0);
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

			if (jEntity.contains("PrefabComponent"))
			{
				const auto& jPrefab = jEntity["PrefabComponent"];

				auto& c = e.Add<PrefabComponent>();
				c.PrefabID = jPrefab["PrefabID"].get<uint64_t>();
				c.EntityID = jPrefab["EntityID"].get<uint64_t>();
			}

			if (jEntity.contains("NativeScriptComponent"))
			{
				auto& c = e.Add<NativeScriptComponent>();
				c.ScriptHandle = jEntity["NativeScriptComponent"]["ScriptHandle"].get<uint64_t>();

				if (c.ScriptHandle != 0)
					c.BindFromHandle(c.ScriptHandle);
			}

			if (jEntity.contains("SpriteRendererComponent"))
			{
				const auto& jSprite = jEntity["SpriteRendererComponent"];

				auto& c = e.Add<SpriteRendererComponent>();
				c.spriteHandle = jSprite["spriteHandle"].get<uint64_t>();

				if (jSprite.contains("tint"))
				{
					const auto& jTint = jSprite["tint"];
					c.tint = sf::Color(
						jTint[0].get<uint8_t>(),
						jTint[1].get<uint8_t>(),
						jTint[2].get<uint8_t>(),
						jTint[3].get<uint8_t>()
					);
				}
			}

			if (jEntity.contains("SpriteAnimatorComponent"))
			{
				const auto& jAnim = jEntity["SpriteAnimatorComponent"];

				auto& c = e.Add<SpriteAnimatorComponent>();

				if (jAnim.contains("animationHandles"))
				{
					for (const auto& handleJson : jAnim["animationHandles"])
					{
						c.animationHandles.push_back(handleJson.get<uint64_t>());
					}
				}

				c.currentAnimationIndex = jAnim.value("currentAnimationIndex", 0);
				c.currentFrame = jAnim.value("currentFrame", 0);
				c.frameTimer = jAnim.value("frameTimer", 0.0f);
				c.playbackSpeed = jAnim.value("playbackSpeed", 1.0f);

				if (jAnim.contains("tint"))
				{
					const auto& jTint = jAnim["tint"];
					c.tint = sf::Color(
						jTint[0].get<uint8_t>(),
						jTint[1].get<uint8_t>(),
						jTint[2].get<uint8_t>(),
						jTint[3].get<uint8_t>()
					);
				}

			}

			if (jEntity.contains("TextComponent"))
			{
				const auto& jText = jEntity["TextComponent"];

				auto& c = e.Add<TextComponent>();
				c.fontHandle = jText["fontHandle"].get<uint64_t>();
				c.text = jText["text"].get<std::string>();
				c.characterSize = jText.value("characterSize", 30u);

				if (jText.contains("fillColor"))
				{
					const auto& jColor = jText["fillColor"];
					c.fillColor = sf::Color(
						jColor[0].get<uint8_t>(),
						jColor[1].get<uint8_t>(),
						jColor[2].get<uint8_t>(),
						jColor[3].get<uint8_t>()
					);
				}

				if (jText.contains("outlineColor"))
				{
					const auto& jColor = jText["outlineColor"];
					c.outlineColor = sf::Color(
						jColor[0].get<uint8_t>(),
						jColor[1].get<uint8_t>(),
						jColor[2].get<uint8_t>(),
						jColor[3].get<uint8_t>()
					);
				}

				c.outlineThickness = jText.value("outlineThickness", 0.0f);
				c.letterSpacing = jText.value("letterSpacing", 1.0f);
				c.lineSpacing = jText.value("lineSpacing", 1.0f);
				c.style = jText.value("style", 0u);
				c.lineAlignment = jText.value("lineAlignment", TextComponent::LineAlignment::Default);
				c.textOrientation = jText.value("textOrientation", TextComponent::TextOrientation::Default);
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

