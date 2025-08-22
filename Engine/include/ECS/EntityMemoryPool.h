#pragma once

#include <cassert>
#include <memory>
#include <string>
#include <tuple>
#include <vector>

#include "Core/UUID.h"
#include "ECS/Components/Components.h"
#include "Reflection/ReflectionMacros.h"

#define MAX_ENTITIES 100000

namespace Luden
{
	class Entity;

	using PoolIndex = std::size_t;

	using EntityComponentVectorTuple = std::tuple<
		std::vector<Luden::CDamage>,
		std::vector<Luden::CDraggable>,
		std::vector<Luden::CFollowPlayer>,
		std::vector<Luden::CGravity>,
		std::vector<Luden::CHealth>,
		std::vector<Luden::CInput>,
		std::vector<Luden::CBoundingBox>,
		std::vector<Luden::CAnimation>,
		std::vector<Luden::CLifespan>,
		std::vector<Luden::CInvincibility>,
		std::vector<Luden::CPatrol>,
		std::vector<Luden::CState>,
		std::vector<Luden::CTransform>
	>;

	using EntityID = UUID;
	class ENGINE_API EntityMemoryPool
	{
		size_t m_NumEntities = 0;
		EntityComponentVectorTuple	m_Pool;
		std::vector<std::string>	m_Tags;
		std::vector<bool>			m_Active;
		std::vector<UUID>			m_IDs;

		std::unordered_map<UUID, PoolIndex> m_IdToIndex;
		std::vector<PoolIndex>				m_FreeList;


		size_t m_Capacity = 0;
		size_t m_NumAlive = 0;

		EntityMemoryPool(size_t maxEntities);

		PoolIndex AcquireIndex();
		void EnsureSizedFor(PoolIndex index);
		PoolIndex IndexOf(const UUID& entityID) const;
		void ClearComponentsAt(PoolIndex index);
	public:
		static EntityMemoryPool& Instance()
		{
			static EntityMemoryPool instance(MAX_ENTITIES);
			return instance;
		}

		Entity AddEntity(const std::string& tag);
		void DestroyEntity(const EntityID& entityID);


		const std::string& GetTag(const UUID& entityID) const;
		bool IsActive(const EntityID& entityID) const;

		template <typename T>
		T& GetComponent(const EntityID& entityID)
		{
			PoolIndex index = IndexOf(entityID);
			return std::get<std::vector<T>>(m_Pool)[index];
		}

		template <typename T>
		void RemoveComponent(const EntityID& entityID)
		{
			PoolIndex index = IndexOf(entityID);
			std::get<std::vector<T>>(m_Pool)[index].has = false;
		}

		template <typename T, typename... TArgs>
		T& AddComponent(const EntityID& entityID, TArgs&&... args)
		{
			PoolIndex index = IndexOf(entityID);

			auto& component = GetComponent<T>(index);
			component = T(std::forward<TArgs>(args)...);
			component.has = true;
			return component;
		}

		template <typename T>
		bool HasComponent(const EntityID& entityID) const
		{
			PoolIndex index = IndexOf(entityID);
			return std::get<std::vector<T>>(m_Pool)[index].has;
		}
	};
}
