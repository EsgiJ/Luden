#pragma once

#include "EngineAPI.h"
#include "IO/FileStream.h"
#include "Resource/ResourceMetadata.h"
#include "Serialization/ResourcePackFile.h"

namespace Luden
{

	struct ENGINE_API ResourceSerializationInfo
	{
		uint64_t Offset = 0;
		uint64_t Size = 0;
	};

	class ENGINE_API ResourceSerializer
	{
	public:
		virtual void Serialize(const ResourceMetadata& metadata, const std::shared_ptr<Resource> resource) const = 0;
		virtual bool TryLoadData(const ResourceMetadata& metadata, std::shared_ptr<Resource> resource) const = 0;

		virtual bool SerializeToResourcePack(ResourceHandle handle, FileStreamWriter& stream, ResourceSerializationInfo& outInfo) const = 0;
		virtual std::shared_ptr<Resource> DeserializeFromResourcePack(FileStreamReader& stream, const ResourcePackFile::ResourceInfo& resourceInfo) const = 0;
	};

	class TextureSerializer : public ResourceSerializer
	{
	public:
		virtual void Serialize(const ResourceMetadata& metadata, const std::shared_ptr<Resource> resource) const override {}
		virtual bool TryLoadData(const ResourceMetadata& metadata, std::shared_ptr<Resource> resource) const override;

		virtual bool SerializeToResourcePack(ResourceHandle handle, FileStreamWriter& stream, ResourceSerializationInfo& outInfo) const;
		virtual std::shared_ptr<Resource> DeserializeFromResourcePack(FileStreamReader& stream, const ResourcePackFile::ResourceInfo& resourceInfo) const;
	};

	class FontSerializer : public ResourceSerializer
	{
	public:
		virtual void Serialize(const ResourceMetadata& metadata, const std::shared_ptr<Resource> resource) const override {}
		virtual bool TryLoadData(const ResourceMetadata& metadata, std::shared_ptr<Resource> resource) const override;

		virtual bool SerializeToResourcePack(ResourceHandle handle, FileStreamWriter& stream, ResourceSerializationInfo& outInfo) const;
		virtual std::shared_ptr<Resource> DeserializeFromResourcePack(FileStreamReader& stream, const ResourcePackFile::ResourceInfo& resourceInfo) const = 0;
	};

	class AudioFileSourceSerializer : public ResourceSerializer
	{
	public:
		virtual void Serialize(const ResourceMetadata& metadata, const std::shared_ptr<Resource> resource) const override;
		virtual bool TryLoadData(const ResourceMetadata& metadata, std::shared_ptr<Resource> resource) const override;

		virtual bool SerializeToResourcePack(ResourceHandle handle, FileStreamWriter& stream, ResourceSerializationInfo& outInfo) const;
		virtual std::shared_ptr<Resource> DeserializeFromResourcePack(FileStreamReader& stream, const ResourcePackFile::ResourceInfo& resourceInfo) const;
	};
/*
	class PrefabSerializer : public ResourceSerializer
	{
	public:
		virtual void Serialize(const ResourceMetadata& metadata, const std::shared_ptr<Resource> resource) const override;
		virtual bool TryLoadData(const ResourceMetadata& metadata, std::shared_ptr<Resource> resource) const override;

		virtual bool SerializeToResourcePack(ResourceHandle handle, FileStreamWriter& stream, ResourceSerializationInfo& outInfo) const;
		virtual std::shared_ptr<Resource> DeserializeFromResourcePack(FileStreamReader& stream, const ResourcePackFile::ResourceInfo& resourceInfo) const;
	private:
		std::string SerializeToJSON(std::shared_ptr<Prefab> prefab) const;
		bool DeserializeFromJSON(const std::string& jsonString, std::shared_ptr<Prefab> prefab) const;
	};
*/
	class SceneResourceSerializer : public ResourceSerializer
	{
	public:
		virtual void Serialize(const ResourceMetadata& metadata, const std::shared_ptr<Resource> resource) const override;
		virtual bool TryLoadData(const ResourceMetadata& metadata, std::shared_ptr<Resource> resource) const override;

		virtual bool SerializeToResourcePack(ResourceHandle handle, FileStreamWriter& stream, ResourceSerializationInfo& outInfo) const;
		virtual std::shared_ptr<Resource> DeserializeFromResourcePack(FileStreamReader& stream, const ResourcePackFile::ResourceInfo& resourceInfo) const;
		std::shared_ptr<Scene> DeserializeSceneFromResourcePack(FileStreamReader& stream, const ResourcePackFile::SceneInfo& sceneInfo) const;
	};

	class AnimationResourceSerializer : public ResourceSerializer
	{
	public:
		virtual void Serialize(const ResourceMetadata& metadata, const std::shared_ptr<Resource> resource) const override {}
		virtual bool TryLoadData(const ResourceMetadata& metadata, std::shared_ptr<Resource> resource) const override;

		virtual bool SerializeToResourcePack(ResourceHandle handle, FileStreamWriter& stream, ResourceSerializationInfo& outInfo) const;
		virtual std::shared_ptr<Resource> DeserializeFromResourcePack(FileStreamReader& stream, const ResourcePackFile::ResourceInfo& resourceInfo) const = 0;
	};
}