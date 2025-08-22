#include "ECS/Entity.h"

#include <vector>

#include "ECS/EntityMemoryPool.h"

namespace Luden
{
	Entity::Entity(EntityID uuid) 
		: m_UUID(id) {}

	bool Entity::IsActive() const 
	{
		return EntityMemoryPool::Instance().IsActive(m_UUID);
	}

	const std::string& Entity::Tag() const 
	{
		return EntityMemoryPool::Instance().GetTag(m_UUID);
	}

	EntityID Entity::UUID() const 
	{
		return m_UUID;
	}

	void Entity::Destroy() 
	{
		EntityMemoryPool::Instance().DestroyEntity(m_UUID);
	}
}
