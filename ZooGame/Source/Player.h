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
        Entity m_MaskEntity;
        MaskType m_Type = MaskType::None;

        AnimationRef m_IdleAnim = nullptr;
        AnimationRef m_BackAnim = nullptr;
        AnimationRef m_FrontAnim = nullptr;
        AnimationRef m_SideAnim = nullptr;

        float m_MoveSpeed = 10.0f;
        CameraShakeParams damageCameraShake;

        WalkDirection m_WalkDirection = WalkDirection::Right;
	private:
        void SetupInput();
        void OnMove(const InputValue& value);
        void TakeDamage(int damage);
        void Die();
    };
}
