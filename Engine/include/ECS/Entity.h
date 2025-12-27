#pragma once
#include "Core/UUID.h"
#include "EngineAPI.h"
#include "ECS/EntityMemoryPool.h"

#include <cstdint>
#include <string>
#include <vector>

namespace Luden
{
	class EntityMemoryPool; 
	class Scene;

	using EntityID = UUID;

	class ENGINE_API Entity
	{
	public:
		Entity() = default;

		void Destroy();
		EntityID UUID() const { return m_UUID; }
		void SetUUID(EntityID uuid) { m_UUID = uuid; }

		void SetTag(const std::string& tag);
		const std::string& Tag() const;

		void SetParent(Entity parent);
		Entity GetParent();

		void SetParentUUID(EntityID parent);
		EntityID GetParentUUID() const;

		std::vector<EntityID>& Children();
		const std::vector<EntityID>& Children() const;
		bool RemoveChild(Entity child);
		bool IsAncestorOf(Entity& entity) const;
		bool IsDescendantOf(Entity& entity) { return entity.IsAncestorOf(*this); }

		bool IsActive() const;
		bool IsValid() const { return m_UUID != 0; }

		operator bool() const { return IsValid(); }

		bool operator==(const Entity& other) const
		{
			return m_UUID == other.UUID();
		}

		bool operator!=(const Entity& other) const
		{
			return !(*this == other);
		}

		template<class T>
		bool Has() const
		{
			if (!EntityMemoryPool::Instance().Exists(m_UUID))
			{
				return false;
			}
			return EntityMemoryPool::Instance().HasComponent<T>(m_UUID);
		}

		template<class T, typename... TArgs>
		T& Add(TArgs&&... args)
		{
			auto& component = EntityMemoryPool::Instance().template AddComponent<T>(m_UUID, std::forward<TArgs>(args)...);
			return component;
		}

		template<class T>
		T& Add(const T& component)
		{
			return EntityMemoryPool::Instance().template AddComponent<T>(m_UUID, component);
		}

		template<class T>
		T& Get()
		{
			return EntityMemoryPool::Instance().GetComponent<T>(m_UUID);
		}

		template<class T>
		const T& Get() const
		{
			return EntityMemoryPool::Instance().GetComponent<T>(m_UUID);
		}

		template<class T>
		void Remove() const
		{
			EntityMemoryPool::Instance().RemoveComponent<T>(m_UUID);
		}

	private:
		Entity(EntityID uuid, Scene* scene);

		EntityID m_UUID = 0;
		Scene* m_Scene = nullptr;

		friend class EntityMemoryPool;
		friend class EntityManager;
		friend class ToolbarPanel;
		friend class SceneHierarchyPanel;
		friend class Scene;
		friend class ScriptableEntity;
	};
}

