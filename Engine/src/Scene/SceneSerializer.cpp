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

			if (e.Has<CDamage>())
			{
				jEntity["CDamage"]["damage"] = e.Get<CDamage>().damage;
			}

			if (e.Has<CDraggable>())
			{
				jEntity["CDraggable"]["dragging"] = e.Get<CDraggable>().dragging;
			}

			if (e.Has<CFollowPlayer>())
			{
				const auto& c = e.Get<CFollowPlayer>();
				jEntity["CFollowPlayer"] = { {"home", {c.home.x, c.home.y}}, {"speed", c.speed} };
			}

			if (e.Has<CGravity>())
			{
				jEntity["CGravity"]["gravity"] = e.Get<CGravity>().gravity;
			}

			if (e.Has<CHealth>())
			{
				const auto& c = e.Get<CHealth>();
				jEntity["CHealth"] = { {"max", c.max}, {"current", c.current} };
			}

			if (e.Has<CInput>())
			{
				const auto& c = e.Get<CInput>();
				jEntity["CInput"] = {
					{"up", c.up}, 
					{"down", c.down}, 
					{"left", c.left},
					{"right", c.right}, 
					{"attack", c.attack}, 
					{"canAttack", c.canAttack}
				};
			}

			if (e.Has<CBoundingBox>())
			{
				const auto& c = e.Get<CBoundingBox>();
				jEntity["CBoundingBox"] = {
					{"size", {c.size.x, c.size.y}},
					{"halfSize", {c.halfSize.x, c.halfSize.y}},
					{"center", {c.center.x, c.center.y}},
					{"prevCenter", {c.prevCenter.x, c.prevCenter.y}},
					{"blockMove", c.blockMove},
					{"blockVision", c.blockVision}
				};
			}

			if (e.Has<CAnimation>())
			{
				const auto& c = e.Get<CAnimation>();
				auto anim = std::static_pointer_cast<Graphics::Animation>(Project::GetResourceManager()->GetResource(c.animationHandle));

				jEntity["CAnimation"] = {
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

			if (e.Has<CFont>())
			{
				jEntity["CFont"]["fontHandle"] = static_cast<uint64_t>(e.Get<CFont>().fontHandle);
			}

			if (e.Has<CTexture>())
			{
				jEntity["CTexture"]["textureHandle"] = static_cast<uint64_t>(e.Get<CTexture>().textureHandle);
			}

			if (e.Has<CInvincibility>())
			{
				jEntity["CInvincibility"]["iframes"] = e.Get<CInvincibility>().iframes;
			}

			if (e.Has<CLifespan>())
			{
				const auto& c = e.Get<CLifespan>();
				jEntity["CLifespan"] = { {"lifespan", c.lifespan}, {"frameCreated", c.frameCreated} };
			}

			if (e.Has<CPatrol>())
			{
				const auto& c = e.Get<CPatrol>();
				json path = json::array();
				for (auto& p : c.positions)
					path.push_back({ p.x, p.y });
				jEntity["CPatrol"] = { {"positions", path}, {"currentPosition", c.currentPosition}, {"speed", c.speed} };
			}

			if (e.Has<CState>())
			{
				const auto& c = e.Get<CState>();
				jEntity["CState"] = {
					{"state", c.state},
					{"previousState", c.previousState},
					{"changeAnimation", c.changeAnimation}
				};
			}

			if (e.Has<CTransform>())
			{
				const auto& c = e.Get<CTransform>();
				jEntity["CTransform"] = {
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
			Entity e = entityManager.AddEntity(jEntity["Tag"]);
			e.SetUUID(static_cast<uint64_t>(jEntity["UUID"]));

			if (jEntity.contains("CDamage"))
			{
				e.Add<CDamage>(jEntity["CDamage"]["damage"]);
			}

			if (jEntity.contains("CDraggable"))
			{
				e.Add<CDraggable>(jEntity["CDraggable"]["dragging"].get<bool>());
			}

			if (jEntity.contains("CFollowPlayer"))
			{
				auto p = jEntity["CFollowPlayer"]["home"];
				e.Add<CFollowPlayer>(Math::Vec2(p[0], p[1]), jEntity["CFollowPlayer"]["speed"]);
			}

			if (jEntity.contains("CGravity"))
			{
				e.Add<CGravity>(jEntity["CGravity"]["gravity"]);

			}

			if (jEntity.contains("CHealth"))
			{
				e.Add<CHealth>(jEntity["CHealth"]["max"], jEntity["CHealth"]["current"]);
			}

			if (jEntity.contains("CInput"))
			{
				e.Add<CInput>();
				auto cInput = e.Get<CInput>();
				cInput.up = jEntity["CInput"]["up"];
				cInput.down = jEntity["CInput"]["down"];
				cInput.left = jEntity["CInput"]["left"];
				cInput.right = jEntity["CInput"]["right"];
				cInput.attack = jEntity["CInput"]["attack"];
				cInput.canAttack = jEntity["CInput"]["canAttack"];
			}

			if (jEntity.contains("CBoundingBox"))
			{
				auto size = jEntity["CBoundingBox"]["size"];
				auto center = jEntity["CBoundingBox"]["center"];
				e.Add<CBoundingBox>(
					Math::Vec2(center[0], center[1]),
					Math::Vec2(size[0], size[1]),
					jEntity["CBoundingBox"]["blockMove"],
					jEntity["CBoundingBox"]["blockVision"]
				);
			}

			if (jEntity.contains("CInvincibility"))
			{
				e.Add<CInvincibility>(jEntity["CInvincibility"]["iframes"]);
			}

			if (jEntity.contains("CLifespan"))
			{
				e.Add<CLifespan>(jEntity["CLifespan"]["lifespan"], jEntity["CLifespan"]["frameCreated"]);
			}

			if (jEntity.contains("CPatrol"))
			{
				std::vector<Math::Vec2> points;
				for (const auto& p : jEntity["CPatrol"]["positions"])
					points.emplace_back(p[0], p[1]);

				auto& c = e.Add<CPatrol>(points, jEntity["CPatrol"]["speed"]);
				c.currentPosition = jEntity["CPatrol"]["currentPosition"];
			}

			if (jEntity.contains("CState"))
			{
				e.Add<CState>(
					jEntity["CState"]["state"]
				);
				auto cState = e.Get<CState>();
				cState.previousState = jEntity["CState"]["previousState"],
				cState.changeAnimation = jEntity["CState"]["changeAnimation"];
			}

			if (jEntity.contains("CTransform"))
			{
				auto pos = jEntity["CTransform"]["pos"];
				auto prevPos = jEntity["CTransform"]["prevPos"];
				auto velocity = jEntity["CTransform"]["velocity"];
				auto scale = jEntity["CTransform"]["scale"];
				auto facing = jEntity["CTransform"]["facing"];
				e.Add<CTransform>(
					Math::Vec2(pos[0], pos[1]),
					Math::Vec2(velocity[0], velocity[1]),
					Math::Vec2(scale[0], scale[1]),
					jEntity["CTransform"]["angle"]
				);
				auto& c = e.Get<CTransform>();
				c.prevPos = { prevPos[0], prevPos[1] };
				c.facing = { facing[0], facing[1] };
			}

			if (jEntity.contains("CAnimation"))
			{
				const auto& jAnim = jEntity["CAnimation"];

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

				auto& c = e.Add<CAnimation>(animationHandle, speed, currentFrame, repeat);
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

