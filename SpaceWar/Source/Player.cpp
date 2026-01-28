#include "Player.h"

#include <iostream>

#include "HealthBar.h"
#include "ScriptAPI/GameplayAPI.h"
#include "ScriptAPI/MathAPI.h"
#include "ScriptAPI/DebugAPI.h"
#include "ScriptAPI/Physics2DAPI.h"

namespace Luden
{
    void Player::OnCreate()
    {
        m_BulletRef = GetResource<Prefab>("Bullet");

        InitializeHealthBar();

        SetupInput();
    }

    void Player::OnUpdate(TimeStep ts)
    {
        m_MousePosition = GameplayAPI::GetMousePosition();

        Entity ownerEntity = GetEntity();

        if (ownerEntity.IsValid())
        {
            DebugAPI::DrawDebugLine(GameplayAPI::GetPosition(ownerEntity), Vec3(m_MousePosition.x, m_MousePosition.y, 0.0f), sf::Color::Red, 0.0f);
            GameplayAPI::LookAtPosition(ownerEntity, { m_MousePosition.x, m_MousePosition.y, 0.0f });
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

    void Player::InitializeHealthBar()
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

    void Player::OnShoot(const InputValue& value)
    {
        Entity ownerEntity = GetEntity();

        std::cout << "OnShoot" << std::endl;

        if (ownerEntity.IsValid())
        {
            Vec3 ownerPosition = GameplayAPI::GetPosition(ownerEntity);
            Vec3 shootDirection = GameplayAPI::Direction(ownerPosition, { m_MousePosition.x, m_MousePosition.y, 0.0f });
            shootDirection = MathAPI::Normalize(shootDirection);

            Vec2 ownerSize = GameplayAPI::GetEntitySize(ownerEntity);

            Vec3 spawnPosition = ownerPosition + Vec3((shootDirection.x * ownerSize.x * 0.6f), (shootDirection.y * ownerSize.y * 0.6f), 0.0f);
            Entity bulletEntity = GameplayAPI::SpawnPrefab(m_BulletRef, spawnPosition);

            GameplayAPI::LookAtPosition(bulletEntity, { m_MousePosition.x, m_MousePosition.y, 0.0f });
            Physics2DAPI::SetLinearVelocity(bulletEntity, shootDirection * m_BulletSpeed);
        }
    }

    void Player::SetupInput()
    {
        auto context = std::make_shared<InputContext>("Gameplay", 100);
        context->SetEnabled(true);

        InputAction shootAction("Shoot");

        context->AddMapping({
			shootAction,
            sf::Mouse::Button::Left,
            ModifierConfig(),
            TriggerConfig(ETriggerType::Pressed)
        });

        InputManager::Instance().PushContext(context);

        auto& input = GetComponent<InputComponent>();
        input.priority = 100;
        input.consumeInput = true;

        input.BindAction(shootAction, ETriggerEvent::Started, this, &Player::OnShoot);
    }
}
