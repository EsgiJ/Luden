#include "ECS/Entity.h"

namespace Luden
{
	Entity::Entity(EntityID id) 
		: m_ID(id) {}

	bool Entity::IsActive() const {
		return EntityMemoryPool::Instance().IsActive(m_ID);
	}

	const std::string& Entity::Tag() const {
		return EntityMemoryPool::Instance().GetTag(m_ID);
	}

	size_t Entity::Id() const {
		return m_ID;
	}

	void Entity::Destroy() {
		EntityMemoryPool::Instance().DestroyEntity(m_ID);
	}

	std::vector<void*> Entity::GetAllComponents() {
		EntityMemoryPool::Instance().GetAllComponents(m_ID);
	}

	std::vector<const void*> Entity::GetAllComponents() const {
		EntityMemoryPool::Instance().GetAllComponents(m_ID);
	}
}
