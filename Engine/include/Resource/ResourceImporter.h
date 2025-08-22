#pragma once

#include "EngineAPI.h"
#include "IO/FileSystem.h"
#include "IO/FileStream.h"
#include "Resource/ResourceMetadata.h"
#include "Resource/ResourceSerializer.h"
#include "Serialization/ResourcePackFile.h"
#include "Scene/Scene.h"

namespace Luden
{
	class ENGINE_API ResourceImporter
	{
	public:
		static void Init();
		static void Serialize(const ResourceMetadata metadata, const std::shared_ptr<Resource> resource);
		static void Serialize(const std::shared_ptr<Resource> resource);
		static void TryLoadData(const ResourceMetadata metadata, const std::shared_ptr<Resource> resource);

		static bool SerializeToResourcePack(ResourceHandle resourceHandle, FileStreamWriter& stream, ResourceSerializationInfo outInfo);
		static std::shared_ptr<Resource> DeserializeFromResourcePack(FileStreamReader& stream, const ResourcePackFile::ResourceInfo& resourceInfo);
		static std::shared_ptr<Scene> DeserializeSceneFromResourcePack(FileStreamReader& stream, const ResourcePackFile::SceneInfo& sceneInfo);
	private:
		static std::unordered_map<ResourceType, std::unique_ptr<ResourceSerializer>> s_Serializers;
	};
}