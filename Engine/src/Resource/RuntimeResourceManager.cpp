#include "Resource/RuntimeResourceManager.h"

#include "Core/RuntimeApplication.h"
#include "Resource/ResourceImporter.h"
#include "Resource/ResourceManager.h"
#include "Scene/Scene.h"

namespace Luden {

	RuntimeResourceManager::RuntimeResourceManager()
	{
		ResourceImporter::Init();
	}

	RuntimeResourceManager::~RuntimeResourceManager()
	{
	}

	ResourceType RuntimeResourceManager::GetResourceType(ResourceHandle resourceHandle)
	{
		std::shared_ptr<Resource> resource = GetResource(resourceHandle);
		if (!resource)
			return ResourceType::None;

		return resource->GetResourceType();
	}

	std::shared_ptr<Resource> RuntimeResourceManager::GetResource(ResourceHandle resourceHandle)
	{
		std::shared_ptr<Resource> resource = nullptr;
		if (IsResourceLoaded(resourceHandle))
		{
			resource = m_LoadedResources[resourceHandle];
		}
		else
		{
			resource = m_ResourcePack->LoadResource(m_ActiveScene, resourceHandle);
			if (resource)
				m_LoadedResources[resourceHandle] = resource;
		}
		return resource;
	}

	bool RuntimeResourceManager::ReloadData(ResourceHandle resourceHandle)
	{
		std::shared_ptr<Resource> resource = m_ResourcePack->LoadResource(m_ActiveScene, resourceHandle);
		if (resource)
			m_LoadedResources[resourceHandle] = resource;

		if (resource == nullptr)
			return false;

		return true;
	}

	bool RuntimeResourceManager::EnsureCurrent(ResourceHandle resourceHandle)
	{
		//TODO:
		return false;
	}

	bool RuntimeResourceManager::EnsureAllLoadedCurrent()
	{
		//TODO:
		return false;
	}

	bool RuntimeResourceManager::IsResourceHandleValid(ResourceHandle resourceHandle)
	{
		if (resourceHandle == 0)
			return false;

		return m_ResourcePack && m_ResourcePack->IsResourceHandleValid(resourceHandle);
	}

	bool RuntimeResourceManager::IsResourceLoaded(ResourceHandle handle)
	{
		return m_LoadedResources.contains(handle);
	}

	bool RuntimeResourceManager::IsResourceValid(ResourceHandle handle)
	{
		return GetResource(handle) != nullptr;
	}

	bool RuntimeResourceManager::IsResourceMissing(ResourceHandle handle)
	{
		return !IsResourceValid(handle);
	}


	void RuntimeResourceManager::RemoveResource(ResourceHandle handle)
	{
		if (m_LoadedResources.contains(handle))
			m_LoadedResources.erase(handle);

	}

	std::unordered_set<ResourceHandle> RuntimeResourceManager::GetAllResourcesWithType(ResourceType type)
	{
		std::unordered_set<ResourceHandle> result;
		
		return result;
	}

	std::shared_ptr<Scene> RuntimeResourceManager::LoadScene(ResourceHandle handle)
	{
		std::shared_ptr<Scene> scene = m_ResourcePack->LoadScene(handle);
		if (scene)
			m_ActiveScene = handle;

		return scene;
	}

	void RuntimeResourceManager::SetResourcePack(std::shared_ptr<ResourcePack> resourcePack)
	{
		m_ResourcePack = resourcePack;
	}

}
