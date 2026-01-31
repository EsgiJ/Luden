#include "Room.h"

#include <iostream>
#include <ScriptAPI/GameplayAPI.h>

namespace Luden
{
    void Room::OnCreate()
    {
    }

    void Room::OnUpdate(TimeStep ts)
    {
        Entity player = GameplayAPI::FindEntityWithTag("Player");
        Entity owner = GetEntity();
        if (player.IsValid() && owner.IsValid())
        {
            Vec3 playerPos = GameplayAPI::GetPosition(player);
            Vec2 playerSize = GameplayAPI::GetEntitySize(player);

            Vec3 ownerPos = GameplayAPI::GetPosition(owner);
            Vec2 ownerSize = GameplayAPI::GetEntitySize(owner);

            m_Overlap = GameplayAPI::CheckAABBOverlap({ playerPos.x, playerPos.y, playerPos.z }, playerSize, { ownerPos.x, ownerPos.y, ownerPos.z }, ownerSize);
        }

        if (m_Overlap)
        {
            std::cout << "Overlap" << std::endl;

            Entity cameraEntity = GameplayAPI::FindEntityWithTag("Camera");

            if (cameraEntity.IsValid())
            {
                Vec3 ownerPos = GameplayAPI::GetPosition(owner);
                GameplayAPI::SetPosition(cameraEntity, ownerPos);
            }
        }
    }

    void Room::OnDestroy()
    {
        // TODO: Cleanup
    }

    void Room::OnCollisionBegin(const CollisionContact& contact)
    {
        // TODO: On contact begin
    }

    void Room::OnCollisionEnd(const CollisionContact& contact)
    {
        // TODO: On contact end
    }

    void Room::OnCollisionHit(const CollisionContact& contact)
    {
        // TODO: On hit(high speed)
    }

}
