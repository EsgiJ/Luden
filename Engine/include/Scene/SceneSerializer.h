#pragma once

#include "EngineAPI.h"
#include "IO/FileStream.h"
#include "Scene/Scene.h"
#include "Resource/ResourceSerializer.h"

#include <filesystem>

#include <nlohmann/json.hpp>

namespace Luden
{
	class ENGINE_API SceneSerializer
	{
	public:
		SceneSerializer(std::shared_ptr<Scene> scene);

		bool Serialize(const std::filesystem::path& path);
		bool Deserialize(const std::filesystem::path& path);
		bool SerializeToJSON(nlohmann::json& outJson);
		bool DeserializeFromJSON(const nlohmann::json& inJson);

		bool SerializeToResourcePack(FileStreamWriter& stream, ResourceSerializationInfo& outInfo);
		bool DeserializeFromResourcePack(FileStreamReader& stream, const ResourcePackFile::SceneInfo& sceneInfo);

	public:
		inline static std::string_view FileFilter = "Luden Scene (*.lscene)\0*.lscene\0";
		inline static std::string_view DefaultExtension = ".lscene";
	private:
		const std::shared_ptr<Scene> m_Scene;
	};
}