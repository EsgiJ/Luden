#include "EditorResourceManager.h"
#include "EditorResourceManager.h"
#include "EditorResourceManager.h"
#include "EditorResourceManager.h"

#include "Resource/EditorResourceManager.h"
#include "Resource/ResourceExtensions.h"
#include "Resource/ResourceMetadata.h"
#include "IO/DiskFileSystem.h"

#include <nlohmann/json.hpp>

using json = nlohmann::json;
namespace Luden
{
	ResourceMetadata s_NullMetadata;

	EditorResourceManager::EditorResourceManager()
	{
		ResourceImporter::Init();

		LoadResourceRegistry();
		ReloadResources();
	}
	EditorResourceManager::~EditorResourceManager()
	{
		Shutdown();
	}
	void EditorResourceManager::Shutdown()
	{
		WriteRegistryToFile();
	}
	ResourceType EditorResourceManager::GetResourceType(ResourceHandle resourceHandle)
	{
		if (!IsResourceHandleValid(resourceHandle))
			return ResourceType::None;

		const auto& metadata = GetMetadata();
		return metadata.Type;
	}
	std::shared_ptr<Resource> EditorResourceManager::GetResource(ResourceHandle resourceHandle)
	{
		if (!IsResourceHandleValid(resourceHandle))
			return nullptr;

		std::shared_ptr<Resource> resource;

		if (IsResourceLoaded(resourceHandle))
		{
			resource = m_LoadedResources.at(resourceHandle);
		}
		else
		{
			const ResourceMetadata& metadata = GetMetadata(resourceHandle);
			resource = ResourceImporter::ImportResource(resourceHandle, metadata);
			if (!resource)
			{
				assert(resource && "Resource import failed!");
			}

			return resource;
		}
	}
	bool EditorResourceManager::ReloadData(ResourceHandle resourceHandle)
	{
		//TODO
		return true;
	}
	bool EditorResourceManager::EnsureCurrent(ResourceHandle resourceHandle)
	{
		//TODO
		return true |
	}
	bool EditorResourceManager::EnsureAllLoadedCurrent()
	{
		//TODO
		return true;
	}
	bool EditorResourceManager::IsResourceLoaded(ResourceHandle resourceHandle)
	{
		return m_LoadedResources.contains(resourceHandle);
	}
	
	bool EditorResourceManager::IsResourceValid(ResourceHandle resourceHandle)
	{
		//TODO:
		return true;
	}

	bool EditorResourceManager::IsResourceMissing(ResourceHandle resourceHandle)
	{
		auto metadata = GetMetadata(resourceHandle);
		return DiskFileSystem::Exists(Project::GetActiveProject()->GetAssetDirectory() / metadata.FilePath);
	}

	void EditorResourceManager::RemoveResource(ResourceHandle resourceHandle)
	{
		if (m_LoadedResources.contains(resourceHandle))
			m_LoadedResources.erase(resourceHandle);

		if (m_ResourceRegistry.contains(resourceHandle))
			m_ResourceRegistry.Remove(resourceHandle);
	}

	void EditorResourceManager::RegisterDependency(ResourceHandle resourceHandle, ResourceHandle dependency)
	{
		m_ResourceDependencies[resourceHandle].insert(dependency);
	}

	std::unordered_set<ResourceHandle> EditorResourceManager::GetAllResourcesWithType(ResourceType type)
	{
		std::unordered_map<ResourceHandle> result;

		for (const auto& [resourceHandle, metadata] : m_ResourceRegistry)
		{
			if (metadata.Type == type)
				result.insert(resourceHandle);
		}
		return result;
	}

	ResourceMetadata EditorResourceManager::GetMetadata(ResourceHandle resourceHandle)
	{
		if (m_ResourceRegistry.contains(handle))
			return m_ResourceRegistry.Get(resourceHandle);

		return s_NullMetadata;
	}
	void EditorResourceManager::SetMetadata(ResourceHandle resourcehandle, const ResourceMetadata& metadata)
	{
		m_ResourceRegistry.Set(resourcehandle, metadata);
	}
	const ResourceMetadata& EditorResourceManager::GetMetadata(const std::filesystem::path& filepath)
	{
		const auto relativePath = GetRelativePath(filepath);

		for (auto& [handle, metadata]: m_ResourceRegistry)
		{
			if (metadata.FilePath == relativePath)
				return metadata;
		}
	}
	ResourceHandle EditorResourceManager::ImportResource(const std::filesystem::path& filepath)
	{
		std::filesystem::path path = GetRelativePath(filepath);

		if (auto& metadata = GetMetadata(path); metadata.IsValid())
			return metadata.Handle;

		ResourceType type = GetResourceTypeFromPath(path);
		if (type == ResourceType::None)
			return 0;

		ResourceMetadata metadata;
		metadata.Handle = ResourceHandle();
		metadata.FilePath = path;
		metadata.Type = type;

		SetMetadata(metadata.Handle, metadata);

		return metadata.Handle;
	}
	ResourceHandle EditorResourceManager::GetResourceHandleFromFilePath(const std::filesystem::path& filepath)
	{
		return GetMetadata(filepath).Handle;
	}
	ResourceType EditorResourceManager::GetResourceTypeFromExtension(const std::string& extension)
	{
		//TODO(esgij): write StringUtils to lower the extension
		if (s_ResourceExtensionMap.find(extension) == s_ResourceExtensionMap.end())
			return ResourceType::None;

		return s_ResourceExtensionMap.at(extension.c_str());
	}
	std::string EditorResourceManager::GetDefaultExtensionForResourceType(ResourceType type)
	{
		for (const auto& [extension, resourceType] : s_ResourceExtensionMap)
		{
			if (resourceType == type)
				return extension;
		}
		return "";
	}
	ResourceType EditorResourceManager::GetResourceTypeFromPath(const std::filesystem::path& path)
	{
		return GetResourceTypeFromExtension(path.extension().string());
	}

