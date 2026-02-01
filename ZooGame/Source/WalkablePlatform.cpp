#include "WalkablePlatform.h"
#include <iostream>
#include "ScriptAPI/GameplayAPI.h"
#include "Player.h"

namespace Luden
{
    void WalkablePlatform::OnCreate()
    {
        if (GetEntity().Has<SpriteRendererComponent>())
        {
            PlatformSize = GameplayAPI::GetEntitySize(GetEntity());
        }
    }

    void WalkablePlatform::OnUpdate(TimeStep ts)
    {
        /*bool isPlayerOn = IsPlayerOnPlatform();

        std::cout << "[WalkablePlatform] IsPlayerOnPlatform: " << isPlayerOn << std::endl;
        std::cout << "[WalkablePlatform] WasPlayerOnPlatform: " << m_PlayerWasOnPlatform << std::endl;

        if (isPlayerOn)
        {
            Entity player = GameplayAPI::FindEntityWithTag("Player");
            if (player.IsValid())
            {
                auto playerScript = GameplayAPI::GetScript<Player>(player);
                if (playerScript)
                {
                    playerScript->m_IsOnRabbitPlatform = false;

                    playerScript->m_CurrentPlatform = Entity();

                    std::cout << "[WalkablePlatform] Player can walk freely!" << std::endl;
                }
            }
        }

        m_PlayerWasOnPlatform = isPlayerOn;
		*/
    }

    void WalkablePlatform::OnDestroy()
    {
        // Cleanup
    }

    void WalkablePlatform::OnCollisionBegin(const CollisionContact& contact)
    {
    }

    void WalkablePlatform::OnCollisionEnd(const CollisionContact& contact)
    {
    }

    void WalkablePlatform::OnCollisionHit(const CollisionContact& contact)
    {
    }

    bool WalkablePlatform::IsPlayerOnPlatform()
    {
        Entity player = GameplayAPI::FindEntityWithTag("Player");
        if (!player.IsValid())
            return false;

        Vec2 platformSize = GameplayAPI::GetEntitySize(GetEntity());
        Vec3 platformPos = GameplayAPI::GetPosition(GetEntity());
        Vec3 playerPos = GameplayAPI::GetPosition(player);
        Vec2 playerSize = GameplayAPI::GetEntitySize(player);

    	return GameplayAPI::IsPointInRect(playerPos, platformPos, platformSize);
    }
}