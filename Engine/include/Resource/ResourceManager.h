#pragma once

#include <unordered_map>
#include <memory>
#include <string>
#include <functional>

#include "EngineAPI.h"
#include "Resource/IResource.h"
#include "Resource/RuntimeResourceDatabase.h"

namespace Luden 
{

	class ENGINE_API ResourceManager 
	{
		using ResourceFactory = std::function<std::shared_ptr<IResource>()>;

		void SetFileSystem(std::shared_ptr<IFileSystem> fs);
		void SetRuntimeDatabase(std::shared_ptr<RuntimeResourceDatabase> db);
		void RegisterType(const std::string& type, ResourceFactory factory);

		std::shared_ptr<IResource> Load(const ResourceID& id, const std::string& path, const std::string& type);
		std::shared_ptr<IResource> Load(const ResourceID& uuid);
		std::shared_ptr<IResource> Get(const ResourceID& id) const;
		void Unload(const ResourceID& id);

	private:
		std::unordered_map<ResourceID, std::shared_ptr<IResource>> m_ResourceMap;
		std::unordered_map<std::string, ResourceFactory> m_Factories;
		std::shared_ptr<IFileSystem> m_FileSystem;
		std::shared_ptr<RuntimeResourceDatabase> m_RuntimeDB;
	};
}
