#include "Tetris.h"
#include "NativeScript/NativeScriptRegistry.h"

namespace Luden
{
	void Tetris::OnLoad()
	{
		std::cout << "Game Module Loaded!" << std::endl;
	}

	void Tetris::OnUnload()
	{
		std::cout << "Game Module Unloaded!" << std::endl;
	}

	void Tetris::RegisterScripts(ResourceManagerBase* resourceManager)
	{
		std::cout << "Registering scripts..." << std::endl;
	}

	uint32_t Tetris::GetVersion() const
	{
		return 1;
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
