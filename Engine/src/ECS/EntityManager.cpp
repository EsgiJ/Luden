#include "ECS/EntityManager.h"

#include "ECS/EntityMemoryPool.h"

#include <algorithm>


namespace Luden
{
	EntityManager::EntityManager() = default;

	void EntityManager::Update(TimeStep ts) {
		for (const auto& entity : m_EntitiesToAdd) {
			m_Entities.push_back(entity);
			m_EntityMap[entity.Tag()].push_back(entity);
		}

		m_EntitiesToAdd.clear();

		for (const auto& uuid : m_EntitiesToDestroy)
		{
			DestroyEntityImmediate(uuid);
		}
		m_EntitiesToDestroy.clear();

		RemoveDeadEntities(m_Entities);

	}

	bool EntityManager::Exists(const UUID& uuid)
	{
		return EntityMemoryPool::Instance().Exists(uuid);
	}

	bool EntityManager::Exists(const UUID& uuid) const
	{
		return EntityMemoryPool::Instance().Exists(uuid);
	}

	void EntityManager::RemoveDeadEntities(EntityVec& vec) 
	{
		std::erase_if(
			vec,
			[](const Entity& entity) {
				if (!entity.IsActive())
				{
					EntityMemoryPool::Instance().DestroyEntity(entity.UUID());
					return true;
				}
				return false;
			}
		);

		for (auto& pair : m_EntityMap)
		{
			std::erase_if(
				pair.second,
				[](const Entity& entity) {
					return !entity.IsActive();
				}
			);
		}
	}

	Entity EntityManager::AddEntityImmediate(const std::string& tag, Scene* scene)
	{
		Entity entity = EntityMemoryPool::Instance().AddEntity(tag, scene);

		m_Entities.push_back(entity);
		m_EntityMap[tag].push_back(entity);

		return entity;
	}

	Entity EntityManager::AddEntityImmediate(const std::string& tag, const UUID& id, Scene* scene)
	{
		Entity entity = EntityMemoryPool::Instance().AddEntity(tag, id, scene);

		m_Entities.push_back(entity);
		m_EntityMap[tag].push_back(entity);

		return entity;
	}

	void EntityManager::DestroyEntityImmediate(const EntityID& uuid)
	{
		if (!EntityMemoryPool::Instance().Exists(uuid))
			return;

		std::erase_if(m_Entities, [uuid](const Entity& entity) {
			return entity.UUID() == uuid;
			});

		for (auto& pair : m_EntityMap)
		{
			std::erase_if(pair.second, [uuid](const Entity& entity) {
				return entity.UUID() == uuid;
				});
		}

		EntityMemoryPool::Instance().DestroyEntity(uuid);
	}

	Entity EntityManager::AddEntity(const std::string& tag, Scene* scene)
	{
		Entity entity = EntityMemoryPool::Instance().AddEntity(tag, scene);
		m_EntitiesToAdd.push_back(entity);

		return entity;
	}

	Entity EntityManager::AddEntity(const std::string& tag, const UUID& id, Scene* scene)
	{
		Entity entity = EntityMemoryPool::Instance().AddEntity(tag, id, scene);
		m_EntitiesToAdd.push_back(entity);

		return entity;
	}

	void EntityManager::DestroyEntity(const EntityID& uuid)
	{
    if (EntityMemoryPool::Instance().Exists(uuid))
    {
        EntityMemoryPool::Instance().SetActive(uuid, false);
        m_EntitiesToDestroy.push_back(uuid);
    }
	}

	Entity& EntityManager::GetEntity(const EntityID& uuid) 
	{
		if (!EntityMemoryPool::Instance().Exists(uuid))
			throw std::runtime_error("Entity with UUID not found!");

		for (auto& e : m_Entities)
		{
			if (e.UUID() == uuid)
				return e;
		}

		throw std::runtime_error("Entity not tracked in EntityManager!");
	}

	const Entity& EntityManager::GetEntity(const EntityID& uuid) const
	{
		if (!EntityMemoryPool::Instance().Exists(uuid))
			throw std::runtime_error("Entity with UUID not found!");

		for (auto& e : m_Entities)
		{
			if (e.UUID() == uuid)
				return e;
		}

		throw std::runtime_error("Entity not tracked in EntityManager!");
	}

	EntityVec& EntityManager::GetEntities() 
	{
		return m_Entities;
	}

	EntityVec& EntityManager::GetEntities(const std::string& tag) 
	{
		return m_EntityMap[tag];
	}

	EntityMap& EntityManager::GetEntityMap()
	{
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
	}

	Entity EntityManager::TryGetEntityWithUUID(const UUID& uuid) const
	{
		if (!EntityMemoryPool::Instance().Exists(uuid))
		{
			Entity invalidEntity;
			return invalidEntity;
		}

		for (auto& e : m_Entities)
		{
			if (e.UUID() == uuid)
				return e;
		}
		Entity invalidEntity;
		return invalidEntity;
	}

	Entity EntityManager::TryGetEntityWithTag(const std::string& tag) const
	{
		auto it = m_EntityMap.find(tag);
		if (it != m_EntityMap.end() && !it->second.empty())
			return it->second.front();
		Entity invalidEntity;
		return invalidEntity;
	}
}
