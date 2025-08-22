#pragma once

#include "EngineAPI.h"
#include "Resource/ResourceManagerBase.h"

namespace Luden 
{
	class ENGINE_API RuntimeResourceManager : public ResourceManagerBase
	{
	public:
		RuntimeResourceManager() = default;
		virtual ~RuntimeResourceManager() = default;

		virtual void Shutdown() override;

		virtual ResourceType GetResourceType(ResourceHandle resourceHandle) override;
		virtual std::shared_ptr<Resource> GetResource(ResourceHandle resourceHandle) override;
		virtual std::shared_ptr<Resource> GetResource(ResourceHandle resourceHandle) override;

		virtual bool IsResourceHandleValid(ResourceHandle resourceHandle) override;
		virtual bool IsResourceLoaded(ResourceHandle resourceHandle) override;
		virtual bool IsResourceValid(ResourceHandle resourceHandle) override;
		virtual bool IsResourceMissing(ResourceHandle resourceHandle) override;
		virtual void RemoveResource(ResourceHandle resourceHandle) override;

		virtual void RegisterDependency(ResourceHandle resourceHandle, ResourceHandle dependency) override;

		virtual std::unordered_set<ResourceHandle> GetAllResourcesWithType(ResourceType type) override;
		virtual const std::unordered_map<ResourceHandle, std::shared_ptr<Resource>>& GetLoadedResources() override;
	};
}
