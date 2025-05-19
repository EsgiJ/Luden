#include "ECS/EntityManager.h"

#include "ECS/Entity.h"


namespace Luden
{
	EntityManager::EntityManager(size_t maxEntities)
	{
		m_Tags.resize(maxEntities);
		m_Active.resize(maxEntities);

		std::apply([maxEntities](auto&... vecs)
		{
			(..., vecs.resize(maxEntities));
		}, m_Pool);
	}

	EntityID EntityManager::GetNextIndex()
	{
		for (EntityID i = 0; i < MAX_ENTITIES; ++i)
		{
			if (!m_Active[i])
				return i;
		}

		assert(false && "Entity limit reached!");
		return MAX_ENTITIES - 1;
	}

	Entity EntityManager::AddEntity(const std::string& tag)
	{
		EntityID index = GetNextIndex();
		m_Tags[index] = tag;
		m_Active[index] = 1;
		return Entity(index);
	}

	std::vector<Entity> EntityManager::GetEntitiesByTag(const std::string& tag)
	{
		std::vector<Entity> result;
		for (EntityID i = 0; i < m_Tags.size(); ++i)
		{
			if (m_Active[i] && m_Tags[i] == tag)
			{
				result.emplace_back(Entity(i));
			}
		}
		return result;
	}

	std::vector<Entity> EntityManager::GetAllEntities() const
	{
		std::vector<Entity> result;
		for (EntityID i = 0; i < MAX_ENTITIES; ++i)
		{
			if (m_Active[i])
			{
				result.emplace_back(Entity(i));
			}
		}
		return result;
	}

	void EntityManager::DestroyEntity(EntityID entityID)
	{
		m_Active[entityID] = 0;
	}
}
