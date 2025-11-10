#pragma once

#include "ECS/ISystem.h"

namespace Luden
{
	class AnimationSystem : public ISystem
	{
	public:
		void OnStart() override;
		void OnUpdate(float dt) override;
		void OnStop() override;
	};
}
