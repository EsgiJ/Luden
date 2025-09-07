#include "Resource/ResourceImporter.h"
#include "Resource/ResourceManager.h"

namespace Luden
{
	void ResourceImporter::Init()
	{
		s_Serializers.clear();
		s_Serializers[ResourceType::Texture] = std::make_unique<TextureSerializer>();
		s_Serializers[ResourceType::Audio] = std::make_unique<AudioFileSourceSerializer>();
		s_Serializers[ResourceType::Scene] = std::make_unique<SceneResourceSerializer>();
		s_Serializers[ResourceType::Font] = std::make_unique<FontSerializer>();
		s_Serializers[ResourceType::Animation] = std::make_unique<AnimationResourceSerializer>();
	}
	void ResourceImporter::Serialize(const ResourceMetadata metadata, const std::shared_ptr<Resource> resource)
	{
		if (s_Serializers.find(metadata.Type) == s_Serializers.end())
			return;

		s_Serializers[resource->GetResourceType()]->Serialize(metadata, resource);
	}
	void ResourceImporter::Serialize(const std::shared_ptr<Resource> resource)
	{
		const ResourceMetadata& metadata = Project::GetEditorResourceManager()->GetMetadata(resource->Handle);
		Serialize(metadata, resource);
	}
	bool ResourceImporter::TryLoadData(const ResourceMetadata metadata, const std::shared_ptr<Resource> resource)
	{
		if (s_Serializers.find(metadata.Type) == s_Serializers.end())
			return false;
		return s_Serializers[metadata.Type]->TryLoadData(metadata, resource);
	}
	bool ResourceImporter::SerializeToResourcePack(ResourceHandle resourceHandle, FileStreamWriter& stream, ResourceSerializationInfo outInfo)
	{
		outInfo.Size = 0;

		if (!ResourceManager::IsResourceHandleValid(resourceHandle))
			return false;

		ResourceType type = ResourceManager::GetResourceType(resourceHandle);
		if (s_Serializers.find(type) == s_Serializers.end())
		{
			const auto& metadata = Project::GetEditorResourceManager()->GetMetadata(resourceHandle);
			return false;
		}

		return s_Serializers[type]->SerializeToResourcePack(resourceHandle, stream, outInfo);
	}
	std::shared_ptr<Resource> ResourceImporter::DeserializeFromResourcePack(FileStreamReader& stream, const ResourcePackFile::ResourceInfo& resourceInfo)
	{
		ResourceType resourceType = (ResourceType)resourceInfo.Type;
		if (s_Serializers.find(resourceType) == s_Serializers.end())
			return nullptr;

		return s_Serializers[resourceType]->DeserializeFromResourcePack(stream, resourceInfo);
	}
	std::shared_ptr<Scene> ResourceImporter::DeserializeSceneFromResourcePack(FileStreamReader& stream, const ResourcePackFile::SceneInfo& sceneInfo)
	{
		ResourceType resourceType = ResourceType::Scene;
		if (s_Serializers.find(resourceType) == s_Serializers.end())
			return nullptr;

		SceneResourceSerializer* sceneResourceSerializer = (SceneResourceSerializer*)s_Serializers[resourceType].get();
		return sceneResourceSerializer->DeserializeSceneFromResourcePack(stream, sceneInfo);
	}

	std::unordered_map<ResourceType, std::unique_ptr<ResourceSerializer>> ResourceImporter::s_Serializers;
}