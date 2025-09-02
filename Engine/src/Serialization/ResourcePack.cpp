#include "Serialization/ResourcePack.h"

#include "Core/Platform.h"
#include "Resource/ResourceManager.h"
#include "Resource/ResourceImporter.h"
#include "Scene/Scene.h"
#include "Scene/SceneSerializer.h"
#include "Audio/SoundBuffer.h"

namespace Luden {

	ResourcePack::ResourcePack(const std::filesystem::path& path)
		: m_Path(path)
	{
	}

	void ResourcePack::AddResource(std::shared_ptr<Resource> resource)
	{
		/*
		if (!resource)
			return;

		ResourceHandle handle = resource->Handle;

		auto& resourceMap = m_File.Index.Resources;
		if (resourceMap.find(handle) != resourceMap.end())
		{
			HZ_CORE_WARN_TAG("ResourcePack", "Resource already present in resource pack");
			return;
		}

		ResourcePackFile::ResourceInfo& info = resourceMap[handle];
		info.Type = (uint16_t)resource->GetResourceType();
		*/
	}

	void ResourcePack::Serialize()
	{
		//m_Serializer.Serialize(m_Path, m_File);
	}

	std::shared_ptr<Scene> ResourcePack::LoadScene(ResourceHandle sceneHandle)
	{
		auto it = m_File.Index.Scenes.find(sceneHandle);
		if (it == m_File.Index.Scenes.end())
			return nullptr;

		const ResourcePackFile::SceneInfo& sceneInfo = it->second;

		FileStreamReader stream(m_Path);
		std::shared_ptr<Scene> scene = ResourceImporter::DeserializeSceneFromResourcePack(stream, sceneInfo);
		scene->Handle = sceneHandle;
		return scene;
	}

	std::shared_ptr<Resource> ResourcePack::LoadResource(ResourceHandle sceneHandle, ResourceHandle resourceHandle)
	{
		const ResourcePackFile::ResourceInfo* resourceInfo = nullptr;

		bool foundResource = false;
		if (sceneHandle)
		{
			// Fast(er) path
			auto it = m_File.Index.Scenes.find(sceneHandle);
			if (it != m_File.Index.Scenes.end())
			{
				const ResourcePackFile::SceneInfo& sceneInfo = it->second;
				auto resourceIt = sceneInfo.Resources.find(resourceHandle);
				if (resourceIt != sceneInfo.Resources.end())
				{
					foundResource = true;
					resourceInfo = &resourceIt->second;
				}
			}
		}

		if (!foundResource)
		{
			// Slow(er) path
			for (const auto& [handle, sceneInfo] : m_File.Index.Scenes)
			{
				auto resourceIt = sceneInfo.Resources.find(resourceHandle);
				if (resourceIt != sceneInfo.Resources.end())
				{
					resourceInfo = &resourceIt->second;
					break;
				}
			}

			if (!resourceInfo)
				return nullptr;
		}

		FileStreamReader stream(m_Path);
		std::shared_ptr<Resource> resource = ResourceImporter::DeserializeFromResourcePack(stream, *resourceInfo);
		if (!resource)
			return nullptr;

		resource->Handle = resourceHandle;
		return resource;
	}

	bool ResourcePack::IsResourceHandleValid(ResourceHandle resourceHandle) const
	{
		return m_ResourceHandleIndex.find(resourceHandle) != m_ResourceHandleIndex.end();
	}

	bool ResourcePack::IsResourceHandleValid(ResourceHandle sceneHandle, ResourceHandle resourceHandle) const
	{
		auto sceneIterator = m_File.Index.Scenes.find(sceneHandle);
		if (sceneIterator == m_File.Index.Scenes.end())
			return false;

		const auto& sceneInfo = sceneIterator->second;
		return sceneInfo.Resources.find(resourceHandle) != sceneInfo.Resources.end();
	}

	Buffer ResourcePack::ReadAppBinary()
	{
		FileStreamReader stream(m_Path);
		stream.SetStreamPosition(m_File.Index.PackedAppBinaryOffset);
		Buffer buffer;
		stream.ReadBuffer(buffer);
		return buffer;
	}

	uint64_t ResourcePack::GetBuildVersion()
	{
		return m_File.Header.BuildVersion;
	}

	ResourceType ResourcePack::GetResourceType(ResourceHandle sceneHandle, ResourceHandle resourceHandle) const
	{
		const ResourcePackFile::ResourceInfo* resourceInfo = nullptr;

		bool foundResource = false;
		if (sceneHandle)
		{
			// Fast(er) path
			auto it = m_File.Index.Scenes.find(sceneHandle);
			if (it != m_File.Index.Scenes.end())
			{
				const ResourcePackFile::SceneInfo& sceneInfo = it->second;
				auto resourceIt = sceneInfo.Resources.find(resourceHandle);
				if (resourceIt != sceneInfo.Resources.end())
				{
					foundResource = true;
					resourceInfo = &resourceIt->second;
				}
			}
		}

		if (!foundResource)
		{
			// Slow(er) path
			for (const auto& [handle, sceneInfo] : m_File.Index.Scenes)
			{
				auto resourceIt = sceneInfo.Resources.find(resourceHandle);
				if (resourceIt != sceneInfo.Resources.end())
				{
					resourceInfo = &resourceIt->second;
					break;
				}
			}

			if (!resourceInfo)
				return ResourceType::None;
		}

		return (ResourceType)resourceInfo->Type;
	}

