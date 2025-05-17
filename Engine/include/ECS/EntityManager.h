#pragma once

#include <cassert>
#include <memory>
#include <string>
#include <tuple>
#include <vector>

#include <rttr/registration_friend.h>

#include "ECS/Components/Components.h"
#include "Reflection/ReflectionMacros.h"

#define MAX_ENTITIES 100000

namespace Luden
{
	class Entity;
	using EntityID = std::size_t;

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

	class ENGINE_API EntityManager
	{
		RTTR_REGISTRATION_FRIEND

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
	};
}
