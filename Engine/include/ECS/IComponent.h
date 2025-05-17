#pragma once

#include <rttr/type.h>
#include <rttr/rttr_enable.h>

#include "EngineAPI.h"
#include "Reflection/ReflectionMacros.h"

namespace Luden 
{
	struct ENGINE_API IComponent 
	{
	public:
		bool has = false;
		RTTR_ENABLE();
	};
}