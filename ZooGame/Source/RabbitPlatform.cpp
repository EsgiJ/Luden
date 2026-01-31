#include "RabbitPlatform.h"

#include <iostream>
#include <ScriptAPI/GameplayAPI.h>
#include <ScriptAPI/MathAPI.h>

namespace Luden
{
    void RabbitPlatform::OnCreate()
    {
        // TODO: Initialize
    }

    void RabbitPlatform::OnUpdate(TimeStep ts)
    {
        if (m_IsTeleporting && m_TeleportingPlayer.IsValid())
        {
            Vec3 playerPos = GameplayAPI::GetPosition(m_TeleportingPlayer);

            Vec3 newPos = MathAPI::Lerp(playerPos, m_TargetPosition, TeleportSpeed * ts);
            GameplayAPI::SetPosition(m_TeleportingPlayer, newPos);

            float dist = GameplayAPI::Distance(newPos, m_TargetPosition);
            if (dist < 5.0f)
            {
                GameplayAPI::SetPosition(m_TeleportingPlayer, m_TargetPosition);
                m_IsTeleporting = false;

                std::cout << "[RabbitPlatform] Teleport complete!" << std::endl;
            }
        }
    }

    void RabbitPlatform::OnDestroy()
    {
        // TODO: Cleanup
    }

    void RabbitPlatform::OnCollisionBegin(const CollisionContact& contact)
    {
        // TODO: On contact begin
    }

    void RabbitPlatform::OnCollisionEnd(const CollisionContact& contact)
    {
        // TODO: On contact end
    }

    void RabbitPlatform::OnCollisionHit(const CollisionContact& contact)
    {
        // TODO: On hit(high speed)
    }

    void RabbitPlatform::TeleportPlayerHere(Entity player)
    {
        if (!player.IsValid())
            return;

        Vec3 platformPos = GameplayAPI::GetPosition(GetEntity());
        m_TargetPosition = platformPos + TeleportOffset;

        m_TeleportingPlayer = player;
        m_IsTeleporting = true;

        std::cout << "[RabbitPlatform] Teleporting player to: " << m_TargetPosition.x << ", " << m_TargetPosition.y << std::endl;

        //TODO: Play teleport effect&sound
    }
}
