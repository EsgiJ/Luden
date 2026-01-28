#include "Player.h"

#include <iostream>

#include "HealthBar.h"
#include "ScriptAPI/GameplayAPI.h"
#include "ScriptAPI/DebugAPI.h"

namespace Luden
{
    void Player::OnCreate()
    {
        InitializeHealthBar();

        SetupInput();
    }

    void Player::OnUpdate(TimeStep ts)
    {
        Vec2 mousePosition = GameplayAPI::GetMousePosition();
        //std::cout << "MousePosition.X: " << mousePosition.x << " MousePosition.Y: " << mousePosition.y << std::endl;

        Entity ownerEntity = GetEntity();

        if (ownerEntity.IsValid())
        {
            DebugAPI::DrawDebugLine(GameplayAPI::GetPosition(ownerEntity), Vec3(mousePosition.x, mousePosition.y, 0.0f), sf::Color::Red, 0.0f);
            GameplayAPI::LookAtPosition(ownerEntity, { mousePosition.x, mousePosition.y, 0.0f });
        }
    }

    void Player::OnDestroy()
    {
        // TODO: Cleanup
    }

    void Player::OnCollisionBegin(const CollisionContact& contact)
    {
        // TODO: On contact begin
    }

    void Player::OnCollisionEnd(const CollisionContact& contact)
    {
        // TODO: On contact end
    }

    void Player::OnCollisionHit(const CollisionContact& contact)
    {
        // TODO: On hit(high speed)
    }

    void Player::SetupInput()
    {
        m_HealthBarPrefab = GetResource<Prefab>("HealthBar");

        Entity ownerEntity = GetEntity();

        if (ownerEntity.IsValid())
        {
            Vec3 ownerPosition = GameplayAPI::GetPosition(ownerEntity);

            Entity healthBarEntity = GameplayAPI::SpawnPrefabAsChild(m_HealthBarPrefab, ownerEntity, Vec3(0.0f));
            m_PlayerHealthBar = GameplayAPI::GetScript<HealthBar>(healthBarEntity);

            if (m_PlayerHealthBar)
            {
                m_PlayerHealthBar->m_Offset = m_HealthBarOffset;
            }
        }
    }

    void Player::InitializeHealthBar()
    {
        auto context = std::make_shared<InputContext>("Gameplay", 100);
        context->SetEnabled(true);

        
    }
}
