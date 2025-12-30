#pragma once

#include "Core/UUID.h"
#include "ECS/IComponent.h"
#include "ECS/Components/Components.h"
#include <cassert>
#include <memory>
#include <string>
#include <tuple>
#include <vector>
#include <unordered_map>

#define MAX_ENTITIES 100000

namespace Luden
{
	class Entity;
	class Scene;

	using PoolIndex = std::size_t;
	using EntityID = UUID;

	using EntityComponentVectorTuple = std::tuple<
		std::vector<Luden::RelationshipComponent>,
		std::vector<Luden::DamageComponent>,
		std::vector<Luden::DraggableComponent>,
		std::vector<Luden::FollowPLayerComponent>,
		std::vector<Luden::GravityComponent>,
		std::vector<Luden::HealthComponent>,
		std::vector<Luden::InputComponent>,
		std::vector<Luden::Camera2DComponent>,
		std::vector<Luden::RigidBody2DComponent>,
		std::vector<Luden::BoxCollider2DComponent>,
		std::vector<Luden::CircleCollider2DComponent>,
		std::vector<Luden::PrefabComponent>,
		std::vector<Luden::NativeScriptComponent>,
		std::vector<Luden::SpriteAnimatorComponent>,
		std::vector<Luden::TextComponent>,
		std::vector<Luden::SpriteRendererComponent>,
		std::vector<Luden::LifespanComponent>,
		std::vector<Luden::InvincibilityComponent>,
		std::vector<Luden::PatrolComponent>,
		std::vector<Luden::StateComponent>,
		std::vector<Luden::TransformComponent>
	>;

	class ENGINE_API EntityMemoryPool
	{
	public:
		static EntityMemoryPool& Instance()
		{
			static EntityMemoryPool instance(MAX_ENTITIES);
			return instance;
		}

		Entity AddEntity(const std::string& tag, Scene* scene);
		Entity AddEntity(const std::string& tag, const UUID& id, Scene* scene);
		void DestroyEntity(const EntityID& entityID);

		void Clear();

		const std::string& GetTag(const UUID& entityID) const;
		void SetTag(const EntityID& entityID, const std::string& tag);

		void SetActive(const EntityID& uuid, bool isActive);
		bool IsActive(const EntityID& entityID) const;
		bool Exists(const EntityID& entityID) const;

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
			auto& component = GetComponent<T>(entityID);
			component = T(std::forward<TArgs>(args)...);
			component.has = true;
			return component;
		}

		template<typename T>
		T& AddComponent(const EntityID& entityID, const T& component)
		{
			PoolIndex index = IndexOf(entityID);

			auto& destComponent = std::get<std::vector<T>>(m_Pool)[index];
			destComponent = component;
			destComponent.has = true;
			return destComponent;
		}

		template <typename T>
		bool HasComponent(const EntityID& entityID) const
		{
			PoolIndex index = IndexOf(entityID);
			return std::get<std::vector<T>>(m_Pool)[index].has;
		}
	private:
		PoolIndex AcquireIndex();
		void EnsureSizedFor(PoolIndex index);
		PoolIndex IndexOf(const UUID& entityID) const;
		void ClearComponentsAt(PoolIndex index);

	private:
		EntityMemoryPool(size_t maxEntities);

		size_t m_NumEntities = 0;
		EntityComponentVectorTuple	m_Pool;
		std::vector<std::string>	m_Tags;
		std::vector<bool>			m_Active;
		std::vector<UUID>			m_IDs;

		std::unordered_map<UUID, PoolIndex> m_IdToIndex;
		std::vector<PoolIndex>				m_FreeList;


		size_t m_Capacity = 0;
		size_t m_NumAlive = 0;

	};
}
