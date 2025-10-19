#pragma once

#include "Resource/ResourceManagerBase.h"
#include "Resource/ResourceMetadata.h"
#include "Resource/ResourceRegistry.h"
#include "EngineAPI.h"


namespace Luden
{
	class ENGINE_API EditorResourceManager : public ResourceManagerBase
	{
	public:
		EditorResourceManager();
		virtual ~EditorResourceManager();

		virtual void Shutdown() override;

		virtual ResourceType GetResourceType(ResourceHandle resourceHandle) override;
		virtual std::shared_ptr<Resource> GetResource(ResourceHandle resourceHandle) override;

		virtual bool ReloadData(ResourceHandle resourceHandle) override;
		virtual bool EnsureCurrent(ResourceHandle resourceHandle) override;
		virtual bool EnsureAllLoadedCurrent() override;
		virtual bool IsResourceHandleValid(ResourceHandle resourceHandle) override { return GetMetadata(resourceHandle).IsValid(); };
		virtual bool IsResourceLoaded(ResourceHandle resourceHandle) override;
		virtual bool IsResourceValid(ResourceHandle resourceHandle) override;
		virtual bool IsResourceMissing(ResourceHandle resourceHandle) override;
		virtual void RemoveResource(ResourceHandle resourceHandle) override;

		virtual void RegisterDependency(ResourceHandle resourceHandle, ResourceHandle dependency) override;

		virtual std::unordered_set<ResourceHandle> GetAllResourcesWithType(ResourceType type) override;
		virtual const std::unordered_map<ResourceHandle, std::shared_ptr<Resource>>& GetLoadedResources() override { return m_LoadedResources; }

		// ------------- Editor-only ----------------
		ResourceMetadata GetMetadata(ResourceHandle resourceHandle);
		void SetMetadata(ResourceHandle resourceHandle, const ResourceMetadata& metadata);
		const ResourceMetadata& GetMetadata(const std::filesystem::path& filepath);

		ResourceHandle ImportResource(const std::filesystem::path& filepath);

		ResourceHandle GetResourceHandleFromFilePath(const std::filesystem::path& filepath);

		ResourceRegistry& GetResourceRegistry() { return m_ResourceRegistry; }

		ResourceType GetResourceTypeFromExtension(const std::string& extension);
		std::string GetDefaultExtensionForResourceType(ResourceType type);
		ResourceType GetResourceTypeFromPath(const std::filesystem::path& path);

		std::filesystem::path GetFileSystemPath(const ResourceMetadata& metadata);
		std::filesystem::path GetFileSystemPath(ResourceHandle resourceHandle);
		std::string GetFileSystemPathString(const ResourceMetadata& metadata);
		std::filesystem::path GetRelativePath(const std::filesystem::path& filepath);

		bool FileExists(ResourceMetadata& metadata) const;
	private:
		void LoadResourceRegistry();
		void ProcessDirectory(const std::filesystem::path& directoryPath);
		void ReloadResources();
		void WriteRegistryToFile();

		void OnResourceRenamed(ResourceHandle resourceHandle, const std::filesystem::path& newFilePath);
		void OnResourceDeleted(ResourceHandle resourceHandle);
	private:
		std::unordered_map<ResourceHandle, std::unordered_set<ResourceHandle>> m_ResourceDependencies;

		ResourceRegistry m_ResourceRegistry;
		ResourceMap m_LoadedResources;
	};
}
