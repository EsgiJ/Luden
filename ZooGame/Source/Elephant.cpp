#include "Elephant.h"

#include <iostream>
#include <ScriptAPI/AnimationAPI.h>
#include <ScriptAPI/GameplayAPI.h>
#include "ElephantTarget.h"

namespace Luden
{
    void Elephant::OnCreate()
    {
        m_EmptyAnim = GetResource<Animation>("ElephantIdleAnim");
        m_ElephantPushAnim = GetResource<Animation>("ElephantPushAnim");
        m_ElephantPullAnim = GetResource<Animation>("ElephantPullAnim");
        m_ElephantSquirtAnim = GetResource<Animation>("ElephantSquirtAnim");
        AnimationAPI::PlayAnimation(GetEntity(), m_EmptyAnim);

        Entity elephantRoot = GetEntity().GetParent();
        if (elephantRoot.IsValid())
        {
            auto children = GameplayAPI::GetChildren(elephantRoot);
            for (auto child : children)
            {
                if (child.IsValid() && child.Tag() == "ElephantTarget")
                {
                    m_ElephantTarget = child;
                    std::cout << "[Elephant] Found ElephantTarget!" << std::endl;
                    break;
                }
            }
        }
    }

    void Elephant::OnUpdate(TimeStep ts)
    {
        if (AnimTriggered)
        {
            PassedTime += ts;
        }

        if (m_ElephantTarget.IsValid())
        {
            auto script = GameplayAPI::GetScript<ElephantTarget>(m_ElephantTarget);
            if (script != nullptr)
            {
                Vec3 elephantPos = GameplayAPI::GetPosition(GetEntity());

                Vec3 elephantScale = GameplayAPI::GetScale(GetEntity());
                float direction = (elephantScale.x > 0.0f) ? 1.0f : -1.0f;

                script->StartPosition = elephantPos;

                script->EndPosition = elephantPos + Vec3(direction * script->DistanceBetweenPositions, 0.0f, 0.0f);
            }
        }
    }

    void Elephant::OnDestroy()
    {
        // TODO: Cleanup
    }

    void Elephant::OnCollisionBegin(const CollisionContact& contact)
    {
        // TODO: On contact begin
    }

    void Elephant::OnCollisionEnd(const CollisionContact& contact)
    {
        // TODO: On contact end
    }

    void Elephant::OnCollisionHit(const CollisionContact& contact)
    {
        // TODO: On hit(high speed)
    }

    void Elephant::Activate()
    {
        if (m_ElephantPushAnim)
        {
            AnimationAPI::PlayAnimation(GetEntity(), m_ElephantPushAnim);
            AnimTriggered = true;
            PassedTime = 0.0f;
            IsActivated = true; 

            std::cout << "[Elephant] Activated!" << std::endl;
        }
    }

    void Elephant::Deactivate()
    {
        if (m_ElephantPullAnim)
        {
            AnimationAPI::PlayAnimation(GetEntity(), m_ElephantPullAnim);
            AnimTriggered = false;
            IsActivated = false;

            std::cout << "[Elephant] Deactivated!" << std::endl;
        }
    }

    void Elephant::Toggle()
    {
        if (!IsActivated)
        {
            Activate();
        }
        else
        {
            Deactivate();
        }
    }
}