#pragma once
#include "ECS/Entity.h"
#include "ECS/IComponent.h"
#include <string>

#define MAX_ENTITIES 100000

namespace Luden
{
	typedef std::tuple<
		std::vector<CTransform>,
		std::vector<CLifespan>,
		std::vector<CDamage>,
		std::vector<CInvincibility>,
		std::vector<CHealth>,
		std::vector<CInput>,
		std::vector<CBoundingBox>,
		std::vector<CAnimation>,
		std::vector<CGravity>,
		std::vector<CState>,
		std::vector<CFollowPlayer>,
		std::vector<CPatrol>,
		std::vector<CDraggable>,
	> EntityComponentVectorTuple;

	class EntityMemoryPool
	{
		size_t m_NumEntities;
		EntityComponentVectorTuple m_Pool;
		std::vector<std::string> m_Tags;
		std::vector<bool> m_Active;

		EntityMemoryPool(size_t maxEntities);

		EntityID GetNextIndex();
	public:
		static EntityMemoryPool& Instance() 
		{
			static EntityMemoryPool pool(MAX_ENTITIES);
			return pool;
		}

		void DestroyEntity(EntityID entityID);

		template <typename T>
		T& GetComponent(EntityID entityID) 
		{
			return std::get<std::vector<T>>(m_Pool)[entityID];
		}

		template <typename T>
		void RemoveComponent(EntityID entityID) 
		{
			std::get<std::vector<T>>(m_Pool)[entityID].has = false;
		}

		template<class T, typename... TArgs>
		T& AddComponent(EntityID entityID, TArgs &&... mArgs)
		{
			auto& component = GetComponent<T>(entityID);
			component = T(std::forward<TArgs>(mArgs)...);
			component.has = true;
			return component;
		}

		const std::string& GetTag(EntityID entityID) const 
		{
			return m_Tags[entityID];
		}

		template <typename T>
		bool HasComponent(EntityID entityID) const 
		{
			return std::get<std::vector<T>>(m_pool)[entityID].has;
		}

		bool IsActive(EntityID entityID) const 
		{
			return m_Active[entityID];
		}

		Entity AddEntity(const std::string& tag)
		{
			EntityID index = GetNextIndex();

			m_Tags[index] = tag;
			m_Active[index] = true;
			return Entity(index);
		}

		std::vector<void*> GetAllComponents(EntityID id);
		std::vector<const void*> GetAllComponents(EntityID id) const;
	};
}

