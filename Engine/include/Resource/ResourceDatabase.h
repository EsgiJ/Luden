#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <memory>

#include "EngineAPI.h"

namespace Luden 
{
	struct ENGINE_API ResourceMeta
	{
		std::string uuid;
		std::string type;
		std::string sourcePath;
		std::string compiledPath;
		std::vector<std::string> dependencies;
	};

	class ENGINE_API ResourceDatabase 
	{
	public:
		bool LoadFromFile(const std::string& jsonPath);
		bool SaveToFile(const std::string& jsonPath);

		const ResourceMeta* GetByUUID(const std::string& uuid) const;
		const ResourceMeta* GetBySourcePath(const std::string& path) const;

		void AddOrUpdate(const ResourceMeta& meta);
		void RemoveByUUID(const std::string& uuid);

		const std::unordered_map<std::string, ResourceMeta>& GetAll() const;
	private:
		std::unordered_map<std::string, ResourceMeta> m_Database;
	};

}
