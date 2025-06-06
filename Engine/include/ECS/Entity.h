#pragma once

#include <cstdint>
#include <string>
#include <tuple>
#include <vector>


#include "ECS/IComponent.h"
#include "ECS/EntityMemoryPool.h"
#include "EngineAPI.h"

namespace Luden 
{
	class EntityMemoryPool;

	using EntityID = std::size_t;

	class ENGINE_API Entity
	{
		RTTR_REGISTRATION_FRIEND
		RTTR_ENABLE()

	private:
		friend class EntityMemoryPool;

		EntityID m_ID = 0;

		Entity() = default;
		explicit Entity(EntityID id);

	public:
		void Destroy();

		[[nodiscard]] EntityID Id() const;

		[[nodiscard]] bool IsActive() const;

		[[nodiscard]] const std::string& Tag() const;

		template<class T>
		bool Has() const 
		{
			return EntityMemoryPool::Instance().HasComponent<T>(m_ID);
		}

		template<class T, typename... TArgs>
		T& Add(TArgs&&... args)
		{
			auto& component = EntityMemoryPool::Instance().template AddComponent<T>(m_ID, std::forward<TArgs>(args)...);
			return component;
		}

		template<class T>
		T& Get() {
			return EntityMemoryPool::Instance().GetComponent<T>(m_ID);
		}

		template<class T>
		const T& Get() const {
			return EntityMemoryPool::Instance().GetComponent<T>(m_ID);
		}

		template<class T>
		void Remove() const {
			EntityMemoryPool::Instance().RemoveComponent<T>(m_ID);
		}
	};
}