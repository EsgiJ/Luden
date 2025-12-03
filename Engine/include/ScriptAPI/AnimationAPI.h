#pragma once

#include "EngineAPI.h"
#include "Resource/Resource.h"

namespace Luden
{
	class Animation;
	class Entity;

	namespace AnimationAPI
	{
		ENGINE_API size_t AddAnimation(Entity entity, Animation* animation);

		ENGINE_API void PlayAnimation(Entity entity, Animation* animation);
		ENGINE_API void PlayAnimationByHandle(Entity entity, ResourceHandle animHandle);

		ENGINE_API Animation* GetCurrentAnimation(Entity entity);
		ENGINE_API ResourceHandle GetCurrentAnimationHandle(Entity entity);

		size_t GetAnimationIndex(Entity entity, Animation* animation);
		size_t GetAnimationIndexByHandle(Entity entity, ResourceHandle handle);

		ENGINE_API size_t GetCurrentAnimationIndex(Entity entity);
		ENGINE_API size_t GetCurrentFrame(Entity entity);

		ENGINE_API void SetAnimationSpeed(Entity entity, size_t speed);
		ENGINE_API void SetAnimationRepeat(Entity entity, bool repeat);

		ENGINE_API size_t GetAnimationCount(Entity entity);


	}
}