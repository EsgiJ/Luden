#include "GameModule.h"
#include <iostream>

using namespace Luden;

void GameModule::OnLoad()
{
	std::cout << "Game Module Loaded!" << std::endl;
}

void GameModule::OnUnload()
{
	std::cout << "Game Module Unloaded!" << std::endl;
}

void GameModule::RegisterScripts(ResourceManagerBase* resourceManager)
{
	std::cout << "Registering scripts..." << std::endl;
}

uint32_t GameModule::GetVersion() const
{
	return 1;
}

extern "C" GAME_MODULE_API IScriptModule* CreateScriptModule()
{
	return new GameModule();
}

extern "C" GAME_MODULE_API void DestroyScriptModule(IScriptModule* module) 
{
	delete module;
}