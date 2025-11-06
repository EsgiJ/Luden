#pragma once

#include "NativeScript/NativeScript.h"
#include "EngineAPI.h"
#include "Resource/ResourceManagerBase.h"

#include <iostream>

namespace Luden
{
	class ENGINE_API NativeScriptRegistry
	{
	public:
		template<typename T>
		static void RegisterScript(const std::string& className, ResourceManagerBase* rm)
		{
			auto instantiateScript = []() -> ScriptableEntity* {
				return static_cast<ScriptableEntity*>(new T());
				};
			auto destroyScript = [](ScriptableEntity* instance) {
				delete instance;
				};

			auto script = std::make_shared<NativeScript>(className, instantiateScript, destroyScript);

			auto scriptHandles = rm->GetAllResourcesWithType(ResourceType::NativeScript);

			for (auto handle : scriptHandles)
			{
				auto existingScript = std::static_pointer_cast<NativeScript>(rm->GetResource(handle));

				if (existingScript && existingScript->GetClassName() == className)
				{
					script->Handle = handle;
					auto& resources = rm->GetLoadedResources()[handle];
					resources[handle] = std::static_pointer_cast<Resource>(script);

					std::cout << "Registered script: " << className
						<< " (Handle: " << handle << ")" << std::endl;
					return;
				}
			}

			std::cerr << "Warning: Script metadata not found for: " << className << std::endl;
		}
	};
}