#include "Resource/ResourceManager.h"

namespace Luden {
	void ResourceManager::SetFileSystem(std::shared_ptr<IFileSystem> fs)
	{
		m_FileSystem = fs;
	}

	void ResourceManager::SetRuntimeDatabase(std::shared_ptr<RuntimeResourceDatabase> db)
	{
		m_RuntimeDB = db;
	}

	void ResourceManager::RegisterType(const std::string& type, ResourceFactory factory)
	{
		m_Factories[type] = factory;
	}

	std::shared_ptr<IResource> ResourceManager::Load(const ResourceID& id, const std::string& path, const std::string& type)
	{
		// If already exists just return the resource
		if (m_ResourceMap.contains(id))
			return m_ResourceMap[id];

		if (!m_FileSystem)
			return nullptr;

		auto file = m_FileSystem->Open(path, FileMode::Read);
		if (!file)
			return nullptr;

		auto it = m_Factories.find(type);
		if (it == m_Factories.end())
			return nullptr;

		auto resource = it->second();
		if (!resource)
			return nullptr;

		resource->Init();

		m_ResourceMap[id] = resource;
		return resource;
	}

	std::shared_ptr<IResource> ResourceManager::Load(const ResourceID& uuid)
	{
		if (m_ResourceMap.contains(uuid))
			return m_ResourceMap[uuid];

		if (!m_FileSystem || !m_RuntimeDB)
			return nullptr;

		const std::string* type = m_RuntimeDB->GetType(uuid);
		if (!type)
			return nullptr;

		auto it = m_Factories.find(*type);
		if (it == m_Factories.end())
			return nullptr;

		auto uptr2 = m_FileSystem->Open(uuid, FileMode::Read);
		if (!uptr2)
			return nullptr;
		std::shared_ptr<IFile> file(std::move(uptr2));

		auto resource = it->second();
		if (!resource)
			return nullptr;

		if (!resource->Load(file))
			return nullptr;

		resource->Init();
		m_ResourceMap[uuid] = resource;
		return resource;
	}

	std::shared_ptr<IResource> ResourceManager::Get(const ResourceID& id) const
	{
		auto it = m_ResourceMap.find(id);

		if (it == m_ResourceMap.end())
			return nullptr;
		return it->second;
	}

	void ResourceManager::Unload(const ResourceID& id)
	{
		auto it = m_ResourceMap.find(id);

		if (it != m_ResourceMap.end())
		{
			it->second->Unload();
			m_ResourceMap.erase(id);
		}
	}
}
