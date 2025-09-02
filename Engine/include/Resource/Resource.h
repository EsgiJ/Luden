#pragma once

#include "Resource/ResourceTypes.h"
#include "Core/UUID.h"
#include "EngineAPI.h"

namespace Luden 
{
	using ResourceHandle = UUID;
	class ENGINE_API Resource 
	{
	public:
		ResourceHandle Handle = 0;
		uint16_t Flags = (uint16_t)ResourceFlag::None;

		virtual ~Resource() {}

		static ResourceType GetStaticResourceType() { return ResourceType::None; }
		virtual ResourceType GetResourceType() const { return ResourceType::None; }

		virtual void OnDependencyUpdated(ResourceHandle handle) {}

		virtual bool operator ==(const Resource& other) const
		{
			return Handle == other.Handle;
		}

		virtual bool operator !=(const Resource& other) const
		{
			return !(*this == other);
		}
	private:

		bool IsValid() const { return ((Flags & (uint16_t)ResourceFlag::Missing) | (Flags & (uint16_t)ResourceFlag::Invalid)) == 0; }

		bool IsFlagSet(ResourceFlag flag) const { return (uint16_t)flag & Flags; }
		void SetFlag(ResourceFlag flag, bool value = true)
		{
			if (value)
				Flags |= (uint16_t)flag;
			else
				Flags &= ~(uint16_t)flag;
		}
	};
}
