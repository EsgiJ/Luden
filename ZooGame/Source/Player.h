#pragma once
#include "Luden.h"
#include "Mask.h"

namespace Luden
{
    class HealthBar;

    enum class WalkDirection : uint8_t
    {
        Right,
        Left,
        Up,
        Down
    };

    class Player : public ScriptableEntity
    {
    public:
        virtual void OnCreate() override;
        virtual void OnUpdate(TimeStep ts) override;
        virtual void OnDestroy() override;
        virtual void OnCollisionBegin(const CollisionContact& contact) override;
        virtual void OnCollisionEnd(const CollisionContact& contact) override;
        virtual void OnCollisionHit(const CollisionContact& contact) override;

    public:
        Mask* m_MaskScript = nullptr;
        Entity m_MaskEntity;
        MaskType m_Type = MaskType::None;

        AnimationRef m_IdleAnim = nullptr;
        AnimationRef m_BackAnim = nullptr;
        AnimationRef m_FrontAnim = nullptr;
        AnimationRef m_SideAnim = nullptr;

        float m_MoveSpeed = 10.0f;
        WalkDirection m_WalkDirection = WalkDirection::Right;

        bool m_IsOnRabbitPlatform = false;
        Entity m_CurrentPlatform;

        bool m_IsJumping = false;

        CameraShakeParams damageCameraShake;

    private:
        void SetupInput();
        void OnMove(const InputValue& value);
        void OnChangeMask(const InputValue& value);
        void OnUseAbility(const InputValue& value);

        void UpdateMovementAnimation(const Vec3& movement);

        void SetupMaskAbility();
        void UseElephantAbility();
        void UseRabbitAbility();
        void UseMonkeyAbility();

        void UpdateRabbitJump(TimeStep ts);
        Entity FindClosestPlatformInDirection();

        void TakeDamage(int damage);
        void Die();

    private:
        Vec3 m_JumpStartPos;
        Vec3 m_JumpEndPos;
        float m_JumpProgress = 0.0f;
        float m_JumpDuration = 0.8f;
        float m_JumpHeight = 100.0f;
    };
}