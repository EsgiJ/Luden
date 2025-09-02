#pragma once

#include "ECS/ISystem.h"

namespace Luden
{
	class AnimationSystem : public ISystem
	{
	public:
		void Update(float dt) override;
	};
}
