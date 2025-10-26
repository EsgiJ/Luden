#include "ECS/EntityMemoryPool.h"

#include "ECS/Entity.h"


namespace Luden
{
	EntityMemoryPool::EntityMemoryPool(size_t maxEntities)
		: m_Capacity(maxEntities)
	{
		m_Tags.reserve(maxEntities);
		m_Active.reserve(maxEntities);
		m_IDs.reserve(maxEntities);

		std::apply([&](auto&... vecs)
			{
				(..., vecs.reserve(maxEntities));
			}, m_Pool);
	}

	void EntityMemoryPool::EnsureSizedFor(PoolIndex idx)
	{
		auto ensure = [idx](auto& v)
			{
				if (v.size() <= idx) v.resize(idx + 1);
			};

		ensure(m_Tags);
		ensure(m_Active);
		ensure(m_IDs);

		std::apply([&](auto&... vecs)
			{
				(..., ensure(vecs));
			}, m_Pool);
	}

	PoolIndex EntityMemoryPool::AcquireIndex()
	{
		if (!m_FreeList.empty())
		{
			PoolIndex idx = m_FreeList.back();
			m_FreeList.pop_back();
			return idx;
		}
		// New slot
		PoolIndex idx = static_cast<PoolIndex>(m_Tags.size());
		EnsureSizedFor(idx);
		return idx;
	}

	PoolIndex EntityMemoryPool::IndexOf(const UUID& entityID) const
	{
		auto it = m_IdToIndex.find(entityID);
		assert(it != m_IdToIndex.end() && "Invalid Entity UUID!");
		return it->second;
	}

	void EntityMemoryPool::ClearComponentsAt(PoolIndex idx)
	{
		auto clearHas = [idx](auto& vec)
			{
				if (idx < vec.size())
				{
					vec[idx].has = false;
				}
			};

		std::apply([&](auto&... vecs)
			{
				(..., clearHas(vecs));
			}, m_Pool);
	}

	Entity EntityMemoryPool::AddEntity(const std::string& tag)
	{
		UUID id;

		PoolIndex idx = AcquireIndex();
		EnsureSizedFor(idx);

		m_Tags[idx] = tag;
		m_Active[idx] = true;
		m_IDs[idx] = id;

		m_IdToIndex.emplace(id, idx);

		++m_NumAlive;
		Entity entity(id);

		return entity;
	}

	Entity EntityMemoryPool::AddEntity(const std::string& tag, const UUID& id)
	{
		PoolIndex idx = AcquireIndex();
		EnsureSizedFor(idx);

		m_Tags[idx] = tag;
		m_Active[idx] = true;
		m_IDs[idx] = id;

		m_IdToIndex[id] = idx;

		++m_NumAlive;
		Entity entity(id);
		return entity;
	}


	void EntityMemoryPool::DestroyEntity(const EntityID& entityID)
	{
		PoolIndex idx = IndexOf(entityID);

		ClearComponentsAt(idx);

		m_IdToIndex.erase(entityID);

		m_FreeList.push_back(idx);

		if (m_NumAlive > 0) 
			--m_NumAlive;
	}

	void EntityMemoryPool::Clear()
	{
		m_Tags.clear();
		m_Active.clear();
		m_IDs.clear();
		m_IdToIndex.clear();
		m_FreeList.clear();
		m_NumAlive = 0;

		// Clear all component vectors
		std::apply([&](auto&... vecs)
			{
				(..., vecs.clear());
			}, m_Pool);
	}

	const std::string& EntityMemoryPool::GetTag(const UUID& entityID) const
	{
		PoolIndex idx = IndexOf(entityID);
		return m_Tags[idx];
	}

	void EntityMemoryPool::SetTag(const EntityID& entityID, const std::string& tag)
	{
		PoolIndex idx = IndexOf(entityID);

		m_Tags[idx] = tag;
	}

	void EntityMemoryPool::SetActive(const EntityID& uuid, bool isActive)
	{
		PoolIndex idx = IndexOf(uuid);

		m_Active[idx] = isActive;
	}

	bool EntityMemoryPool::IsActive(const EntityID& entityID) const
	{
		PoolIndex idx = IndexOf(entityID);
		return m_Active[idx];
	}

	bool EntityMemoryPool::Exists(const EntityID& entityID) const
	{
		auto it = m_IdToIndex.find(entityID);
		if (it == m_IdToIndex.end())
			return false;

		return true;
	}
}
