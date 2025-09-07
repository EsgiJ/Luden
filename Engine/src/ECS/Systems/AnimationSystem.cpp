#include "ECS/Systems/AnimationSystem.h"

#include "Core/RuntimeApplication.h"
#include "Scene/Scene.h"
#include "ECS/Entity.h"
#include "ECS/Components/Components.h"
#include "Project/Project.h"
#include "Graphics/Animation.h"

namespace Luden
{
	void AnimationSystem::Update(float dt)
	{	
		/*auto& entities = GameEngine::Get().GetCurrentScene()->GetEntityManager().GetEntities();
		for (Entity& e : entities)
		{
			if (!e.Has<CAnimation>()) continue;

			auto& cAnim = e.Get<CAnimation>();
			auto anim = std::static_pointer_cast<Graphics::Animation>(Project::GetResourceManager()->GetResource(cAnim.animationHandle));
			if (!anim) continue;

			cAnim.currentFrame++;

			if (cAnim.speed == 0 || anim->GetFrameCount() == 0)
				continue;

			size_t frame = (cAnim.currentFrame / cAnim.speed) % anim->GetFrameCount();
			anim->GetSprite().setTextureRect(sf::IntRect(
				{ static_cast<int>(frame * anim->GetSize().x), 0 },
				{ static_cast<int>(anim->GetSize().x), static_cast<int>(anim->GetSize().y) }
			));
		}
	*/
	}
}
