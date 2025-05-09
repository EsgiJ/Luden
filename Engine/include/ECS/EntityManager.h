#pragma once

#include <vector>
#include <string>
#include <tuple>
#include <memory>
#include <cassert>

#include "ECS/Entity.h"
#include "ECS/IComponent.h"

#define MAX_ENTITIES 100000

namespace Luden
{
	using EntityComponentVectorTuple = std::tuple<
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
		std::vector<CDraggable>
	>;

	class ENGINE_API EntityManager
	{
		size_t m_NumEntities = 0;
		EntityComponentVectorTuple m_Pool;
		std::vector<std::string> m_Tags;
		std::vector<bool> m_Active;

		EntityManager(size_t maxEntities);
		EntityID GetNextIndex();

	public:
		static EntityManager& Instance()
		{
			static EntityManager instance(MAX_ENTITIES);
			return instance;
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

		template <typename T, typename... TArgs>
		T& AddComponent(EntityID entityID, TArgs&&... args)
		{
			auto& component = GetComponent<T>(entityID);
			component = T(std::forward<TArgs>(args)...);
			component.has = true;
			return component;
		}

		template <typename T>
		bool HasComponent(EntityID entityID) const
		{
			return std::get<std::vector<T>>(m_Pool)[entityID].has;
		}

		const std::string& GetTag(EntityID entityID) const
		{
			return m_Tags[entityID];
		}

		bool IsActive(EntityID entityID) const
		{
			return m_Active[entityID];
		}

		Entity AddEntity(const std::string& tag);

		std::vector<void*> GetAllComponents(EntityID id);
		std::vector<const void*> GetAllComponents(EntityID id) const;
	};
}
