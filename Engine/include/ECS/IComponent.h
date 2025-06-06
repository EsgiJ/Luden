#pragma once


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