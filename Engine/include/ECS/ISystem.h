#include <rttr/rttr_enable.h>

#include "EngineAPI.h"
#include "Reflection/ReflectionMacros.h"

namespace Luden
{
	class ENGINE_API ISystem
	{
	public:
		virtual ~ISystem() = default;
		virtual void Update(float dt) = 0;
		RTTR_ENABLE();
	};
}
