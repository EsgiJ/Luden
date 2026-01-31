#include "Player.h"

#include <iostream>

#include "ElephantTarget.h"
#include "Mask.h"
#include "MonkeyBridge.h"
#include "RabbitPlatform.h"
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
        if (!m_MaskEntity.IsValid())
        {
            Vector<Entity> children = GameplayAPI::GetChildren(GetEntity());
            for (Entity child : children)
            {
                if (child.Tag() == "Mask")
                {
                    m_MaskEntity = child;
                    m_MaskScript = GameplayAPI::GetScript<Mask>(m_MaskEntity);
                    break;
                }
            }
        }

        if (m_MaskEntity.IsValid() && m_MaskScript)
        {
            if (m_MaskScript->m_Type != m_Type)
            {
                m_MaskScript->m_Type = m_Type;

                SetupMaskAbility();
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

        UpdateMovementAnimation(movement);

        Physics2DAPI::SetLinearVelocity(ownerEntity, movement * m_MoveSpeed);
    }

    void Player::UpdateMovementAnimation(const Vec3& movement)
    {
        if (movement.x == 0.0f && movement.y == 0.0f)
        {
            AnimationAPI::PlayAnimation(GetEntity(), m_IdleAnim);

            if (m_MaskEntity.IsValid() && m_MaskScript)
            {
                if (m_MaskScript->m_CurrentIdleAnim)
                    AnimationAPI::PlayAnimation(m_MaskEntity, m_MaskScript->m_CurrentIdleAnim);
                else
                    AnimationAPI::PlayAnimation(m_MaskEntity, m_MaskScript->m_EmptyAnim);
            }
        }
        else if (movement.x > 0.0f)
        {
            AnimationAPI::PlayAnimation(GetEntity(), m_SideAnim);

            Vec3 playerScale = GameplayAPI::GetScale(GetEntity());
            if (playerScale.x > 0.0f)
                playerScale.x *= -1;
            GameplayAPI::SetScale(GetEntity(), playerScale);

            if (m_MaskEntity.IsValid() && m_MaskScript)
            {
                if (m_MaskScript->m_CurrentSideAnim)
                {
                    AnimationAPI::PlayAnimation(m_MaskEntity, m_MaskScript->m_CurrentSideAnim);

                    Vec3 maskScale = GameplayAPI::GetScale(m_MaskEntity);
                    if (maskScale.x < 0.0f)
                        maskScale.x *= -1;
                    GameplayAPI::SetScale(m_MaskEntity, maskScale);
                }
                else
                {
                    AnimationAPI::PlayAnimation(m_MaskEntity, m_MaskScript->m_EmptyAnim);
                }
            }
        }
        else if (movement.x < 0.0f)
        {
            AnimationAPI::PlayAnimation(GetEntity(), m_SideAnim);

            Vec3 playerScale = GameplayAPI::GetScale(GetEntity());
            if (playerScale.x < 0.0f)
                playerScale.x *= -1;
            GameplayAPI::SetScale(GetEntity(), playerScale);

            if (m_MaskEntity.IsValid() && m_MaskScript)
            {
                if (m_MaskScript->m_CurrentSideAnim)
                {
                    AnimationAPI::PlayAnimation(m_MaskEntity, m_MaskScript->m_CurrentSideAnim);

                    Vec3 maskScale = GameplayAPI::GetScale(m_MaskEntity);
                    if (maskScale.x < 0.0f)
                        maskScale.x *= -1;
                    GameplayAPI::SetScale(m_MaskEntity, maskScale);
                }
                else
                {
                    AnimationAPI::PlayAnimation(m_MaskEntity, m_MaskScript->m_EmptyAnim);
                }
            }
        }
        else if (movement.y > 0.0f)
        {
            AnimationAPI::PlayAnimation(GetEntity(), m_FrontAnim);

            if (m_MaskEntity.IsValid() && m_MaskScript)
            {
                if (m_MaskScript->m_CurrentFrontAnim)
                    AnimationAPI::PlayAnimation(m_MaskEntity, m_MaskScript->m_CurrentFrontAnim);
                else
                    AnimationAPI::PlayAnimation(m_MaskEntity, m_MaskScript->m_EmptyAnim);
            }
        }
        else if (movement.y < 0.0f)
        {
            AnimationAPI::PlayAnimation(GetEntity(), m_BackAnim);

            if (m_MaskEntity.IsValid() && m_MaskScript)
            {
                AnimationAPI::PlayAnimation(m_MaskEntity, m_MaskScript->m_EmptyAnim);
            }
        }
    }

    void Player::OnChangeMask(const InputValue& value)
    {
        int i = (int)m_Type;
        i++;

        if (i > (int)MaskType::Monkey)
        {
            i = (int)MaskType::None;
        }

        m_Type = (MaskType)i;
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

        InputAction ChangeMaskAction("ChangeMask");
        context->AddMapping({
            ChangeMaskAction,
            sf::Keyboard::Key::R,
            ModifierConfig(),
            TriggerConfig()
            });

        InputAction UseAbilityAction("UseAbility");
        context->AddMapping({
            UseAbilityAction,
            sf::Keyboard::Key::E,
            ModifierConfig(),
            TriggerConfig()
            });

        InputManager::Instance().PushContext(context);

        auto& input = GetComponent<InputComponent>();
        input.priority = 100;
        input.consumeInput = true;

        input.BindAction(MoveAction, ETriggerEvent::Ongoing, this, &Player::OnMove);
        input.BindAction(ChangeMaskAction, ETriggerEvent::Started, this, &Player::OnChangeMask);
        input.BindAction(UseAbilityAction, ETriggerEvent::Started, this, &Player::OnUseAbility);  
    }

    void Player::OnUseAbility(const InputValue& value)
    {
        if (m_MaskEntity.IsValid() && m_MaskScript)
        {
            m_MaskScript->UseAbility();
        }
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

    void Player::SetupMaskAbility()
    {
        std::cout << "[Player]SetupAbility" << std::endl;
        if (!m_MaskScript)
            return;
        std::cout << "[Player]SetupAbility valid" << std::endl;

        m_MaskScript->OnAbilityUse = nullptr;

        switch (m_Type)
        {
        case MaskType::Elephant:
            std::cout << "[Player]Elephant ability assigned" << std::endl;
            m_MaskScript->OnAbilityUse = [this]() { UseElephantAbility(); };
            m_MaskScript->MaxCooldown = 3.0f;
            break;

        case MaskType::Rabbit:
            m_MaskScript->OnAbilityUse = [this]() { UseRabbitAbility(); };
            m_MaskScript->MaxCooldown = 5.0f;
            break;

        case MaskType::Monkey:
            m_MaskScript->OnAbilityUse = [this]() { UseMonkeyAbility(); };
            m_MaskScript->MaxCooldown = 2.0f;
            break;

        case MaskType::None:
        default:
            m_MaskScript->OnAbilityUse = nullptr;
            break;
        }
    }

    void Player::UseElephantAbility()
    {
        std::cout << "[Player] Using Elephant Ability - Finding nearest target..." << std::endl;

        Vec3 playerPos = GameplayAPI::GetPosition(GetEntity());
        auto targets = GameplayAPI::FindAllEntitiesWithTag("ElephantTarget");

        Entity closest;
        float minDist = 1000.0f;  

        for (auto target : targets)
        {
            float dist = GameplayAPI::Distance(playerPos, GameplayAPI::GetPosition(target));
            if (dist < minDist)
            {
                minDist = dist;
                closest = target;
            }
        }

        if (closest.IsValid())
        {
            auto elephantTarget = GameplayAPI::GetScript<ElephantTarget>(closest);
            if (elephantTarget)
            {
                elephantTarget->Activate();
                std::cout << "[Player] Activated ElephantTarget!" << std::endl;
            }
        }
        else
        {
            std::cout << "[Player] No ElephantTarget in range!" << std::endl;
        }
    }

    void Player::UseRabbitAbility()
    {
        std::cout << "[Player] Using Rabbit Ability - Finding nearest platform..." << std::endl;

        Vec3 playerPos = GameplayAPI::GetPosition(GetEntity());
        auto platforms = GameplayAPI::FindAllEntitiesWithTag("RabbitPlatform");

        Entity closest;
        float minDist = 500.0f;

        for (auto platform : platforms)
        {
            float dist = GameplayAPI::Distance(playerPos, GameplayAPI::GetPosition(platform));
            if (dist < minDist)
            {
                minDist = dist;
                closest = platform;
            }
        }

        if (closest.IsValid())
        {
            auto rabbitPlatform = GameplayAPI::GetScript<RabbitPlatform>(closest);
            if (rabbitPlatform)
            {
                rabbitPlatform->TeleportPlayerHere(GetEntity());
                std::cout << "[Player] Teleporting to platform!" << std::endl;
            }
        }
        else
        {
            std::cout << "[Player] No RabbitPlatform in range!" << std::endl;
        }
    }

    void Player::UseMonkeyAbility()
    {
        std::cout << "[Player] Using Monkey Ability - Finding nearest bridge..." << std::endl;

        Vec3 playerPos = GameplayAPI::GetPosition(GetEntity());
        auto bridges = GameplayAPI::FindAllEntitiesWithTag("MonkeyBridge");

        Entity closest;
        float minDist = 500.0f;

        for (auto bridge : bridges)
        {
            float dist = GameplayAPI::Distance(playerPos, GameplayAPI::GetPosition(bridge));
            if (dist < minDist)
            {
                minDist = dist;
                closest = bridge;
            }
        }

        if (closest.IsValid())
        {
            auto monkeyBridge = GameplayAPI::GetScript<MonkeyBridge>(closest);
            if (monkeyBridge)
            {
                if (!monkeyBridge->IsActivated)
                    monkeyBridge->Activate();
                else
                    monkeyBridge->Deactivate();

                std::cout << "[Player] Toggled MonkeyBridge!" << std::endl;
            }
        }
        else
        {
            std::cout << "[Player] No MonkeyBridge in range!" << std::endl;
        }
    }
}
