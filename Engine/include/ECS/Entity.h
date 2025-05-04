#pragma once
#include <cstdint>
#include <string>
#include <tuple>

#include "EngineAPI.h"
#include "ECS/EntityMemoryPool.h"

namespace Luden 
{
	using EntityID = std::size_t;

	class ENGINE_API Entity
	{
	private:
		friend class EntityManager;
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
		T& Add(TArgs &&... mArgs) {
			return EntityMemoryPool::Instance().AddComponent<T>(m_ID, std::forward<TArgs>(mArgs)...);
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
			return EntityMemoryPool::Instance().RemoveComponent<T>(m_ID);
		}

		std::vector<void*> GetAllComponents();
		std::vector<const void*> GetAllComponents() const;
	};
}