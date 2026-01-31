#include "Player.h"

#include <iostream>

#include "ScriptAPI/GameplayAPI.h"
#include "ScriptAPI/MathAPI.h"
#include "ScriptAPI/Physics2DAPI.h"

namespace Luden
{
    void Player::OnCreate()
    {
        SetupInput();
    }

    void Player::OnUpdate(TimeStep ts)
    {

    }

    void Player::OnDestroy()
    {
        // TODO: Cleanup
    }

    void Player::OnCollisionBegin(const CollisionContact& contact)
    {

    }

    void Player::OnCollisionEnd(const CollisionContact& contact)
    {
        // TODO: On contact end
    }

    void Player::OnCollisionHit(const CollisionContact& contact)
    {
        // TODO: On hit(high speed)
    }

    void Player::OnMove(const InputValue& value)
    {
        std::cout << "[Player] OnMove triggered..." << std::endl;
        Vec2 moveValue = value.GetAxis2D();

        Entity ownerEntity = GetEntity();
        if (!ownerEntity.IsValid())
            return;

        Vec3 movement = Vec3(moveValue.x, moveValue.y, 0.0f);

        if (MathAPI::Length(movement) > 0.0f)
        {
            movement = MathAPI::Normalize(movement);
        }

        Physics2DAPI::SetLinearVelocity(ownerEntity, movement * m_MoveSpeed);
    }

    void Player::SetupInput()
    {
        auto context = std::make_shared<InputContext>("Gameplay", 100);
        context->SetEnabled(true);

        InputAction MoveAction("Move");

        context->AddAxis2DMapping({
            MoveAction,
            sf::Keyboard::Key::W,
        	sf::Keyboard::Key::S,
            sf::Keyboard::Key::A,
            sf::Keyboard::Key::D,
            ModifierConfig(),
            });

        InputManager::Instance().PushContext(context);

        auto& input = GetComponent<InputComponent>();
        input.priority = 100;
        input.consumeInput = true;

        input.BindAction(MoveAction, ETriggerEvent::Ongoing, this, &Player::OnMove);
    }

    void Player::TakeDamage(int damage)
    {
        Entity ownerEntity = GetEntity();
        if (!ownerEntity.Has<HealthComponent>())
            return;

        auto& health = ownerEntity.Get<HealthComponent>();
        health.current -= damage;

        GameplayAPI::ShakeCamera(damageCameraShake);

        if (health.current <= 0)
        {
            health.current = 0;
            Die();
        }
    }

    void Player::Die()
    {
        std::cout << "Player died!" << std::endl;

        GameplayAPI::DestroyEntity(GetEntity());
    }
}
