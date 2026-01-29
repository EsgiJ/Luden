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
        PrefabRef m_BulletPrefab;
        PrefabRef m_HealthBarPrefab;
        PrefabRef m_CrosshairPrefab;

        Entity m_CrosshairEntity;

		Vec2 m_HealthBarOffset = {0.0f, -200.0f};
        HealthBar* m_PlayerHealthBar;
        Vec2 m_MousePosition {0.0f};

        float m_BulletSpeed = 10.0f;

        float m_MoveSpeed = 10.0f;
        CameraShakeParams damageCameraShake;
	private:
        void InitializeHealthBar();
        void SetupInput();
        void OnShoot(const InputValue& value);
        void OnMove(const InputValue& value);
        void TakeDamage(int damage);
        void Die();
    };
}
