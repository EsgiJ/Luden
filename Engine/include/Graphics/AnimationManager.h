#pragma once

#include "EngineAPI.h"

namespace Luden
{
	class TimeStep;

	class ENGINE_API AnimationManager
	{
	public:
		static AnimationManager& Instance()
		{
			static AnimationManager s_Instance;
			return s_Instance;
		}

		AnimationManager(const AnimationManager&) = delete;
		AnimationManager& operator =(const AnimationManager&) = delete;

		void Update(TimeStep ts);

	private:
		AnimationManager() = default;
	};
}