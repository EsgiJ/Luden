#include "RabbitPlatform.h"
#include <iostream>

#include "CollectableMask.h"
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

        for (int i = 0; i < entities.size(); ++i)
        {
            if (i == 0)
            {
                MonkeyArmRight = entities[0];
            }
        	else if (i == 1)
            {
                MonkeyArmLeft = entities[1];
            }
        	else if (i == 2)
            {
                MonkeyArmUp = entities[2];
            }
        	else if (i == 3)
            {
                MonkeyArmDown = entities[3];
            }
        }

    }

    void RabbitPlatform::OnUpdate(TimeStep ts)
    {
        CheckIfMonkeyMaskGrabbed();
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

    bool RabbitPlatform::CheckIfMonkeyMaskGrabbed()
    {
        Entity maskEntity = GameplayAPI::FindEntityWithTag("CollectableMonkeyMask");

        if (!maskEntity.IsValid())
            return false;

        Vec3 maskPosition = GameplayAPI::GetPosition(maskEntity);
        Vec2 maskSize = GameplayAPI::GetEntitySize(maskEntity);

        Entity playerEntity = GameplayAPI::FindEntityWithTag("Player");

        if (playerEntity.IsValid())
        {
            Vec3 playerPosition = GameplayAPI::GetPosition(playerEntity);
            Vec2 playerSize = GameplayAPI::GetEntitySize(playerEntity);

            bool overlap = GameplayAPI::CheckAABBOverlap(maskPosition, maskSize,
                playerPosition, playerSize);
            if (overlap)
            {
                CollectableMask* mask = GameplayAPI::GetScript<CollectableMask>(maskEntity);
                Player* player = GameplayAPI::GetScript<Player>(playerEntity);

                if (!player || !mask)
                    return false;

                MaskType type = mask->Type;
                player->CollectMask(type);

                GameplayAPI::DestroyEntity(maskEntity);

                std::cout << "[RabbitPlatform] Player collected mask type: "
                    << (int)type << std::endl;
                return true;
            }
        }
        return false;
    }
}
