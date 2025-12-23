#include "Graphics/AnimationManager.h"
#include "ECS/Components/Components.h"
#include "Core/EngineContext.h"
#include "Scene/Scene.h"
#include "Resource/ResourceManager.h"

namespace Luden
{
	void AnimationManager::Update(TimeStep ts)
	{
		auto scene = GEngine.GetActiveScene();
		if (!scene)
			return;

		for (auto entity : scene->GetEntityManager().GetEntities())
		{
			if (!entity.Has<SpriteAnimatorComponent>())
				continue;

			auto& animatorComponent = entity.Get<SpriteAnimatorComponent>();

			if (animatorComponent.currentAnimationIndex >= animatorComponent.animationHandles.size())
				continue;

			ResourceHandle currentHandle = animatorComponent.animationHandles[animatorComponent.currentAnimationIndex];

			auto animationResource = std::static_pointer_cast<Animation>(Project::GetResourceManager()->GetResource(currentHandle));

			if (!animationResource || animationResource->GetFrameCount() == 0)
				continue;

			animatorComponent.frameTimer += static_cast<float>(ts) * animatorComponent.playbackSpeed;

			if (animatorComponent.currentFrame >= animationResource->GetFrameCount())
				animatorComponent.currentFrame = 0;

			const auto& currentFrame = animationResource->GetFrame(animatorComponent.currentFrame);

			if (animatorComponent.frameTimer >= currentFrame.duration)
			{
				animatorComponent.frameTimer = 0.0f;
				animatorComponent.currentFrame++;

				if (animatorComponent.currentFrame >= animationResource->GetFrameCount())
				{
					if (animationResource->IsLooping())
					{
						animatorComponent.currentFrame = 0;
					}
					else
					{
						animatorComponent.currentFrame = animationResource->GetFrameCount() - 1;
						animatorComponent.frameTimer = 0.0f;
					}
				}
			}
		}
	}
}