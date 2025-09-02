#include "Serialization/ResourcePackSerializer.h"
#include "Resource/ResourceImporter.h"
#include "IO/FileStream.h"

#include <filesystem>
#include <fstream>
#include <unordered_set>

namespace Luden {

	static void CreateDirectoriesIfNeeded(const std::filesystem::path& path)
	{
		std::filesystem::path directory = path.parent_path();
		if (!std::filesystem::exists(directory))
			std::filesystem::create_directories(directory);
	}

	void ResourcePackSerializer::Serialize(const std::filesystem::path& path, ResourcePackFile& file, Buffer appBinary, std::atomic<float>& progress)
	{
		// Print Info
		//TODO: LOG "Serializing ResourcePack to {}", path.string());
		//TODO: LOG("  {} scenes", file.Index.Scenes.size());
		uint32_t resourceCount = 0;
		for (const auto& [sceneHandle, sceneInfo] : file.Index.Scenes)
			resourceCount += uint32_t(sceneInfo.Resources.size());
		//TODO: LOG("  {} resources (including duplicates)", resourceCount);

		FileStreamWriter serializer(path);

		serializer.WriteRaw<ResourcePackFile::FileHeader>(file.Header);

		// ===============
		// Write index
		// ===============
		// Write dummy data for index (come back later to fill in)
		uint64_t indexPos = serializer.GetStreamPosition();
		uint64_t indexTableSize = CalculateIndexTableSize(file);
		serializer.WriteZero(indexTableSize);

		std::unordered_map<ResourceHandle, ResourceSerializationInfo> serializedResources;

		float progressIncrement = 0.4f / (float)file.Index.Scenes.size();

		// Write app binary data
		file.Index.PackedAppBinaryOffset = serializer.GetStreamPosition();
		serializer.WriteBuffer(appBinary);
		file.Index.PackedAppBinarySize = serializer.GetStreamPosition() - file.Index.PackedAppBinaryOffset;
		appBinary.Release();

		// Write resource data + fill in offset + size
		for (auto& [sceneHandle, sceneInfo] : file.Index.Scenes)
		{
			// Serialize Scene
			ResourceSerializationInfo serializationInfo;
			ResourceImporter::SerializeToResourcePack(sceneHandle, serializer, serializationInfo);
			file.Index.Scenes[sceneHandle].PackedOffset = serializationInfo.Offset;
			file.Index.Scenes[sceneHandle].PackedSize = serializationInfo.Size;

			// Serialize Resources
			for (auto& [resourceHandle, resourceInfo] : sceneInfo.Resources)
			{
				if (serializedResources.find(resourceHandle) != serializedResources.end())
				{
					// Has already been serialized
					serializationInfo = serializedResources.at(resourceHandle);
					file.Index.Scenes[sceneHandle].Resources[resourceHandle].PackedOffset = serializationInfo.Offset;
					file.Index.Scenes[sceneHandle].Resources[resourceHandle].PackedSize = serializationInfo.Size;
				}
				else
				{
					// Serialize resource
					if (ResourceImporter::SerializeToResourcePack(resourceHandle, serializer, serializationInfo))
					{
						file.Index.Scenes[sceneHandle].Resources[resourceHandle].PackedOffset = serializationInfo.Offset;
						file.Index.Scenes[sceneHandle].Resources[resourceHandle].PackedSize = serializationInfo.Size;
						serializedResources[resourceHandle] = serializationInfo;
					}
					else
					{
						//TODO: LOG ERROR("Failed to serialize resource with handle {}", resourceHandle);
					}
				}
			}

			progress = progress + progressIncrement;
		}

		//TODO: LOG("Serialized {} resources into ResourcePack", serializedResources.size());

		serializer.SetStreamPosition(indexPos);
		serializer.WriteRaw<uint64_t>(file.Index.PackedAppBinaryOffset);
		serializer.WriteRaw<uint64_t>(file.Index.PackedAppBinarySize);

		uint64_t begin = indexPos;
		serializer.WriteRaw<uint32_t>((uint32_t)file.Index.Scenes.size()); // Write scene map size
		for (auto& [sceneHandle, sceneInfo] : file.Index.Scenes)
		{
			serializer.WriteRaw<uint64_t>(sceneHandle);
			serializer.WriteRaw<uint64_t>(sceneInfo.PackedOffset);
			serializer.WriteRaw<uint64_t>(sceneInfo.PackedSize);
			serializer.WriteRaw<uint16_t>(sceneInfo.Flags);

			serializer.WriteMap(file.Index.Scenes[sceneHandle].Resources);
		}

		progress = progress + 0.1f;
	}

	bool ResourcePackSerializer::DeserializeIndex(const std::filesystem::path& path, ResourcePackFile& file)
	{
		// Print Info
		//TODO: LOG("Resource Pack", "Deserializing Resource Pack from {}", path.string());

		FileStreamReader stream(path);
		if (!stream.IsStreamGood())
			return false;

		stream.ReadRaw<ResourcePackFile::FileHeader>(file.Header);
		bool validHeader = memcmp(file.Header.HEADER, "HZAP", 4) == 0;

		if (!validHeader)
			return false;

		ResourcePackFile current;
		if (file.Header.Version != current.Header.Version)
		{
			//TODO: LOG("ResourcePack version {} is not compatible with current version {}", file.Header.Version, current.Header.Version);
			return false;
		}

		// Read app binary info
		stream.ReadRaw<uint64_t>(file.Index.PackedAppBinaryOffset);
		stream.ReadRaw<uint64_t>(file.Index.PackedAppBinarySize);

		uint32_t sceneCount = 0;
		stream.ReadRaw<uint32_t>(sceneCount); // Read scene map size
		for (uint32_t i = 0; i < sceneCount; i++)
		{
			uint64_t sceneHandle = 0;
			stream.ReadRaw<uint64_t>(sceneHandle);

			ResourcePackFile::SceneInfo& sceneInfo = file.Index.Scenes[sceneHandle];
			stream.ReadRaw<uint64_t>(sceneInfo.PackedOffset);
			stream.ReadRaw<uint64_t>(sceneInfo.PackedSize);
			stream.ReadRaw<uint16_t>(sceneInfo.Flags);

			stream.ReadMap(sceneInfo.Resources);
		}

		//TODO: LOG("Resource Pack", "Deserialized index with {} scenes from ResourcePack", sceneCount);
		return true;
	}

	uint64_t ResourcePackSerializer::CalculateIndexTableSize(const ResourcePackFile& file)
	{
		uint64_t appInfoSize = sizeof(uint64_t) * 2;
		uint64_t sceneMapSize = sizeof(uint32_t) + (sizeof(ResourceHandle) + sizeof(uint64_t) * 2 + sizeof(uint16_t)) * file.Index.Scenes.size();
		uint64_t resourceMapSize = 0;
		for (const auto& [sceneHandle, sceneInfo] : file.Index.Scenes)
			resourceMapSize += sizeof(uint32_t) + (sizeof(ResourceHandle) + sizeof(ResourcePackFile::ResourceInfo)) * sceneInfo.Resources.size();

		return appInfoSize + sceneMapSize + resourceMapSize;
	}

}
