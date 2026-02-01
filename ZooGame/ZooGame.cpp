#include "ZooGame.h"
#include "ZooGameRuntimeApp.h"
#include "NativeScript/NativeScriptRegistry.h"
#include <iostream>

#include "CollectableMask.h"
#include "Elephant.h"
#include "ElephantTarget.h"
#include "MainMenu.h"
#include "Player.h"
#include "GameManager.h"
#include "Mask.h"
#include "RabbitPlatform.h"
#include "UIButton.h"
#include "Room.h"
#include "MonkeyArm.h"
#include "CollectableMask.h"
#include "ExitGate.h"
#include "WalkablePlatform.h"
#include "CollectableKey.h"
#include "CreditsGameManager.h"

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
		NativeScriptRegistry::RegisterScript<RabbitPlatform>("RabbitPlatform", resourceManager);
		NativeScriptRegistry::RegisterScript<ElephantTarget>("ElephantTarget", resourceManager);
		NativeScriptRegistry::RegisterScript<MonkeyArm>("MonkeyArm", resourceManager);
		NativeScriptRegistry::RegisterScript<Elephant>("Elephant", resourceManager);
		NativeScriptRegistry::RegisterScript<CollectableMask>("CollectableMask", resourceManager);
		NativeScriptRegistry::RegisterScript<WalkablePlatform>("WalkablePlatform", resourceManager);
		NativeScriptRegistry::RegisterScript<CollectableKey>("CollectableKey", resourceManager);
		NativeScriptRegistry::RegisterScript<ExitGate>("ExitGate", resourceManager);
		NativeScriptRegistry::RegisterScript<CreditsGameManager>("CreditsGameManager", resourceManager);

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
