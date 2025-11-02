#pragma once

#include <filesystem>
#include <map>
#include <unordered_set>

#include "Core/UUID.h"
#include "Resource/Resource.h"
#include "Serialization/ResourcePackFile.h"
#include "Serialization/ResourcePackSerializer.h"

namespace Luden 
{
	class Scene;
	class ResourcePack
	{
	public:
		ResourcePack() = default;
		ResourcePack(const std::filesystem::path& path);

		void AddResource(std::shared_ptr<Resource> resource);

		void Serialize();

		std::shared_ptr<Scene> LoadScene(ResourceHandle sceneHandle);
		std::shared_ptr<Resource> LoadResource(ResourceHandle sceneHandle, ResourceHandle resourceHandle);

		bool IsResourceHandleValid(ResourceHandle resourceHandle) const;
		bool IsResourceHandleValid(ResourceHandle sceneHandle, ResourceHandle resourceHandle) const;

		Buffer ReadAppBinary();
		uint64_t GetBuildVersion();

		ResourceType GetResourceType(ResourceHandle sceneHandle, ResourceHandle resourceHandle) const;

		// This will create a complete resource pack from ALL referenced resources
		// in currently active project. This should change in the future to
		// take in a std::shared_ptr<Project> or something when the ResourceManager becomes
		// non-static, but since it is static at the moment this is what we get
		static std::shared_ptr<ResourcePack> CreateFromActiveProject(std::atomic<float>& progress);
		static std::shared_ptr<ResourcePack> Load(const std::filesystem::path& path);
		static std::shared_ptr<ResourcePack> LoadActiveProject();
	private:
		std::filesystem::path m_Path;
		ResourcePackFile m_File;

		ResourcePackSerializer m_Serializer;

		std::unordered_set<ResourceHandle> m_ResourceHandleIndex;
	};
}
