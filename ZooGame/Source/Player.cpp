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

        if (m_IsJumping)
        {
            UpdateRabbitJump(ts);
        }
    }

    void Player::OnDestroy()
    {
        // Cleanup
    }

    void Player::OnCollisionBegin(const CollisionContact& contact)
    {
        // Handle collisions
    }

    void Player::OnCollisionEnd(const CollisionContact& contact)
    {
        // Handle collision end
    }

    void Player::OnCollisionHit(const CollisionContact& contact)
    {
    }

    void Player::OnMove(const InputValue& value)
    {
        if (m_IsJumping)
            return;

        if (m_IsOnRabbitPlatform)
        {
            Physics2DAPI::SetLinearVelocity(GetEntity(), Vec2(0.0f, 0.0f));

            Vec2 moveValue = value.GetAxis2D();
            if (moveValue.x > 0.0f)
                m_WalkDirection = WalkDirection::Right;
            else if (moveValue.x < 0.0f)
                m_WalkDirection = WalkDirection::Left;
            else if (moveValue.y > 0.0f)
                m_WalkDirection = WalkDirection::Down;
            else if (moveValue.y < 0.0f)
                m_WalkDirection = WalkDirection::Up;

            AnimationAPI::PlayAnimation(GetEntity(), m_IdleAnim);
            if (m_MaskEntity.IsValid() && m_MaskScript && m_MaskScript->m_CurrentIdleAnim)
            {
                AnimationAPI::PlayAnimation(m_MaskEntity, m_MaskScript->m_CurrentIdleAnim);
            }

            return;
        }

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

        if (movement.x > 0.0f)
            m_WalkDirection = WalkDirection::Right;
        else if (movement.x < 0.0f)
            m_WalkDirection = WalkDirection::Left;
        else if (movement.y > 0.0f)
            m_WalkDirection = WalkDirection::Down;
        else if (movement.y < 0.0f)
            m_WalkDirection = WalkDirection::Up;

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

        std::cout << "[Player] Mask changed to: " << i << std::endl;
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

        // Use Ability
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

    void Player::SetupMaskAbility()
    {
        if (!m_MaskScript)
            return;

        m_MaskScript->OnAbilityUse = nullptr;

        switch (m_Type)
        {
        case MaskType::Elephant:
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

        std::cout << "[Player] Mask ability setup for type: " << (int)m_Type << std::endl;
    }

    void Player::UseElephantAbility()
    {
        std::cout << "[Player] Using Elephant Ability - Finding nearest target..." << std::endl;

        Vec3 playerPos = GameplayAPI::GetPosition(GetEntity());
        auto targets = GameplayAPI::FindAllEntitiesWithTag("ElephantTarget");

        Entity closest;
        float minDist = 500.0f;

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
        if (!m_IsOnRabbitPlatform)
        {
            std::cout << "[Player] Must be on a RabbitPlatform to jump!" << std::endl;
            return;
        }

        if (m_IsJumping)
        {
            std::cout << "[Player] Already jumping!" << std::endl;
            return;
        }

        Entity targetPlatform = FindClosestPlatformInDirection();

        if (!targetPlatform.IsValid())
        {
            std::cout << "[Player] No platform in that direction!" << std::endl;
            return;
        }

        auto targetScript = GameplayAPI::GetScript<RabbitPlatform>(targetPlatform);
        if (!targetScript)
        {
            std::cout << "[Player] Target platform has no script!" << std::endl;
            return;
        }

        m_JumpStartPos = GameplayAPI::GetPosition(GetEntity());
        m_JumpEndPos = targetScript->GetLandingPosition();
        m_JumpProgress = 0.0f;
        m_IsJumping = true;

        Physics2DAPI::SetLinearVelocity(GetEntity(), Vec2(0.0f, 0.0f));

        std::cout << "[Player] Jumping to platform!" << std::endl;
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

    Entity Player::FindClosestPlatformInDirection()
    {
        if (!m_CurrentPlatform.IsValid())
        {
            std::cout << "[Player] Not on a platform!" << std::endl;
            return Entity();
        }

        Vec3 playerPos = GameplayAPI::GetPosition(GetEntity());
        auto allPlatforms = GameplayAPI::FindAllEntitiesWithTag("RabbitPlatform");

        Entity closestPlatform;
        float minDistance = FLT_MAX;

        for (auto platform : allPlatforms)
        {
            if (platform == m_CurrentPlatform)
                continue;

            Vec3 platformPos = GameplayAPI::GetPosition(platform);
            Vec3 direction = platformPos - playerPos;

            bool isInDirection = false;

            switch (m_WalkDirection)
            {
            case WalkDirection::Right:
                isInDirection = (direction.x > 50.0f && abs(direction.y) < 100.0f);
                break;

            case WalkDirection::Left:
                isInDirection = (direction.x < -50.0f && abs(direction.y) < 100.0f);
                break;

            case WalkDirection::Up:
                isInDirection = (direction.y < -50.0f && abs(direction.x) < 100.0f);
                break;

            case WalkDirection::Down:
                isInDirection = (direction.y > 50.0f && abs(direction.x) < 100.0f);
                break;
            }

            if (!isInDirection)
                continue;

            float distance = GameplayAPI::Distance(playerPos, platformPos);
            if (distance < minDistance)
            {
                minDistance = distance;
                closestPlatform = platform;
            }
        }

        if (closestPlatform.IsValid())
        {
            std::cout << "[Player] Found platform in direction at distance: " << minDistance << std::endl;
        }
        else
        {
            std::cout << "[Player] No platform found in facing direction!" << std::endl;
        }

        return closestPlatform;
    }

    void Player::UpdateRabbitJump(TimeStep ts)
    {
        m_JumpProgress += ts / m_JumpDuration;

        if (m_JumpProgress >= 1.0f)
        {
            m_JumpProgress = 1.0f;
            GameplayAPI::SetPosition(GetEntity(), m_JumpEndPos);
            m_IsJumping = false;

            AnimationAPI::PlayAnimation(GetEntity(), m_IdleAnim);
            if (m_MaskEntity.IsValid() && m_MaskScript && m_MaskScript->m_CurrentIdleAnim)
            {
                AnimationAPI::PlayAnimation(m_MaskEntity, m_MaskScript->m_CurrentIdleAnim);
            }

            std::cout << "[Player] Landed!" << std::endl;
            return;
        }

        float t = m_JumpProgress;
        float smoothT = t * t * (3.0f - 2.0f * t);  

        Vec3 horizontalPos = MathAPI::Lerp(m_JumpStartPos, m_JumpEndPos, smoothT);

        float arcOffset = -4.0f * m_JumpHeight * t * (t - 1.0f);

        Vec3 finalPos = horizontalPos;
        finalPos.y += arcOffset;

        GameplayAPI::SetPosition(GetEntity(), finalPos);
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
        std::cout << "[Player] Player died!" << std::endl;
        GameplayAPI::DestroyEntity(GetEntity());
    }
}