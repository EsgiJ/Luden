#pragma once

#include <rttr/type.h>
#include <rttr/rttr_enable.h>

#include "EngineAPI.h"
#include "Reflection/ReflectionMacros.h"

namespace Luden 
{
	struct ENGINE_API COMPONENT() IComponent 
	{
	public:
		PROPERTY() bool has = false;
		RTTR_ENABLE();
	};
}