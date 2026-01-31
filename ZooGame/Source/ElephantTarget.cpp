#include "ElephantTarget.h"

#include <iostream>
#include <ScriptAPI/GameplayAPI.h>
#include <ScriptAPI/MathAPI.h>

namespace Luden
{
    void ElephantTarget::OnCreate()
    {
        StartPosition = GameplayAPI::GetPosition(GetEntity());

        if (EndPosition == Vec3(0.0f, 0.0f, 0.0f))
        {
            EndPosition = StartPosition + Vec3(DistanceBetweenPositions, 0.0f, 0.0f);
        }

        m_CurrentTarget = EndPosition;
    }

    void ElephantTarget::OnUpdate(TimeStep ts)
    {
        if (IsActivated)
        {
            TimeActive += ts;

            MoveTowardsEnd(ts);

            if (TimeActive >= ActiveDuration)
            {
                IsActivated = false;
                TimeActive = 0.0f;
                m_CurrentTarget = StartPosition;

                std::cout << "[ElephantTarget] Deactivating - returning to start" << std::endl;
            }
        }
        else
        {
            if (IsAtEnd)
            {
                MoveTowardsStart(ts);
            }
        }
    }

    void ElephantTarget::OnDestroy()
    {
        // TODO: Cleanup
    }

    void ElephantTarget::OnCollisionBegin(const CollisionContact& contact)
    {
        // TODO: On contact begin
    }

    void ElephantTarget::OnCollisionEnd(const CollisionContact& contact)
    {
        // TODO: On contact end
    }

    void ElephantTarget::OnCollisionHit(const CollisionContact& contact)
    {
        // TODO: On hit(high speed)
    }

    void ElephantTarget::Activate()
    {
        if (IsActivated)
        {
            std::cout << "[ElephantTarget] Already activated!" << std::endl;
            return;
        }

        IsActivated = true;
        TimeActive = 0.0f;
        m_CurrentTarget = EndPosition;

        std::cout << "[ElephantTarget] Activated! Moving to end position" << std::endl;

        //TODO: Play water spray animation&sound
    }

    void ElephantTarget::MoveTowardsEnd(TimeStep ts)
    {
        Vec3 currentPos = GameplayAPI::GetPosition(GetEntity());

        Vec3 newPos = MathAPI::Lerp(currentPos, EndPosition, MoveSpeed * ts);
        GameplayAPI::SetPosition(GetEntity(), newPos);

        float dist = GameplayAPI::Distance(newPos, EndPosition);
        if (dist < 1.0f)
        {
            GameplayAPI::SetPosition(GetEntity(), EndPosition);
            IsAtEnd = true;
        }
    }

    void ElephantTarget::MoveTowardsStart(TimeStep ts)
    {
        Vec3 currentPos = GameplayAPI::GetPosition(GetEntity());

        Vec3 newPos = MathAPI::Lerp(currentPos, StartPosition, MoveSpeed * ts);
        GameplayAPI::SetPosition(GetEntity(), newPos);

        float dist = GameplayAPI::Distance(newPos, StartPosition);
        if (dist < 1.0f)
        {
            GameplayAPI::SetPosition(GetEntity(), StartPosition);
            IsAtEnd = false;
        }
    }
}
