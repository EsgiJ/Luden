#pragma once

#include "EngineAPI.h"
#include "Resource/ResourceManagerBase.h"
#include "Serialization/ResourcePack.h"

namespace Luden 
{
	class ENGINE_API RuntimeResourceManager : public ResourceManagerBase
	{
	public:
		RuntimeResourceManager();
		virtual ~RuntimeResourceManager();

		virtual void Shutdown() override;

		virtual ResourceType GetResourceType(ResourceHandle resourceHandle) override;
		virtual std::shared_ptr<Resource> GetResource(ResourceHandle resourceHandle) override;

		virtual bool ReloadData(ResourceHandle resourceHandle) override;
		virtual bool IsResourceHandleValid(ResourceHandle resourceHandle) override;
		virtual bool IsResourceLoaded(ResourceHandle resourceHandle) override;
		virtual bool IsResourceValid(ResourceHandle resourceHandle) override;
		virtual bool IsResourceMissing(ResourceHandle resourceHandle) override;
		virtual void RemoveResource(ResourceHandle resourceHandle) override;

		virtual void RegisterDependency(ResourceHandle resourceHandle, ResourceHandle dependency) override;

		virtual std::unordered_set<ResourceHandle> GetAllResourcesWithType(ResourceType type) override;
		virtual const std::unordered_map<ResourceHandle, std::shared_ptr<Resource>>& GetLoadedResources() override;

		// ------------- Runtime-only ----------------

		std::shared_ptr<Scene> LoadScene(ResourceHandle handle);

		void SetResourcePack(std::shared_ptr<ResourcePack> resourcePack);

	private:
		void UpdateDependencies(ResourceHandle handle) {}

	private:
		std::unordered_map<ResourceHandle, std::shared_ptr<Resource>> m_LoadedResources;
		std::unordered_set<ResourceHandle> m_PendingResources;

		std::shared_ptr<ResourcePack> m_ResourcePack;
		ResourceHandle m_ActiveScene = 0;
	};
}
