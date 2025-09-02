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
		void DestroyEntity(const EntityID& uuid);

		EntityVec& GetEntities();

		EntityVec& GetEntities(const std::string& tag);

		const EntityMap& GetEntityMap();
		const EntityVec& GetEntityVec();

		void Clear();
		//TODO:
		/*
		const Entity& TryGetEntityWithUUID(const UUID& uuid) const;
		Entity& TryGetEntityWithUUID();

		const Entity& TryGetEntityWithTag() const;
		Entity& TryGetEntityWithTag();
		*/
	private:
		EntityVec m_Entities;
		EntityVec m_EntitiesToAdd;
		EntityMap m_EntityMap;
		size_t m_TotalEntities = 0;

		void RemoveDeadEntities(EntityVec& vec);
	};
}


