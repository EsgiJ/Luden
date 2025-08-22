#pragma once

#include <map>
#include <memory>
#include <vector>

#include "Entity.h"

#include "EngineAPI.h"

namespace Luden
{
	using EntityVec = std::vector<Entity>;
	using EntityMap = std::map<std::string, EntityVec>;


	class ENGINE_API EntityManager {
	public:
		EntityManager();

		void Update();

		Entity AddEntity(const std::string& tag);

		EntityVec& GetEntities();

		EntityVec& GetEntities(const std::string& tag);

		const EntityMap& GetEntityMap();

	private:
		EntityVec m_entities;
		EntityVec m_entitiesToAdd;
		EntityMap m_entityMap;
		size_t m_totalEntities = 0;

		void RemoveDeadEntities(EntityVec& vec);
	};
}


