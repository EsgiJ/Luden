#include "SpaceWar.h"
#include "SpaceWarRuntimeApp.h"
#include "NativeScript/NativeScriptRegistry.h"

#include "Source/HealthBar.h"
#include "Source/Player.h"

#include <iostream>

namespace Luden
{
	void SpaceWar::OnLoad()
	{
		std::cout << "[SpaceWar] Module Loaded!" << std::endl;
	}

	void SpaceWar::OnUnload()
	{
		std::cout << "[SpaceWar] Module Unloaded!" << std::endl;
	}

	void SpaceWar::RegisterScripts(ResourceManagerBase* resourceManager)
	{
		std::cout << "[SpaceWar] Registering scripts..." << std::endl;
		
		// TODO: Register your native scripts here
		// Example:
		NativeScriptRegistry::RegisterScript<HealthBar>("HealthBar", resourceManager);
		NativeScriptRegistry::RegisterScript<Player>("Player", resourceManager);

	}

	uint32_t SpaceWar::GetVersion() const
	{
		return 1;
	}

	RuntimeApplication* SpaceWar::CreateRuntimeApplication(const ApplicationSpecification& spec)
	{
		return new SpaceWarRuntimeApp(spec);
	}

	extern "C" GAME_MODULE_API IScriptModule* CreateScriptModule()
	{
		return new SpaceWar();
	}

	extern "C" GAME_MODULE_API void DestroyScriptModule(IScriptModule* module)
	{
		delete module;
	}
}
