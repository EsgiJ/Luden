#include "CollectableKey.h"

#include <iostream>
#include <ScriptAPI/GameplayAPI.h>

#include "Player.h"

namespace Luden
{
    void CollectableKey::OnCreate()
    {
        // TODO: Initialize
    }

    void CollectableKey::OnUpdate(TimeStep ts)
    {
        // TODO: Update logic
    }

    void CollectableKey::OnDestroy()
    {
        // TODO: Cleanup
    }

    void CollectableKey::OnCollisionBegin(const CollisionContact& contact)
    {
        Entity other = contact.otherEntity;
        if (!other.IsValid() || other.Tag() != "Player")
            return;

        Player* player = GameplayAPI::GetScript<Player>(other);
        if (!player)
            return;
			
        player->CollectKey();
        std::cout << "[Key] Player collected the key!" << std::endl;
        GameplayAPI::DestroyEntity(GetEntity());
    }

    void CollectableKey::OnCollisionEnd(const CollisionContact& contact)
    {
        // TODO: On contact end
    }

    void CollectableKey::OnCollisionHit(const CollisionContact& contact)
    {
        // TODO: On hit(high speed)
    }

}
