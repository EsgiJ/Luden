#pragma once

#include <unordered_set>
#include <unordered_map>

#include "EngineAPI.h"
#include "Resource/Resource.h"
#include "Resource/ResourceMetadata.h"

namespace Luden 
{
	using ResourceMap = std::unordered_map<ResourceHandle, std::shared_ptr<Resource>>;

	class ENGINE_API ResourceManagerBase
	{
	public:
		ResourceManagerBase() = default;
		virtual ~ResourceManagerBase() = default;

		virtual void Shutdown() = 0;

		virtual ResourceType GetResourceType(ResourceHandle resourceHandle) = 0;
		virtual std::shared_ptr<Resource> GetResource(ResourceHandle resourceHandle) = 0;

		virtual bool ReloadData(ResourceHandle resourceHandle) = 0;
		virtual bool EnsureCurrent(ResourceHandle resourceHandle) = 0;
		virtual bool EnsureAllLoadedCurrent() = 0;
		virtual bool IsResourceHandleValid(ResourceHandle resourceHandle) = 0;
		virtual bool IsResourceLoaded(ResourceHandle resourceHandle) = 0;
		virtual bool IsResourceValid(ResourceHandle resourceHandle) = 0;
		virtual bool IsResourceMissing(ResourceHandle resourceHandle) = 0;
		virtual void RemoveResource(ResourceHandle resourceHandle) = 0;

		virtual void RegisterDependency(ResourceHandle resourceHandle, ResourceHandle dependency) = 0;

		virtual std::unordered_set<ResourceHandle> GetAllResourcesWithType(ResourceType type) = 0;
		virtual const std::unordered_map<ResourceHandle, std::shared_ptr<Resource>>& GetLoadedResources() = 0;
	};
}
