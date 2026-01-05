#include "Tetris.h"
#include "TetrisRuntimeApp.h"
#include "NativeScript/NativeScriptRegistry.h"
#include <iostream>

namespace Luden
{
	void Tetris::OnLoad()
	{
		std::cout << "[Tetris] Module Loaded!" << std::endl;
	}

	void Tetris::OnUnload()
	{
		std::cout << "[Tetris] Module Unloaded!" << std::endl;
	}

	void Tetris::RegisterScripts(ResourceManagerBase* resourceManager)
	{
		std::cout << "[Tetris] Registering scripts..." << std::endl;
		
		// TODO: Register your native scripts here
		// Example:
		// REGISTER_SCRIPT(resourceManager, PlayerController);
		// REGISTER_SCRIPT(resourceManager, EnemyAI);
	}

	uint32_t Tetris::GetVersion() const
	{
		return 1;
	}

	RuntimeApplication* Tetris::CreateRuntimeApplication(const ApplicationSpecification& spec)
	{
		return new TetrisRuntimeApp(spec);
	}

	extern "C" GAME_MODULE_API IScriptModule* CreateScriptModule()
	{
		return new Tetris();
	}

	extern "C" GAME_MODULE_API void DestroyScriptModule(IScriptModule* module)
	{
		delete module;
	}
}
