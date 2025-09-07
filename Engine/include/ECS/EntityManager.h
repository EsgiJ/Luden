#pragma once

#include "Core/TimeStep.h"
#include "Entity.h"
#include "EngineAPI.h"

#include <map>
#include <memory>
#include <vector>

namespace Luden
{
	using EntityVec = std::vector<Entity>;
	using EntityMap = std::map<std::string, EntityVec>;

	class ENGINE_API EntityManager {
	public:
		EntityManager();

		void Update(TimeStep ts);

		Entity AddEntity(const std::string& tag);
		void DestroyEntity(const EntityID& uuid);

		Entity& GetEntity(const EntityID& uuid);
		const Entity& GetEntity(const EntityID& uuid) const;

		EntityVec& GetEntities();
		EntityVec& GetEntities(const std::string& tag);

		const EntityMap& GetEntityMap();
		const EntityVec& GetEntityVec();

		void Clear();

		Entity TryGetEntityWithUUID(const UUID& uuid);
		Entity TryGetEntityWithTag(const std::string& tag);

		bool Exists(const UUID& uuid);
		bool Exists(const UUID& uuid) const;
	private:
		EntityVec m_Entities;
		EntityVec m_EntitiesToAdd;
		EntityMap m_EntityMap;
		size_t m_TotalEntities = 0;

		void RemoveDeadEntities(EntityVec& vec);
	};
}


