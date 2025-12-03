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
			if (!entity.Has<Animation2DComponent>())
				continue;

			auto& animationComponent = entity.Get<Animation2DComponent>();

			if (animationComponent.currentAnimationIndex >= animationComponent.animationHandles.size())
				continue;

			ResourceHandle currentHandle = animationComponent.animationHandles[animationComponent.currentAnimationIndex];

			auto animationResource = std::static_pointer_cast<Animation>(Project::GetResourceManager()->GetResource(currentHandle));

			if (!animationResource)
				continue;

			size_t totalFrames = animationResource->GetFrameCount();

			animationComponent.frameTimer++;

			if (animationComponent.frameTimer >= animationComponent.speed)
			{
				animationComponent.frameTimer = 0;
				animationComponent.currentFrame++;

				if (animationComponent.currentFrame >= totalFrames)
				{
					if (animationComponent.repeat)
					{
						animationComponent.currentFrame = 0;
					}
					else
					{
						animationComponent.currentFrame = totalFrames - 1;
					}
				}
			}
		}
	}
}