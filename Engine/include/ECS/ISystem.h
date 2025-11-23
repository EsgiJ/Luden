#pragma once

#include "EngineAPI.h"

namespace Luden
{
	class ENGINE_API ISystem
	{
	public:
		virtual ~ISystem() = default;
		virtual void OnStart() = 0;
		virtual void OnUpdate(float dt) = 0;
		virtual void OnStop() = 0;
	};
}
