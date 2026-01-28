#include "SpaceWar.h"
#include "SpaceWarRuntimeApp.h"
#include "NativeScript/NativeScriptRegistry.h"

#include "Source/HealthBar.h"
#include "Source/Player.h"
#include "Source/GameManager.h"

#include <iostream>

#include "Bullet.h"
#include "Enemy.h"
#include "EnemySpawner.h"


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
		NativeScriptRegistry::RegisterScript<HealthBar>("HealthBar", resourceManager);
		NativeScriptRegistry::RegisterScript<Player>("Player", resourceManager);
		NativeScriptRegistry::RegisterScript<GameManager>("GameManager", resourceManager);
		NativeScriptRegistry::RegisterScript<Enemy>("Enemy", resourceManager);
		NativeScriptRegistry::RegisterScript<EnemySpawner>("EnemySpawner", resourceManager);
		NativeScriptRegistry::RegisterScript<Bullet>("Bullet", resourceManager);
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
