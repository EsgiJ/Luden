#pragma once
#include "NativeScript/IScriptModule.h"

namespace Luden
{
	class Tetris : public IScriptModule
	{
	public:
		void OnLoad() override;
		void OnUnload() override;
		void RegisterScripts(ResourceManagerBase* resourceManager) override;
		uint32_t GetVersion() const override;
	};
}
