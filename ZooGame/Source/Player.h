#pragma once
#include "Luden.h"
#include "Mask.h"

namespace Luden
{
    class HealthBar;

    enum class WalkDirection : uint8_t
    {
        Right = 3,
        Left = 2,
        Up = 0,
        Down = 1
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

        bool m_IsWalkingToMonkey = false;

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

        void UpdateMonkeyWalk(TimeStep ts);

        void TakeDamage(int damage);
        void Die();

    private:
        Vec3 m_JumpStartPos;
        Vec3 m_JumpEndPos;
        float m_JumpProgress = 0.0f;
        float m_JumpDuration = 0.8f;
        float m_JumpHeight = 100.0f;

        Vec3 m_MonkeyWalkTarget;
        Vec3 m_MonkeyWalkStart;
        float m_MonkeyWalkProgress = 0.0f;
        float m_MonkeyWalkDuration = 1.5f;
        Entity m_ActiveMonkeyArm;

        float m_MinRabbitJumpDistance = 500.0f;
        float m_MinElephantDistance = 500.0f;
        float m_MinMonkeyJumpDistance = 1000.0f;

    };
}