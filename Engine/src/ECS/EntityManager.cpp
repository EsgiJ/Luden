#include "ECS/EntityManager.h"

#include "ECS/EntityMemoryPool.h"

#include <algorithm>


namespace Luden
{
	EntityManager::EntityManager() = default;

	void EntityManager::Update() {
		for (const auto& entity : m_entitiesToAdd) {
			m_entities.push_back(entity);
			m_entityMap[entity.Tag()].push_back(entity);
		}
		m_entitiesToAdd.clear();

		RemoveDeadEntities(m_entities);

		for (auto& [tag, entityVec] : m_entityMap) {
			RemoveDeadEntities(entityVec);
		}
	}

	void EntityManager::RemoveDeadEntities(EntityVec& vec) {
		std::erase_if(
			vec,
			[](const Entity& entity) {
				return !entity.IsActive();
			}
		);
	}

	Entity EntityManager::AddEntity(const std::string& tag) {
		Entity entity = EntityMemoryPool::Instance().AddEntity(tag);
		m_entitiesToAdd.push_back(entity);

		return entity;
	}

	EntityVec& EntityManager::GetEntities() {
		return m_entities;
	}

	EntityVec& EntityManager::GetEntities(const std::string& tag) {
		return m_entityMap[tag];
	}

	const EntityMap& EntityManager::GetEntityMap() {
		return m_entityMap;
	}
}
