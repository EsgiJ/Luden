#include "ZooGame.h"
#include "ZooGameRuntimeApp.h"
#include "NativeScript/NativeScriptRegistry.h"
#include <iostream>

#include "ElephantTarget.h"
#include "MainMenu.h"
#include "Player.h"
#include "GameManager.h"
#include "Mask.h"
#include "MonkeyBridge.h"
#include "RabbitPlatform.h"
#include "UIButton.h"
#include "Room.h"

namespace Luden
{
	void ZooGame::OnLoad()
	{
		std::cout << "[ZooGame] Module Loaded!" << std::endl;
	}

	void ZooGame::OnUnload()
	{
		std::cout << "[ZooGame] Module Unloaded!" << std::endl;
	}

	void ZooGame::RegisterScripts(ResourceManagerBase* resourceManager)
	{
		std::cout << "[ZooGame] Registering scripts..." << std::endl;

		NativeScriptRegistry::RegisterScript<Player>("Player", resourceManager);
		NativeScriptRegistry::RegisterScript<UIButton>("UIButton", resourceManager);
		NativeScriptRegistry::RegisterScript<MainMenu>("MainMenu", resourceManager);
		NativeScriptRegistry::RegisterScript<GameManager>("GameManager", resourceManager);
		NativeScriptRegistry::RegisterScript<Room>("Room", resourceManager);
		NativeScriptRegistry::RegisterScript<Mask>("Mask", resourceManager);
		NativeScriptRegistry::RegisterScript<MonkeyBridge>("MonkeyBridge", resourceManager);
		NativeScriptRegistry::RegisterScript<RabbitPlatform>("RabbitPlatform", resourceManager);
		NativeScriptRegistry::RegisterScript<ElephantTarget>("ElephantTarget", resourceManager);

	}

	uint32_t ZooGame::GetVersion() const
	{
		return 1;
	}

	RuntimeApplication* ZooGame::CreateRuntimeApplication(const ApplicationSpecification& spec)
	{
		return new ZooGameRuntimeApp(spec);
	}

	extern "C" GAME_MODULE_API IScriptModule* CreateScriptModule()
	{
		return new ZooGame();
	}

	extern "C" GAME_MODULE_API void DestroyScriptModule(IScriptModule* module)
	{
		delete module;
	}
}
