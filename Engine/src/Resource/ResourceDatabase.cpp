#include "Resource/ResourceDatabase.h"

#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
namespace Luden {
	bool ResourceDatabase::LoadFromFile(const std::string& jsonPath)
	{
		std::ifstream in(jsonPath);
		if (!in.is_open())
			return false;

		json j;
		in >> j;

		for (const auto& item: j["resources"])
		{
			ResourceMeta meta;
			meta.uuid = item["uuid"];
			meta.type = item["type"];
			meta.sourcePath = item["source"];
			meta.compiledPath = item["compiled"];

			if (item.contains("deps"))
				meta.dependencies = item["deps"].get<std::vector<std::string>>();

			m_Database[meta.uuid] = std::move(meta);

			return true;
		}
	}
	bool ResourceDatabase::SaveToFile(const std::string& jsonPath)
	{
		json j;

		for (const auto& [uuid, meta] : m_Database)
		{
			j["resource"].push_back({
				{"uuid", uuid},
				{"type", meta.type},
				{"source", meta.sourcePath},
				{"compiled", meta.compiledPath},
				{"deps", meta.dependencies}
			});
		}

		std::ofstream out(jsonPath);
		if (!out.is_open())
			return false;

		out << j.dump();
		return true;
	}
	const ResourceMeta* ResourceDatabase::GetByUUID(const std::string& uuid) const
	{
		auto it = m_Database.find(uuid);

		if (it == m_Database.end())
			return nullptr;

		return &it->second;
	}
	const ResourceMeta* ResourceDatabase::GetBySourcePath(const std::string& path) const
	{
		for (const auto& [_, meta] : m_Database)
		{
			if (meta.sourcePath == path)
				return &meta;
		}
		return nullptr;
	}
	void ResourceDatabase::AddOrUpdate(const ResourceMeta& meta)
	{
		m_Database[meta.uuid] = meta;
	}
	void ResourceDatabase::RemoveByUUID(const std::string& uuid)
	{
		m_Database.erase(uuid);
	}
	const std::unordered_map<std::string, ResourceMeta>& ResourceDatabase::GetAll() const
	{
		return m_Database;
	}
}