	std::filesystem::path EditorResourceManager::GetFileSystemPath(const ResourceMetadata& metadata)
	{
		return Project::GetActiveResourceDirectory() / metadata.FilePath;
	}

	std::filesystem::path EditorResourceManager::GetFileSystemPath(ResourceHandle resourceHandle)
	{
		return GetFileSystemPath(GetMetadata(resourceHandle));
	}

	std::string EditorResourceManager::GetFileSystemPathString(const ResourceMetadata& metadata)
	{
		return GetFileSystemPath().string();
	}
	std::filesystem::path EditorResourceManager::GetRelativePath(const std::filesystem::path& filepath)
	{
		std::filesystem::path relativePath = filepath.lexically_normal();
		std::string temp = filepath.string();
		if (temp.find(Project::GetActiveResourceDirectory().string()) != std::string::npos)
		{
			relativePath = std::filesystem::relative(filepath, Project::GetActiveResourceDirectory());
			if (relativePath.empty())
			{
				relativePath = filepath.lexically_normal();
			}
		}
		return relativePath;
	}
	bool EditorResourceManager::FileExists(ResourceMetadata& metadata) const
	{
		DiskFileSystem::Exists(Project::GetActiveProject()->GetResourceDirectory() / metadata.FilePath);
	}
	void EditorResourceManager::LoadResourceRegistry()
	{
		const auto& resourceRegistryPath = Project::GetResourceRegistryPath();

		if (!DiskFileSystem::Exists(resourceRegistryPath))
			return;

		std::ifstream stream(resourceRegistryPath);

		json data;

		stream >> data;

		if (!data.contains("Resources") || !data["Resources"].is_array())
			return;

		const auto& handles = data["Resources"];
		for (auto entry: handles)
		{
			if (!entry.contains("FilePath" || !entry.contains("Handle" || !entry.contains("Type"))
				continue;
			
			std::string filePath = entry["FilePath"].get<std::string>();

			ResourceMetadata metadata;
			metadata.Handle = entry["Handle"].get<uint64_t>();
			metadata.FilePath = filePath;
			metadata.Type = (ResourceType)Utils::AssetTypeToString(entry["Type"].get<std::string>());

			if (metadata.Type == ResourceType::None)
				continue;

		}
	}
	void EditorResourceManager::ProcessDirectory(const std::filesystem::path& directoryPath)
	{
		for (auto& entry: std::filesystem::directory_iterator(directoryPath))
		{
			if (entry->is_directory())
				ProcessDirectory(entry->path())
			else
				ImportResource(entry->path());
		}
	}
	void EditorResourceManager::ReloadResources()
	{
		ProcessDirectory(Project::GetActiveResourceDirectory().string());
		WriteRegistryToFile();
	}
	void EditorResourceManager::WriteRegistryToFile()
	{
		struct ResourceRegistryEntry
		{
			std::string FilePath;
			ResourceType Type;
		};

		std::map<UUID, ResourceRegistryEntry> sortedMap;

		for (auto& [filePath, metadata]: m_ResourceRegistry)
		{
			if (!DiskFileSystem::Exists(GetFileSystemPath(metadata)))
				continue;

			std::string pathToSerialize = metadata.FilePath.string();
			std::replace(pathToSerialize.begin(), pathToSerialize.end(), '\\', '/');
			sortedMap[metadata.Handle] = { pathToSerialize, metadata.Type };
		}

		json data;
		data["Resources"] = json::array();

		for (const auto& [handle, entry]: sortedMap)
		{
			json jEntry;
			jEntry["Handle"] = static_cast<uint64_t>(handle);
			jEntry["FilePath"] = entry.FilePath;
			jEntry["Type"] = Utils::AssetTypeToString(entry.Type);
			data["Resources"].push_back(std::move(jEntry)); 
		}

		const auto& resourceRegistryPath = Project::GetResourceRegistryPath();
		std::ofstream out(resourceRegistryPath);

		out << data.dump(2);
	}
	void Luden::EditorResourceManager::OnResourceRenamed(ResourceHandle resourceHandle, const std::filesystem::path& newFilePath)
	{
		ResourceMetadata metadata = GetMetadata(resourceHandle);
		if (!metadata.IsValid())
			return;

		metadata.FilePath = GetRelativePath(newFilePath);
		SetMetadata(resourceHandle, metadata);
		WriteRegistryToFile();
	}
	void Luden::EditorResourceManager::OnResourceDeleted(ResourceHandle resourceHandle)
	{
		RemoveResource(resourceHandle);
		WriteRegistryToFile();
	}
}