	std::shared_ptr<ResourcePack> ResourcePack::CreateFromActiveProject(std::atomic<float>& progress)
	{
#define DEBUG_PRINT 1

		// Need to find all scenes and see which resources they use

		ResourcePackFile resourcePackFile;
		resourcePackFile.Header.BuildVersion = Platform::GetCurrentDateTimeU64();

		progress = 0.0f;

		std::unordered_set<ResourceHandle> fullResourceList;

		// Note: user could create more scenes on main thread while resource pack thread is busy serializing these ones!
		std::unordered_set<ResourceHandle> sceneHandles = ResourceManager::GetAllResourcesWithType<Scene>();
		uint32_t sceneCount = (uint32_t)sceneHandles.size();

		if (sceneCount == 0)
		{
			//TODO: Log error ("There are no scenes in the project.  Nothing to serialize to resource pack!");
			return nullptr;
		}

		float progressIncrement = 0.5f / (float)sceneCount;

		// Audio "files"
		std::unordered_set<ResourceHandle> audioFiles = ResourceManager::GetAllResourcesWithType<SoundBuffer>();
		fullResourceList.insert(audioFiles.begin(), audioFiles.end());

		for (const auto sceneHandle : sceneHandles)
		{
			const auto metadata = Project::GetEditorResourceManager()->GetMetadata(sceneHandle);

			std::shared_ptr<Scene> scene = std::shared_ptr<Scene>();
			SceneSerializer serializer(scene);
			//TODO: Log ("Deserializing Scene: {}", metadata.FilePath);
			if (serializer.Deserialize(Project::GetActiveResourceDirectory() / metadata.FilePath))
			{
				std::unordered_set<ResourceHandle> sceneResourceList = scene->GetResourceList();
				//TODO: Log("  Scene has {} used resources", sceneResourceList.size());

				std::unordered_set<ResourceHandle> sceneResourceListWithoutPrefabs = sceneResourceList;
				for (ResourceHandle resourceHandle : sceneResourceListWithoutPrefabs)
				{
					ResourceType type = ResourceManager::GetResourceType(resourceHandle);
					//TODO:
					/*
					if (type == ResourceType::Prefab)
					{
						std::shared_ptr<Prefab> prefab = ResourceManager::GetResource<Prefab>(resourceHandle);
						std::unordered_set<ResourceHandle> childPrefabResourceList = prefab->GetResourceList(true);  // Note: not thread-safe!
						sceneResourceList.insert(childPrefabResourceList.begin(), childPrefabResourceList.end());
					}
					*/
				}

				sceneResourceList.insert(audioFiles.begin(), audioFiles.end());

				ResourcePackFile::SceneInfo& sceneInfo = resourcePackFile.Index.Scenes[sceneHandle];
				for (ResourceHandle resourceHandle : sceneResourceList)
				{
					ResourcePackFile::ResourceInfo& resourceInfo = sceneInfo.Resources[resourceHandle];
					resourceInfo.Type = (uint16_t)ResourceManager::GetResourceType(resourceHandle);
				}

				fullResourceList.insert(sceneResourceList.begin(), sceneResourceList.end());
			}
			else
			{
				//TODO: Log ERROR "Failed to deserialize Scene: {} ({})", metadata.FilePath, sceneHandle);
			}
			progress = progress + progressIncrement;
		}

#if 0
		// Make sure all Prefab-referenced resources are included
		for (ResourceHandle handle : fullResourceList)
		{
			const auto& metadata = Project::GetEditorResourceManager()->GetMetadata(handle);
			if (metadata.Type == ResourceType::Prefab)
			{
				std::shared_ptr<Prefab> prefab = ResourceManager::GetResource<Prefab>(handle);
				std::unordered_set<ResourceHandle> childPrefabResourceList = prefab->GetResourceList(true);
				fullResourceList.insert(childPrefabResourceList.begin(), childPrefabResourceList.end());
			}
		}
#endif

		//TODO: Log("Project contains {} used resources", fullResourceList.size());

		Buffer appBinary;

		ResourcePackSerializer::Serialize(Project::GetActiveResourceDirectory() / "ResourcePack.hap", resourcePackFile, appBinary, progress);
		progress = 1.0f;

		std::unordered_map<ResourceHandle, ResourcePackFile::ResourceInfo> serializedResources;
		for (auto& [sceneHandle, sceneInfo] : resourcePackFile.Index.Scenes)
		{
			for (auto& [resourceHandle, resourceInfo] : sceneInfo.Resources)
			{
				if (serializedResources.find(resourceHandle) == serializedResources.end())
				{
					serializedResources[resourceHandle] = resourceInfo;
				}
			}
		}

		for (const auto& [handle, info] : serializedResources)
		{
			const auto& metadata = Project::GetEditorResourceManager()->GetMetadata(handle);
		}

		return nullptr;
	}

	std::shared_ptr<ResourcePack> ResourcePack::LoadActiveProject()
	{
		return Load(Project::GetActiveResourceDirectory() / "ResourcePack.hap");
	}

	std::shared_ptr<ResourcePack> ResourcePack::Load(const std::filesystem::path& path)
	{
		std::shared_ptr<ResourcePack> resourcePack = std::shared_ptr<ResourcePack>();
		resourcePack->m_Path = path;
		bool success = ResourcePackSerializer::DeserializeIndex(resourcePack->m_Path, resourcePack->m_File);
		if (!success)
			return nullptr;

		// Populate resource handle index
		const auto& index = resourcePack->m_File.Index;
		for (const auto& [sceneHandle, sceneInfo] : index.Scenes)
		{
			resourcePack->m_ResourceHandleIndex.insert(sceneHandle);
			for (const auto& [resourceHandle, resourceInfo] : sceneInfo.Resources)
			{
				resourcePack->m_ResourceHandleIndex.insert(resourceHandle);
			}
		}

		return resourcePack;
	}

}
