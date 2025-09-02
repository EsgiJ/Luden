#pragma once

#include "EngineAPI.h"

namespace Luden
{
	class ENGINE_API ISystem
	{
	public:
		virtual ~ISystem() = default;
		virtual void Update(float dt) = 0;
	};
}
