#pragma once


#include "EngineAPI.h"
#include "Project/Project.h"
#include "Resource/Resource.h"

namespace Luden
{
	class ENGINE_API ResourceManager
	{
	public:
		static bool IsResourceHandleValid(ResourceHandle resourceHandle) { return Project::GetResourceManager()->IsResourceHandleValid(resourceHandle); }
		static bool IsResourceValid(ResourceHandle resourceHandle) { return Project::GetResourceManager()->IsResourceValid(resourceHandle); }

		static bool IsResourceMissing(ResourceHandle resourceHandle) { return Project::GetResourceManager()->IsResourceMissing(resourceHandle); }


		static bool ReloadData(ResourceHandle resourceHandle) { return Project::GetResourceManager()->ReloadData(resourceHandle); }
		static bool EnsureCurrent(ResourceHandle resourceHandle) { return Project::GetResourceManager()->EnsureCurrent(resourceHandle); }
		static bool EnsureAllLoadedCurrent() { return Project::GetResourceManager()->EnsureAllLoadedCurrent(); }

		static ResourceType GetResourceType(ResourceHandle resourceHandle) { return Project::GetResourceManager()->GetResourceType(resourceHandle); }
 
		//TODO
		//static std::shared_ptr<Resource> GetPlaceholderResource(ResourceType type);

		template<typename T>
		static std::shared_ptr<T> GetResource(ResourceHandle handle)
		{
			std::shared_ptr<Resource> res = Project::GetResourceManager()->GetResource(handle);
			if (!res) return nullptr;
			//TODO: Is it safe?
			auto typed = std::dynamic_pointer_cast<T>(res);
			return typed;
		}

		template<typename T>
		static std::unordered_set<ResourceHandle> GetAllResourcesWithType()
		{
			return Project::GetResourceManager()->GetAllResourcesWithType(T::GetStaticType());
		}

		static const std::unordered_map<ResourceHandle, std::shared_ptr<Resource>>& GetLoadedResources() { return Project::GetResourceManager()->GetLoadedResources(); }


		static void RegisterDependency(ResourceHandle handle, ResourceHandle dependency) { return Project::GetResourceManager()->RegisterDependency(handle, dependency); }

		static void RemoveResource(ResourceHandle handle)
		{
			Project::GetResourceManager()->RemoveResource(handle);
		}
	};
}
