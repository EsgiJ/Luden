#include "ECS/EntityMemoryPool.h"
#include <tuple>
#include <cassert>

namespace Luden
{
	EntityMemoryPool::EntityMemoryPool(size_t maxEntities)
	{
		m_Tags.resize(maxEntities);
		m_Active.resize(maxEntities);

		std::apply([maxEntities](auto&... vecs)
		{
			(..., vecs.resize(maxEntities));
		}, m_Pool);
	}
	EntityID EntityMemoryPool::GetNextIndex()
	{
		for (EntityID i = 0; i < MAX_ENTITIES; ++i)
		{
			if (!m_Active[i])
			{
				return i;
			}

			assert(false && "Entity limit reached!");
			return MAX_ENTITIES - 1;
		}
	}
	void EntityMemoryPool::DestroyEntity(EntityID entityID)
	{
		m_Active[entityID] = false;
	}

	std::vector<void*> EntityMemoryPool::GetAllComponents(EntityID id) {
		std::vector<void*> components;
		std::apply([&](auto&... vecs) {
			(components.push_back(&vecs[id]), ...);
			}, m_Pool);
		return components;
	}

	std::vector<const void*> EntityMemoryPool::GetAllComponents(EntityID id) const {
		std::vector<const void*> components;
		std::apply([&](const auto&... vecs) {
			(components.push_back(&vecs[id]), ...);
			}, m_Pool);
		return components;
	}
}