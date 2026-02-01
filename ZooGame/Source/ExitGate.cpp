#include "ExitGate.h"

#include <iostream>
#include <ScriptAPI/GameplayAPI.h>

#include "Player.h"

namespace Luden
{
    void ExitGate::OnCreate()
    {
        // TODO: Initialize
    }

    void ExitGate::OnUpdate(TimeStep ts)
    {
        Entity playerEntity = GameplayAPI::FindEntityWithTag("Player");

        if (playerEntity.IsValid())
        {
            Vec3 gatePosition = GameplayAPI::GetPosition(GetEntity());
            Vec2 gateSize = GameplayAPI::GetEntitySize(GetEntity());

            Vec3 playerPosition = GameplayAPI::GetPosition(playerEntity);
            Vec2 playerSize = GameplayAPI::GetEntitySize(playerEntity);

            bool overlap = GameplayAPI::CheckAABBOverlap(gatePosition, gateSize, playerPosition, playerSize);
            if (overlap)
            {
                auto playerScript = GameplayAPI::GetScript<Player>(playerEntity);

                if (!playerScript || !playerScript->HasKey())
                {
                    std::cout << "[Gate] You need the key to exit!" << std::endl;
                    return;
                }
                std::cout << "[Gate] Opening gate and loading end scene" << std::endl;
                GameplayAPI::ChangeScene("EndScene");
            }
        }
    }

    void ExitGate::OnDestroy()
    {
        
    }

    void ExitGate::OnCollisionBegin(const CollisionContact& contact)
    {
        Entity other = contact.otherEntity;
        if (!other.IsValid() || other.Tag() != "Player")
            return;

        Player* player = GameplayAPI::GetScript<Player>(other);
        if (!player || !player->HasKey())
        {
            std::cout << "[Gate] You need the key to exit!" << std::endl;
            return;
        }

        std::cout << "[Gate] Opening gate and loading end scene" << std::endl;
        GameplayAPI::ChangeScene("EndScene");
    }

    void ExitGate::OnCollisionEnd(const CollisionContact& contact)
    {
        // TODO: On contact end
    }

    void ExitGate::OnCollisionHit(const CollisionContact& contact)
    {
        // TODO: On hit(high speed)
    }

}
