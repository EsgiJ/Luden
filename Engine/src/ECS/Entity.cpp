#include "ECS/Entity.h"

#include <vector>

#include "ECS/EntityMemoryPool.h"

namespace Luden
{
	Entity::Entity(EntityID uuid) 
		: m_UUID(uuid) {}

	bool Entity::IsActive() const 
	{
		if (!EntityMemoryPool::Instance().Exists(m_UUID))
		{
			return false;
		}

		return EntityMemoryPool::Instance().IsActive(m_UUID);
	}

	void Entity::SetTag(const std::string& tag)
	{
		return EntityMemoryPool::Instance().SetTag(m_UUID, tag);
	}

	const std::string& Entity::Tag() const
	{
		if (!EntityMemoryPool::Instance().Exists(m_UUID))
		{
			static const std::string invalidTag = "Invalid Entity";
			return invalidTag;
		}

		return EntityMemoryPool::Instance().GetTag(m_UUID);
	}

	void Entity::Destroy() 
	{
		EntityMemoryPool::Instance().SetActive(m_UUID, false);
	}
}
