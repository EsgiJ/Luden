#include "CollectableMask.h"

#include <iostream>

#include "Player.h"
#include "ScriptAPI/GameplayAPI.h"

namespace Luden
{
    void CollectableMask::OnCreate()
    {
        ElephantCollectable = GetResource<Sprite>("ElephantCollectableMask");
        MonkeyCollectable = GetResource<Sprite>("MonkeyCollectableMask");
        RabbitCollectable = GetResource<Sprite>("RabbitCollectableMask");

        Entity ownerEntity = GetEntity();
        if (HasComponent<SpriteRendererComponent>())
        {
            auto spriteRenderer = GetComponent<SpriteRendererComponent>();
            if (ownerEntity.Tag() == "CollectableElephantMask")
            {
	            if (ElephantCollectable)
                    spriteRenderer.spriteHandle = ElephantCollectable->Handle;

                Type = MaskType::Elephant;
            }
            else if (ownerEntity.Tag() == "CollectableRabbitMask")
            {
                if (RabbitCollectable)
                    spriteRenderer.spriteHandle = RabbitCollectable->Handle;
                Type = MaskType::Rabbit;
            }
            else if (ownerEntity.Tag() == "CollectableMonkeyMask")
            {
                if (MonkeyCollectable)
                    spriteRenderer.spriteHandle = MonkeyCollectable->Handle;
                Type = MaskType::Monkey;
            }
        }

    }

    void CollectableMask::OnUpdate(TimeStep ts)
    {
        /*
        Vec3 maskPosition = GameplayAPI::GetPosition(GetEntity());
        Vec2 maskSize = GameplayAPI::GetEntitySize(GetEntity());

        Entity playerEntity = GameplayAPI::FindEntityWithTag("Player");

        if (playerEntity.IsValid())
        {
            Vec3 playerPosition = GameplayAPI::GetPosition(playerEntity);
            Vec2 playerSize = GameplayAPI::GetEntitySize(playerEntity);

            bool overlap = GameplayAPI::CheckAABBOverlap(maskPosition, maskSize, playerPosition, playerSize);
			if (overlap)
			{
                Player* player = GameplayAPI::GetScript<Player>(playerEntity);
                if (!player)
                    return;

                player->CollectMask(Type);

                std::cout << "[CollectableMask] Player collected mask type: " << (int)Type << std::endl;

                GameplayAPI::DestroyEntity(GetEntity());
			}
        }*/
    }

    void CollectableMask::OnDestroy()
    {
    }

    void CollectableMask::OnCollisionBegin(const CollisionContact& contact)
    {
        Entity other = contact.otherEntity;

        if (!other.IsValid() || other.Tag() != "Player")
            return;

        Player* player = GameplayAPI::GetScript<Player>(other);
        if (!player)
            return;

        player->CollectMask(Type);

        std::cout << "[CollectableMask] Player collected mask type: " << (int)Type << std::endl;

        GameplayAPI::DestroyEntity(GetEntity());
    }

    void CollectableMask::OnCollisionEnd(const CollisionContact& contact)
    {
    }

    void CollectableMask::OnCollisionHit(const CollisionContact& contact)
    {
    }
}