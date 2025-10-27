#include "Scene/SceneSerializer.h"
#include "ECS/Components/Components.h"
#include "Resource/EditorResourceManager.h"
#include "Project/Project.h"

#include <fstream>

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
				jEntity["HealthComponent"] = { {"max", c.max}, {"current", c.current} };
			}

			if (e.Has<InputComponent>())
			{
				const auto& c = e.Get<InputComponent>();
				jEntity["InputComponent"] = {
					{"up", c.up}, 
					{"down", c.down}, 
					{"left", c.left},
					{"right", c.right}, 
					{"attack", c.attack}, 
					{"canAttack", c.canAttack}
				};
			}

			if (e.Has<BoxCollider2DComponent>())
			{
				const auto& c = e.Get<BoxCollider2DComponent>();
				jEntity["BoxCollider2DComponent"] = {
					{"size", {c.size.x, c.size.y}},
					{"halfSize", {c.halfSize.x, c.halfSize.y}},
					{"center", {c.center.x, c.center.y}},
					{"prevCenter", {c.prevCenter.x, c.prevCenter.y}},
					{"blockMove", c.blockMove},
					{"blockVision", c.blockVision}
				};
			}

			if (e.Has<Animation2DComponent>())
			{
				const auto& c = e.Get<Animation2DComponent>();
				auto anim = std::static_pointer_cast<Graphics::Animation>(Project::GetResourceManager()->GetResource(c.animationHandle));

				jEntity["Animation2DComponent"] = {
					{ "animationHandle", static_cast<uint64_t>(c.animationHandle) },
					{ "repeat", c.repeat },
					{ "name", anim->GetName() },
					{ "frameCount", anim->GetFrameCount() },
					{ "currentFrame", c.currentFrame },
					{ "speed", c.speed },
					{ "size", { anim->GetSize().x, anim->GetSize().y } },
					{ "textureHandle", static_cast<uint64_t>(anim->GetTextureHandle()) }
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
					{"pos", {c.pos.x, c.pos.y}},
					{"prevPos", {c.prevPos.x, c.prevPos.y}},
					{"velocity", {c.velocity.x, c.velocity.y}},
					{"scale", {c.scale.x, c.scale.y}},
					{"facing", {c.facing.x, c.facing.y}},
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
			Entity e = entityManager.AddEntity(jEntity["Tag"].get<std::string>(), jEntity["UUID"].get<uint64_t>());

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
				e.Add<FollowPLayerComponent>(Math::Vec2(p[0], p[1]), jEntity["FollowPLayerComponent"]["speed"]);
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
				inputComponent.up = jEntity["InputComponent"]["up"];
				inputComponent.down = jEntity["InputComponent"]["down"];
				inputComponent.left = jEntity["InputComponent"]["left"];
				inputComponent.right = jEntity["InputComponent"]["right"];
				inputComponent.attack = jEntity["InputComponent"]["attack"];
				inputComponent.canAttack = jEntity["InputComponent"]["canAttack"];
			}

			if (jEntity.contains("BoxCollider2DComponent"))
			{
				auto size = jEntity["BoxCollider2DComponent"]["size"];
				auto center = jEntity["BoxCollider2DComponent"]["center"];
				e.Add<BoxCollider2DComponent>(
					Math::Vec2(center[0], center[1]),
					Math::Vec2(size[0], size[1]),
					jEntity["BoxCollider2DComponent"]["blockMove"],
					jEntity["BoxCollider2DComponent"]["blockVision"]
				);
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
				std::vector<Math::Vec2> points;
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
				auto pos = jEntity["TransformComponent"]["pos"];
				auto prevPos = jEntity["TransformComponent"]["prevPos"];
				auto velocity = jEntity["TransformComponent"]["velocity"];
				auto scale = jEntity["TransformComponent"]["scale"];
				auto facing = jEntity["TransformComponent"]["facing"];
				e.Add<TransformComponent>(
					Math::Vec2(pos[0], pos[1]),
					Math::Vec2(velocity[0], velocity[1]),
					Math::Vec2(scale[0], scale[1]),
					jEntity["TransformComponent"]["angle"]
				);
				auto& c = e.Get<TransformComponent>();
				c.prevPos = { prevPos[0], prevPos[1] };
				c.facing = { facing[0], facing[1] };
			}

			if (jEntity.contains("Animation2DComponent"))
			{
				const auto& jAnim = jEntity["Animation2DComponent"];

				ResourceHandle texHandle(jAnim["textureHandle"].get<uint64_t>());

				Graphics::Animation anim(
					jAnim["name"].get<std::string>(),
					texHandle,
					jAnim["frameCount"].get<size_t>()
				);

				anim.SetSize({
					jAnim["size"][0].get<float>(),
					jAnim["size"][1].get<float>()
					});

				ResourceHandle animationHandle(jAnim["animationHandle"].get<uint64_t>());
				bool repeat = jAnim["repeat"].get<bool>();
				size_t speed = jAnim["speed"].get<size_t>();
				size_t currentFrame = jAnim["currentFrame"].get<size_t>();

				auto& c = e.Add<Animation2DComponent>(animationHandle, speed, currentFrame, repeat);
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

