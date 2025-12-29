#include "Graphics/Animation.h"
#include "ScriptAPI/AnimationAPI.h"
#include "ECS/Components/Components.h"
#include "ECS/Entity.h"
#include "Resource/ResourceManager.h"


namespace Luden
{
	namespace AnimationAPI
	{
		size_t AddAnimation(Entity entity, Animation* animation)
		{
			if (!entity.Has<SpriteAnimatorComponent>() || animation == nullptr)
				return 0;

			auto& animationComponent = entity.Get<SpriteAnimatorComponent>();
			animationComponent.animationHandles.push_back(animation->Handle);

			return animationComponent.animationHandles.size() - 1;
		}

		void PlayAnimation(Entity entity, Animation* animation)
		{
			size_t index = GetAnimationIndex(entity, animation);
			if (index == size_t(-1))
				return;

			auto& anim = entity.Get<SpriteAnimatorComponent>();

			if (anim.currentAnimationIndex == index)
				return;

			anim.currentAnimationIndex = index;
			anim.currentFrame = 0;
			anim.frameTimer = 0;
		}

		void PlayAnimationByHandle(Entity entity, ResourceHandle animHandle)
		{
			size_t index = GetAnimationIndexByHandle(entity, animHandle);
			if (index == size_t(-1))
				return;

			auto& anim = entity.Get<SpriteAnimatorComponent>();

			if (anim.currentAnimationIndex == index)
				return;

			anim.currentAnimationIndex = index;
			anim.currentFrame = 0;
			anim.frameTimer = 0;
		}

		Animation* GetCurrentAnimation(Entity entity)
		{
			if (!entity.Has<SpriteAnimatorComponent>())
				return nullptr;

			auto& animationComponent = entity.Get<SpriteAnimatorComponent>();
			ResourceHandle handle = animationComponent.animationHandles[animationComponent.currentAnimationIndex];

			auto animationResource = ResourceManager::GetResource<Animation>(handle);
			return animationResource.get();
		}

		ResourceHandle GetCurrentAnimationHandle(Entity entity)
		{
			if (!entity.Has<SpriteAnimatorComponent>())
				return ResourceHandle();

			auto& animationComponent = entity.Get<SpriteAnimatorComponent>();
			return animationComponent.animationHandles[animationComponent.currentAnimationIndex];
		}

		size_t GetAnimationIndex(Entity entity, Animation* animation)
		{
			if (!entity.Has<SpriteAnimatorComponent>() || animation == nullptr)
				return static_cast<size_t>(-1);

			auto& anim = entity.Get<SpriteAnimatorComponent>();
			ResourceHandle target = animation->Handle;

			for (size_t i = 0; i < anim.animationHandles.size(); i++)
			{
				if (anim.animationHandles[i] == target)
					return i;
			}

			return static_cast<size_t>(-1);
		}

		size_t GetAnimationIndexByHandle(Entity entity, ResourceHandle handle)
		{
			if (!entity.Has<SpriteAnimatorComponent>())
				return size_t(-1);

			auto& anim = entity.Get<SpriteAnimatorComponent>();

			for (size_t i = 0; i < anim.animationHandles.size(); i++)
				if (anim.animationHandles[i] == handle)
					return i;

			return size_t(-1);
		}

		size_t GetCurrentAnimationIndex(Entity entity)
		{
			if (!entity.Has<SpriteAnimatorComponent>())
				return size_t(-1);

			auto& animationComponent = entity.Get<SpriteAnimatorComponent>();
			return animationComponent.currentAnimationIndex;
		}

		size_t GetCurrentFrame(Entity entity)
		{
			if (!entity.Has<SpriteAnimatorComponent>())
				return size_t(-1);

			auto& animationComponent = entity.Get<SpriteAnimatorComponent>();
			return animationComponent.currentFrame;
		}

		void SetAnimationSpeed(Entity entity, float speed)
		{
			if (!entity.Has<SpriteAnimatorComponent>())
				return;

			entity.Get<SpriteAnimatorComponent>().playbackSpeed = speed;
		}

		size_t GetAnimationCount(Entity entity)
		{
			if (!entity.Has<SpriteAnimatorComponent>())
				return size_t(-1);

			return entity.Get<SpriteAnimatorComponent>().animationHandles.size();
		}
	}
}