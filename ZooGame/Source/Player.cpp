#include "Player.h"
#include <iostream>

#include "Elephant.h"
#include "ElephantTarget.h"
#include "Mask.h"
#include "MonkeyArm.h"
#include "RabbitPlatform.h"
#include "ScriptAPI/GameplayAPI.h"
#include "ScriptAPI/MathAPI.h"
#include "ScriptAPI/Physics2DAPI.h"
#include "ScriptAPI/AnimationAPI.h"
#include "ScriptAPI/DebugAPI.h"

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

        if (m_IsWalkingToMonkey)
        {
            UpdateMonkeyWalk(ts);
        }

        Vec3 playerPos = GameplayAPI::GetPosition(GetEntity());

        switch (m_Type)
        {
        case MaskType::Elephant:
            DebugAPI::DrawDebugCircle(playerPos, m_MinElephantDistance, sf::Color::Blue, 0.0f);
            break;

        case MaskType::Rabbit:
            DebugAPI::DrawDebugCircle(playerPos, m_MinRabbitJumpDistance, sf::Color::Blue, 0.0f);
            break;

        case MaskType::Monkey:
            DebugAPI::DrawDebugCircle(playerPos, m_MinMonkeyJumpDistance, sf::Color::Blue, 0.0f);
            break;

        case MaskType::None:
        default:
            break;
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
        if (m_IsJumping || m_IsWalkingToMonkey)
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

    void Player::UpdateMonkeyWalk(TimeStep ts)
    {
        m_MonkeyWalkProgress += ts / m_MonkeyWalkDuration;

        if (m_MonkeyWalkProgress >= 1.0f)
        {
            m_MonkeyWalkProgress = 1.0f;
            GameplayAPI::SetPosition(GetEntity(), m_MonkeyWalkTarget);
            m_IsWalkingToMonkey = false;

            ActivateMonkeyArmInDirection(m_MonkeySourcePlatform, m_WalkDirection, true);
            ActivateMonkeyArmInDirection(m_MonkeyTargetPlatform, GetOppositeDirection(m_WalkDirection), true);

            m_CurrentPlatform = m_MonkeyTargetPlatform;

            m_MonkeySourcePlatform = Entity();
            m_MonkeyTargetPlatform = Entity();

            m_IsOnRabbitPlatform = true;

            std::cout << "[Player] Monkey walk complete!" << std::endl;
            return;
        }

        float t = m_MonkeyWalkProgress;
        Vec3 currentPos = MathAPI::Lerp(m_MonkeyWalkStart, m_MonkeyWalkTarget, t);

        GameplayAPI::SetPosition(GetEntity(), currentPos);

        Vec3 direction = m_MonkeyWalkTarget - m_MonkeyWalkStart;
        Vec3 normalizedDir = MathAPI::Normalize(direction);
        UpdateMovementAnimation(normalizedDir);
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
        Vec3 playerPos = GameplayAPI::GetPosition(GetEntity());

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
        auto targets = GameplayAPI::FindAllEntitiesWithTag("ElephantRoot");

        Entity closest;
        float minDist = m_MinElephantDistance;

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
            auto children = GameplayAPI::GetChildren(closest);

            Entity elephantEntity = children[0];
            Entity elephantTargetEntity = children[1];

            if (elephantTargetEntity.IsValid())
            {
                auto elephantTarget = GameplayAPI::GetScript<ElephantTarget>(elephantTargetEntity);
                if (elephantTarget)
                {
                    elephantTarget->Toggle();
                    std::cout << "[Player] Toggled ElephantTarget!" << std::endl;
                }
            }

            if (elephantEntity.IsValid())
            {
                auto elephant = GameplayAPI::GetScript<Elephant>(elephantEntity);
                if (elephant)
                {
                    elephant->Toggle(); 
                }
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

        Entity targetPlatform = FindClosestPlatformInDirection(50.0f);

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
        std::cout << "[Player] Using Monkey Ability..." << std::endl;

        if (!m_IsOnRabbitPlatform || !m_CurrentPlatform.IsValid())
        {
            std::cout << "[Player] Must be on a RabbitPlatform!" << std::endl;
            return;
        }

        if (m_IsWalkingToMonkey)
        {
            std::cout << "[Player] Already walking!" << std::endl;
            return;
        }

        Entity targetPlatform = FindClosestPlatformInDirection(50.0f);

        if (!targetPlatform.IsValid())
        {
            std::cout << "[Player] No platform in that direction!" << std::endl;
            return;
        }

        ActivateMonkeyArmInDirection(m_CurrentPlatform, m_WalkDirection, false);
        WalkDirection oppositeDir = GetOppositeDirection(m_WalkDirection);
        ActivateMonkeyArmInDirection(targetPlatform, oppositeDir, false);

        auto targetScript = GameplayAPI::GetScript<RabbitPlatform>(targetPlatform);
        Vec3 targetPos = targetScript ? targetScript->GetLandingPosition() : GameplayAPI::GetPosition(targetPlatform);

        Vec3 playerPos = GameplayAPI::GetPosition(GetEntity());
        m_MonkeyWalkStart = playerPos;
        m_MonkeyWalkTarget = targetPos;  
        m_MonkeyWalkProgress = 0.0f;
        m_IsWalkingToMonkey = true;

        m_MonkeySourcePlatform = m_CurrentPlatform;
        m_MonkeyTargetPlatform = targetPlatform;

        Physics2DAPI::SetLinearVelocity(GetEntity(), Vec2(0.0f, 0.0f));

        std::cout << "[Player] Walking across monkey bridge!" << std::endl;
    }

    WalkDirection Player::GetOppositeDirection(WalkDirection dir)
    {
        switch (dir)
        {
        case WalkDirection::Right: return WalkDirection::Left;
        case WalkDirection::Left:  return WalkDirection::Right;
        case WalkDirection::Up:    return WalkDirection::Down;
        case WalkDirection::Down:  return WalkDirection::Up;
        default: return dir;
        }
    }

    void Player::ActivateMonkeyArmInDirection(Entity platform, WalkDirection direction, bool deactivate)
    {
        if (!platform.IsValid())
            return;

        auto platformScript = GameplayAPI::GetScript<RabbitPlatform>(platform);
        if (!platformScript)
            return;

        Entity monkeyArm;

        switch (direction)
        {
        case WalkDirection::Right:
            monkeyArm = platformScript->MonkeyArmRight;
            break;
        case WalkDirection::Left:
            monkeyArm = platformScript->MonkeyArmLeft;
            break;
        case WalkDirection::Up:
            monkeyArm = platformScript->MonkeyArmUp;
            break;
        case WalkDirection::Down:
            monkeyArm = platformScript->MonkeyArmDown;
            break;
        }

        if (!monkeyArm.IsValid())
        {
            std::cout << "[Player] No monkey arm in that direction!" << std::endl;
            return;
        }

        auto monkeyScript = GameplayAPI::GetScript<MonkeyArm>(monkeyArm);
        if (!monkeyScript)
            return;

        if (deactivate)
        {
            monkeyScript->Deactivate();
            std::cout << "[Player] Deactivated monkey arm" << std::endl;
        }
        else
        {
            monkeyScript->Activate();
            std::cout << "[Player] Activated monkey arm" << std::endl;
        }
    }

    Entity Player::FindClosestPlatformInDirection(float greaterThan)
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
                isInDirection = (direction.x > 50.0f && abs(direction.y) < 200.0f);  
                break;

            case WalkDirection::Left:
                isInDirection = (direction.x < -50.0f && abs(direction.y) < 200.0f);
                break;

            case WalkDirection::Up:
                isInDirection = (direction.y < -50.0f && abs(direction.x) < 200.0f);
                break;

            case WalkDirection::Down:
                isInDirection = (direction.y > 50.0f && abs(direction.x) < 200.0f);
                break;
            }

            if (!isInDirection)
                continue;

            float distance = GameplayAPI::Distance(playerPos, platformPos);

            if (distance > greaterThan && distance < minDistance)
            {
                minDistance = distance;
                closestPlatform = platform;
            }
        }

        if (closestPlatform.IsValid())
        {
            std::cout << "[Player] Found platform at distance: " << minDistance << std::endl;
        }
        else
        {
            std::cout << "[Player] No platform found in direction!" << std::endl;
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