#include "RabbitPlatform.h"
#include <iostream>
#include "ScriptAPI/GameplayAPI.h"
#include "Player.h"
#include "MonkeyArm.h"

namespace Luden
{
    void RabbitPlatform::OnCreate()
    {
        if (GetEntity().Has<SpriteRendererComponent>())
        {
            PlatformSize = GameplayAPI::GetEntitySize(GetEntity());
        }

        auto entities = GameplayAPI::GetChildren(GetEntity());

        MonkeyArmRight = entities[0];
        MonkeyArmLeft = entities[1];
        MonkeyArmUp = entities[2];
        MonkeyArmDown = entities[3];

    }

    void RabbitPlatform::OnUpdate(TimeStep ts)
    {
        bool isPlayerOn = IsPlayerOnPlatform();

        if (isPlayerOn && !m_PlayerWasOnPlatform)
        {
            Entity player = GameplayAPI::FindEntityWithTag("Player");
            if (player.IsValid())
            {
                auto playerScript = GameplayAPI::GetScript<Player>(player);
                if (playerScript)
                {
                    playerScript->m_IsOnRabbitPlatform = true;
                    playerScript->m_CurrentPlatform = GetEntity();

                    std::cout << "[RabbitPlatform] Player entered - movement locked!" << std::endl;
                }
            }
        }
        else if (!isPlayerOn && m_PlayerWasOnPlatform)
        {
            Entity player = GameplayAPI::FindEntityWithTag("Player");
            if (player.IsValid())
            {
                auto playerScript = GameplayAPI::GetScript<Player>(player);
                if (playerScript && playerScript->m_CurrentPlatform == GetEntity())
                {
                    playerScript->m_IsOnRabbitPlatform = false;
                    playerScript->m_CurrentPlatform = Entity();

                    std::cout << "[RabbitPlatform] Player left - movement unlocked!" << std::endl;
                }
            }
        }

        m_PlayerWasOnPlatform = isPlayerOn;
    }

    void RabbitPlatform::OnDestroy()
    {
        // Cleanup
    }

    void RabbitPlatform::OnCollisionBegin(const CollisionContact& contact)
    {
    }

    void RabbitPlatform::OnCollisionEnd(const CollisionContact& contact)
    {
    }

    void RabbitPlatform::OnCollisionHit(const CollisionContact& contact)
    {
    }

    bool RabbitPlatform::IsPlayerOnPlatform()
    {
        Entity player = GameplayAPI::FindEntityWithTag("Player");
        if (!player.IsValid())
            return false;

        Vec3 platformPos = GameplayAPI::GetWorldPosition(GetEntity());
        Vec3 playerPos = GameplayAPI::GetWorldPosition(player);
        Vec2 playerSize = GameplayAPI::GetEntitySize(player);

        return GameplayAPI::CheckAABBOverlap(
            platformPos, PlatformSize,
            playerPos, playerSize
        );
    }

    Vec3 RabbitPlatform::GetLandingPosition()
    {
        Vec3 platformPos = GameplayAPI::GetPosition(GetEntity());
        platformPos += LandingOffset;
        platformPos.z = 200.0f;

        return platformPos + LandingOffset;
    }

    Entity RabbitPlatform::GetMonkeyInDirection(int direction)
    {
        switch (direction)
        {
        case 0: 
            return MonkeyArmUp;
        case 1: 
            return MonkeyArmDown;
        case 2: 
            return MonkeyArmLeft;
        case 3: 
            return MonkeyArmRight;
        default:
            return Entity();
        }
    }
}