#include "PaddleGame.h"
#include "PaddleGameRuntimeApp.h"
#include "NativeScript/NativeScriptRegistry.h"
#include "Source/PaddleController.h"
#include "Source/GameManager.h"
#include "Source/BallController.h"
#include "Source/Brick.h"

#include <iostream>

namespace Luden
{
	void PaddleGame::OnLoad()
	{
		std::cout << "[PaddleGame] Module Loaded!" << std::endl;
	}

	void PaddleGame::OnUnload()
	{
		std::cout << "[PaddleGame] Module Unloaded!" << std::endl;
	}

	void PaddleGame::RegisterScripts(ResourceManagerBase* resourceManager)
	{
		std::cout << "[PaddleGame] Registering scripts..." << std::endl;

		NativeScriptRegistry::RegisterScript<PaddleController>("PaddleController", resourceManager);
		NativeScriptRegistry::RegisterScript<GameManager>("GameManager", resourceManager);
		NativeScriptRegistry::RegisterScript<BallController>("BallController", resourceManager);
		NativeScriptRegistry::RegisterScript<Brick>("Brick", resourceManager);
	}

	uint32_t PaddleGame::GetVersion() const
	{
		return 1;
	}

	RuntimeApplication* PaddleGame::CreateRuntimeApplication(const ApplicationSpecification& spec)
	{
		return new PaddleGameRuntimeApp(spec);
	}

	extern "C" GAME_MODULE_API IScriptModule* CreateScriptModule()
	{
		return new PaddleGame();
	}

	extern "C" GAME_MODULE_API void DestroyScriptModule(IScriptModule* module)
	{
		delete module;
	}
}
