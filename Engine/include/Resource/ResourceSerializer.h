#pragma once

#include "EngineAPI.h"
#include "IO/FileStream.h"
#include "Resource/ResourceMetadata.h"
#include "Serialization/ResourcePackFile.h"

#include <memory>

namespace Luden
{
	class Scene;

	struct ENGINE_API ResourceSerializationInfo
	{
		uint64_t Offset = 0;
		uint64_t Size = 0;
	};

	class ENGINE_API ResourceSerializer
	{
	public:
		virtual void Serialize(const ResourceMetadata& metadata, const std::shared_ptr<Resource>& resource) const = 0;
		virtual bool TryLoadData(const ResourceMetadata& metadata, std::shared_ptr<Resource>& resource) const = 0;

		virtual bool SerializeToResourcePack(ResourceHandle handle, FileStreamWriter& stream, ResourceSerializationInfo& outInfo) const = 0;
		virtual std::shared_ptr<Resource> DeserializeFromResourcePack(FileStreamReader& stream, const ResourcePackFile::ResourceInfo& resourceInfo) const = 0;
	};

	class ENGINE_API TextureSerializer : public ResourceSerializer
	{
	public:
		virtual void Serialize(const ResourceMetadata& metadata, const std::shared_ptr<Resource>& resource) const override {}
		virtual bool TryLoadData(const ResourceMetadata& metadata, std::shared_ptr<Resource>& resource) const override;

		virtual bool SerializeToResourcePack(ResourceHandle handle, FileStreamWriter& stream, ResourceSerializationInfo& outInfo) const;
		virtual std::shared_ptr<Resource> DeserializeFromResourcePack(FileStreamReader& stream, const ResourcePackFile::ResourceInfo& resourceInfo) const;
	};

	class ENGINE_API SpriteSerializer : public ResourceSerializer
	{
	public:
		virtual void Serialize(const ResourceMetadata& metadata, const std::shared_ptr<Resource>& resource) const override;
		virtual bool TryLoadData(const ResourceMetadata& metadata, std::shared_ptr<Resource>& resource) const override;
		virtual bool SerializeToResourcePack(ResourceHandle handle, FileStreamWriter& stream, ResourceSerializationInfo& outInfo) const override;
		virtual std::shared_ptr<Resource> DeserializeFromResourcePack(FileStreamReader& stream, const ResourcePackFile::ResourceInfo& resourceInfo) const override;
	};

	class ENGINE_API NativeScriptResourceSerializer : public ResourceSerializer
	{
	public:
		virtual void Serialize(const ResourceMetadata& metadata, const std::shared_ptr<Resource>& resource) const override;
		virtual bool TryLoadData(const ResourceMetadata& metadata, std::shared_ptr<Resource>& resource) const override;

		virtual bool SerializeToResourcePack(ResourceHandle handle, FileStreamWriter& stream, ResourceSerializationInfo& outInfo) const override;
		virtual std::shared_ptr<Resource> DeserializeFromResourcePack(FileStreamReader& stream, const ResourcePackFile::ResourceInfo& resourceInfo) const override;
	};

	class ENGINE_API FontSerializer : public ResourceSerializer
	{
	public:
		virtual void Serialize(const ResourceMetadata& metadata, const std::shared_ptr<Resource>& resource) const override {}
		virtual bool TryLoadData(const ResourceMetadata& metadata, std::shared_ptr<Resource>& resource) const override;

		virtual bool SerializeToResourcePack(ResourceHandle handle, FileStreamWriter& stream, ResourceSerializationInfo& outInfo) const;
		virtual std::shared_ptr<Resource> DeserializeFromResourcePack(FileStreamReader& stream, const ResourcePackFile::ResourceInfo& resourceInfo) const;
	};

	class ENGINE_API SoundResourceSerializer : public ResourceSerializer
	{
	public:
		virtual void Serialize(const ResourceMetadata& metadata, const std::shared_ptr<Resource>& resource) const override;
		virtual bool TryLoadData(const ResourceMetadata& metadata, std::shared_ptr<Resource>& resource) const override;

		virtual bool SerializeToResourcePack(ResourceHandle handle, FileStreamWriter& stream, ResourceSerializationInfo& outInfo) const;
		virtual std::shared_ptr<Resource> DeserializeFromResourcePack(FileStreamReader& stream, const ResourcePackFile::ResourceInfo& resourceInfo) const;
	};

	class ENGINE_API MusicResourceSerializer : public ResourceSerializer
	{
	public:
		virtual void Serialize(const ResourceMetadata& metadata, const std::shared_ptr<Resource>& resource) const override;
		virtual bool TryLoadData(const ResourceMetadata& metadata, std::shared_ptr<Resource>& resource) const override;

		virtual bool SerializeToResourcePack(ResourceHandle handle, FileStreamWriter& stream, ResourceSerializationInfo& outInfo) const;
		virtual std::shared_ptr<Resource> DeserializeFromResourcePack(FileStreamReader& stream, const ResourcePackFile::ResourceInfo& resourceInfo) const;
	};

	class ENGINE_API PrefabSerializer : public ResourceSerializer
	{
	public:
		virtual void Serialize(const ResourceMetadata& metadata, const std::shared_ptr<Resource>& resource) const override;
		virtual bool TryLoadData(const ResourceMetadata& metadata, std::shared_ptr<Resource>& resource) const override;

		virtual bool SerializeToResourcePack(ResourceHandle handle, FileStreamWriter& stream, ResourceSerializationInfo& outInfo) const;
		virtual std::shared_ptr<Resource> DeserializeFromResourcePack(FileStreamReader& stream, const ResourcePackFile::ResourceInfo& resourceInfo) const;
	};

	class ENGINE_API SceneResourceSerializer : public ResourceSerializer
	{
	public:
		virtual void Serialize(const ResourceMetadata& metadata, const std::shared_ptr<Resource>& resource) const override;
		virtual bool TryLoadData(const ResourceMetadata& metadata, std::shared_ptr<Resource>& resource) const override;

		virtual bool SerializeToResourcePack(ResourceHandle handle, FileStreamWriter& stream, ResourceSerializationInfo& outInfo) const;
		virtual std::shared_ptr<Resource> DeserializeFromResourcePack(FileStreamReader& stream, const ResourcePackFile::ResourceInfo& resourceInfo) const;
		std::shared_ptr<Scene> DeserializeSceneFromResourcePack(FileStreamReader& stream, const ResourcePackFile::SceneInfo& sceneInfo) const;
	};

	class ENGINE_API AnimationResourceSerializer : public ResourceSerializer
	{
	public:
		virtual void Serialize(const ResourceMetadata& metadata, const std::shared_ptr<Resource>& resource) const override;
		virtual bool TryLoadData(const ResourceMetadata& metadata, std::shared_ptr<Resource>& resource) const override;

		virtual bool SerializeToResourcePack(ResourceHandle handle, FileStreamWriter& stream, ResourceSerializationInfo& outInfo) const;
		virtual std::shared_ptr<Resource> DeserializeFromResourcePack(FileStreamReader& stream, const ResourcePackFile::ResourceInfo& resourceInfo) const;
	};
}