#pragma once
#include "Luden.h"

namespace Luden
{
    class HealthBar;

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
        float m_MoveSpeed = 10.0f;
        CameraShakeParams damageCameraShake;
	private:
        void SetupInput();
        void OnMove(const InputValue& value);
        void TakeDamage(int damage);
        void Die();
    };
}
