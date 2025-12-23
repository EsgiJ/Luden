#include "Resource/EditorResourceManager.h"

#include "Project/Project.h"
#include "Resource/ResourceExtensions.h"
#include "Resource/ResourceImporter.h"
#include "Resource/ResourceMetadata.h"
#include "IO/FileSystem.h"

#include <nlohmann/json.hpp>
#include <iostream>

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
	}

	void EditorResourceManager::Shutdown()
	{
		WriteRegistryToFile();
		m_LoadedResources.clear();
	}

	ResourceType EditorResourceManager::GetResourceType(ResourceHandle resourceHandle)
	{
		if (!IsResourceHandleValid(resourceHandle))
			return ResourceType::None;

		const auto& metadata = GetMetadata(resourceHandle);
		return metadata.Type;
	}

	std::shared_ptr<Resource> EditorResourceManager::GetResource(ResourceHandle resourceHandle)
	{
		if (!IsResourceHandleValid(resourceHandle))
			return nullptr;

		if (IsResourceLoaded(resourceHandle))
		{
			return m_LoadedResources.at(resourceHandle);
		}

		const ResourceMetadata& metadata = GetMetadata(resourceHandle);
		std::shared_ptr<Resource> resource = nullptr; 

		if (ResourceImporter::TryLoadData(metadata, resource))
		{
			m_LoadedResources[resourceHandle] = resource;
			return resource;
		}
		else
		{
			return nullptr;
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
		return true;
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
		return FileSystem::Exists(Project::GetActiveProject()->GetResourceDirectory() / metadata.FilePath);
	}

	ResourceHandle EditorResourceManager::CreateResource(ResourceType type,const std::filesystem::path& absolutePath)
	{
		if (type == ResourceType::None)
			return 0;

		std::filesystem::path relativePath = GetRelativePath(absolutePath);

		if (auto& existing = GetMetadata(relativePath); existing.IsValid())
			return existing.Handle;

		ResourceMetadata metadata;
		metadata.Handle = ResourceHandle();
		metadata.FilePath = relativePath;
		metadata.Type = type;

		SetMetadata(metadata.Handle, metadata);

		const std::string name = relativePath.stem().string();

		std::shared_ptr<Resource> resource = ResourceImporter::CreateResource(type, name);
		if (!resource)
			return 0;

		resource->Handle = metadata.Handle;

		ResourceImporter::Serialize(metadata, resource);

		ReloadResources();

		return metadata.Handle;
	}

	void EditorResourceManager::RemoveResource(ResourceHandle resourceHandle)
	{
		if (m_LoadedResources.contains(resourceHandle))
			m_LoadedResources.erase(resourceHandle);

		if (m_ResourceRegistry.Contains(resourceHandle))
			m_ResourceRegistry.Remove(resourceHandle);
	}

	void EditorResourceManager::RegisterDependency(ResourceHandle resourceHandle, ResourceHandle dependency)
	{
		m_ResourceDependencies[resourceHandle].insert(dependency);
	}

	std::unordered_set<ResourceHandle> EditorResourceManager::GetAllResourcesWithType(ResourceType type)
	{
		std::unordered_set<ResourceHandle> result;

		for (const auto& [resourceHandle, metadata] : m_ResourceRegistry)
		{
			if (metadata.Type == type)
				result.insert(resourceHandle);
		}
		return result;
	}

	ResourceMetadata EditorResourceManager::GetMetadata(ResourceHandle resourceHandle)
	{
		if (m_ResourceRegistry.Contains(resourceHandle))
			return m_ResourceRegistry.Get(resourceHandle);

		return s_NullMetadata;
	}

	void EditorResourceManager::SetMetadata(ResourceHandle resourceHandle, const ResourceMetadata& metadata)
	{
		m_ResourceRegistry.Set(resourceHandle, metadata);
	}

	const ResourceMetadata& EditorResourceManager::GetMetadata(const std::filesystem::path& filepath)
	{
		const auto relativePath = GetRelativePath(filepath);

		for (auto& [handle, metadata]: m_ResourceRegistry)
		{
			if (metadata.FilePath == relativePath)
				return metadata;
		}
		return s_NullMetadata;
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
		//TODO: write StringUtils to lower the extension
		if (s_ResourceExtensionMap.find(extension) == s_ResourceExtensionMap.end())
			return ResourceType::None;

		return s_ResourceExtensionMap.at(extension);
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
		return GetFileSystemPath(metadata).string();
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
		return FileSystem::Exists(Project::GetActiveProject()->GetResourceDirectory() / metadata.FilePath);
	}

	void EditorResourceManager::LoadResourceRegistry()
	{
		const auto& resourceRegistryPath = Project::GetResourceRegistryPath();

		if (!FileSystem::Exists(resourceRegistryPath))
			return;

		std::ifstream stream(resourceRegistryPath);

		json data;

		try
		{
			stream >> data;
		}
		catch (json::parse_error& e)
		{
			std::cerr << "Warning: Resource registry parse failed: " << e.what() << std::endl;
			return;
		}

		if (!data.contains("Resources") || !data["Resources"].is_array())
			return;

		const auto& handles = data["Resources"];
		for (auto entry: handles)
		{
			if (!entry.contains("FilePath") || !entry.contains("Handle") || !entry.contains("Type"))
				continue;
			
			std::string filePath = entry["FilePath"].get<std::string>();

			ResourceMetadata metadata;
			metadata.Handle = entry["Handle"].get<uint64_t>();
			metadata.FilePath = filePath;
			metadata.Type = (ResourceType)Utils::ResourceTypeToString(entry["Type"].get<std::string>());

			if (metadata.Type == ResourceType::None)
				continue;

			SetMetadata(metadata.Handle, metadata);
		}
	}

	void EditorResourceManager::ProcessDirectory(const std::filesystem::path& directoryPath)
	{
		for (auto entry: std::filesystem::directory_iterator(directoryPath))
		{
			if (entry.is_directory())
				ProcessDirectory(entry.path());
			else
				ImportResource(entry.path());
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
			if (!FileSystem::Exists(GetFileSystemPath(metadata)))
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
			jEntry["Type"] = Utils::ResourceTypeToString(entry.Type);
			data["Resources"].push_back(std::move(jEntry)); 
		}

		const auto& resourceRegistryPath = Project::GetResourceRegistryPath();
		std::ofstream out(resourceRegistryPath);

		out << data.dump(2);
	}

	void EditorResourceManager::OnResourceRenamed(ResourceHandle resourceHandle, const std::filesystem::path& newFilePath)
	{
		ResourceMetadata metadata = GetMetadata(resourceHandle);
		if (!metadata.IsValid())
			return;

		metadata.FilePath = GetRelativePath(newFilePath);
		SetMetadata(resourceHandle, metadata);
		WriteRegistryToFile();
	}
	void EditorResourceManager::OnResourceDeleted(ResourceHandle resourceHandle)
	{
		RemoveResource(resourceHandle);
		WriteRegistryToFile();
	}
}