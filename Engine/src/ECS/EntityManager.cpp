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
		m_Active[index] = true;
		return Entity(index);
	}

	void EntityManager::DestroyEntity(EntityID entityID)
	{
		m_Active[entityID] = false;
	}
}
