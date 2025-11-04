#pragma once
#include "NativeScript/IScriptModule.h"

class GameModule : public Luden::IScriptModule
{
public:
	virtual void OnLoad() override;
	virtual void OnUnload() override;
	virtual void RegisterScripts(Luden::ResourceManagerBase* resourceManager) override;
	virtual uint32_t GetVersion() const override;
};