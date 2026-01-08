#pragma once
#include "EngineAPI.h"
#include "Luden.h"

namespace Luden
{
	class Entity;

	namespace AnimationAPI
	{
		ENGINE_API size_t AddAnimation(Entity entity, AnimationRef animation);
		ENGINE_API void PlayAnimation(Entity entity, AnimationRef animation);

		ENGINE_API void PlayAnimationByHandle(Entity entity, ResourceHandle animHandle);

		ENGINE_API AnimationRef GetCurrentAnimation(Entity entity);
		ENGINE_API ResourceHandle GetCurrentAnimationHandle(Entity entity);
		ENGINE_API size_t GetCurrentAnimationIndex(Entity entity);
		ENGINE_API size_t GetCurrentFrame(Entity entity);
		ENGINE_API size_t GetAnimationCount(Entity entity);

		ENGINE_API void SetAnimationSpeed(Entity entity, float speed);

		size_t GetAnimationIndex(Entity entity, AnimationRef animation);
		size_t GetAnimationIndexByHandle(Entity entity, ResourceHandle handle);
	}
}