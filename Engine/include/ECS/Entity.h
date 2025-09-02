#pragma once

#include <cstdint>
#include <string>
#include <tuple>
#include <vector>

#include "Core/UUID.h"
#include "ECS/IComponent.h"
#include "ECS/EntityMemoryPool.h"
#include "EngineAPI.h"

namespace Luden 
{
	class EntityMemoryPool;

	using EntityID = UUID;

	class ENGINE_API Entity
	{
	public:
		void Destroy();

		[[nodiscard]] EntityID UUID() const;

		void SetUUID(EntityID uuid) { m_UUID = uuid; }

		[[nodiscard]] bool IsActive() const;

		[[nodiscard]] const std::string& Tag() const;

		template<class T>
		bool Has() const 
		{
			return EntityMemoryPool::Instance().HasComponent<T>(m_UUID);
		}

		template<class T, typename... TArgs>
		T& Add(TArgs&&... args)
		{
			auto& component = EntityMemoryPool::Instance().template AddComponent<T>(m_UUID, std::forward<TArgs>(args)...);
			return component;
		}

		template<class T>
		T& Get() {
			return EntityMemoryPool::Instance().GetComponent<T>(m_UUID);
		}

		template<class T>
		const T& Get() const {
			return EntityMemoryPool::Instance().GetComponent<T>(m_UUID);
		}

		template<class T>
		void Remove() const {
			EntityMemoryPool::Instance().RemoveComponent<T>(m_UUID);
		}

	private:
		Entity();
		explicit Entity(EntityID uuid);

		EntityID m_UUID;

		friend class EntityMemoryPool;
	};
}