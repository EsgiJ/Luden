#include "ECS/EntityManager.h"

#include "ECS/EntityMemoryPool.h"

#include <algorithm>


namespace Luden
{
	EntityManager::EntityManager() = default;

	void EntityManager::Update() {
		for (const auto& entity : m_EntitiesToAdd) {
			m_Entities.push_back(entity);
			m_EntityMap[entity.Tag()].push_back(entity);
		}

		m_EntitiesToAdd.clear();

		RemoveDeadEntities(m_Entities);

		for (auto& [tag, entityVec] : m_EntityMap) {
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
		m_EntitiesToAdd.push_back(entity);

		return entity;
	}

	void DestroyEntity(const EntityID& uuid)
	{
		EntityMemoryPool::Instance().DestroyEntity(uuid);
	}

	EntityVec& EntityManager::GetEntities() {
		return m_Entities;
	}

	EntityVec& EntityManager::GetEntities(const std::string& tag) {
		return m_EntityMap[tag];
	}

	const EntityMap& EntityManager::GetEntityMap() {
		return m_EntityMap;
	}
	const EntityVec& EntityManager::GetEntityVec()
	{
		return m_Entities;
	}

	void EntityManager::Clear()
	{
		m_Entities.clear();
		m_EntitiesToAdd.clear();
		m_EntityMap.clear();
		m_TotalEntities = 0;
		EntityMemoryPool::Instance().Clear();
	}

	//TODO:
	/*
	const Entity& EntityManager::TryGetEntityWithUUID(const UUID& uuid) const
	{
		for (const Entity& entity: m_Entities)
		{
			if (entity.UUID() == uuid)
				return entity;
		}
	}
	Entity& EntityManager::TryGetEntityWithUUID()
	{
		// TODO: insert return statement here
	}
	const Entity& EntityManager::TryGetEntityWithTag() const
	{
		// TODO: insert return statement here
	}
	Entity& EntityManager::TryGetEntityWithTag()
	{
		// TODO: insert return statement here
	}
	*/
}
