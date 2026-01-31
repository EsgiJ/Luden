#include "Player.h"

#include <iostream>

#include "Mask.h"
#include "ScriptAPI/GameplayAPI.h"
#include "ScriptAPI/MathAPI.h"
#include "ScriptAPI/Physics2DAPI.h"
#include "ScriptAPI/AnimationAPI.h"

namespace Luden
{
    void Player::OnCreate()
    {
        m_IdleAnim = GetResource<Animation>("IdleAnim");
        m_SideAnim = GetResource<Animation>("SideAnim");
        m_BackAnim = GetResource<Animation>("BackAnim");
        m_FrontAnim = GetResource<Animation>("FrontAnim");

        SetupInput();
    }

    void Player::OnUpdate(TimeStep ts)
    {
    	Vector<Entity> children = GameplayAPI::GetChildren(GetEntity());
	    for (Entity child : children)
	    {
		    if (child.Tag() == "Mask")
		    {
                std::cout << "Mask entity found" << std::endl;
                m_MaskEntity = child;
		    }
	    }

	    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::R))
	    {
            int  i = (int)m_Type;
            i++;
            m_Type = (MaskType)i;
	    }
	    if (m_MaskEntity.IsValid())
	    {
            auto maskScript = GameplayAPI::GetScript<Mask>(m_MaskEntity);

            if (maskScript != nullptr)
            {
                maskScript->m_Type = m_Type;
            }
	    }
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
        Vec2 moveValue = value.GetAxis2D();

        Entity ownerEntity = GetEntity();
        if (!ownerEntity.IsValid())
            return;

        Vec3 movement = Vec3(moveValue.x, moveValue.y, 0.0f);

        if (MathAPI::Length(movement) > 0.0f)
        {
            movement = MathAPI::Normalize(movement);

            if (movement.x != 0.0f)
                movement.y = 0.0f;
        }

        if (movement.x == 0.0f && movement.y == 0.0f)
        {
            AnimationAPI::PlayAnimation(GetEntity(), m_IdleAnim);
        }
        else if (movement.x > 0.0f )
        {
            AnimationAPI::PlayAnimation(GetEntity(), m_SideAnim);
            Vec3 scale = GameplayAPI::GetScale(GetEntity());

            if (scale.x > 0.0f)
                scale.x *= -1;

            GameplayAPI::SetScale(GetEntity(), scale);
        }
        else if (movement.x < 0.0f)
        {
            AnimationAPI::PlayAnimation(GetEntity(), m_SideAnim);
            Vec3 scale = GameplayAPI::GetScale(GetEntity());

            if (scale.x < 0.0f)
                scale.x *= -1;

            GameplayAPI::SetScale(GetEntity(), scale);
        }
        else if (movement.y > 0.0f)
        {
            AnimationAPI::PlayAnimation(GetEntity(), m_FrontAnim);
        }
        else if (movement.y < 0.0f)
        {
            AnimationAPI::PlayAnimation(GetEntity(), m_BackAnim);
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
