#pragma once
#include "Luden.h"

namespace Luden
{
	class HealthBar;

	class Enemy : public ScriptableEntity
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

        float m_ShootRange = 500.0f;
        float m_ShootCooldown = 1.0f;
        float m_BulletSpeed = 800.0f;
        int m_Damage = 1;

        float m_MoveSpeed = 200.0f;
        float m_ChaseRange = 600.0f;
        float m_StopDistance = 400.0f;

        Vec2 m_HealthBarOffset = { 0.0f, -80.0f };

    private:
        void InitializeHealthBar();
        void UpdateAI(TimeStep ts);
        void ChasePlayer(TimeStep ts);
        void ShootAtPlayer();
        void TakeDamage(int damage);
        void Die();

    private:
        HealthBar* m_EnemyHealthBar = nullptr;
        Entity m_PlayerEntity;
        float m_ShootTimer = 0.0f;
        bool m_IsDead = false;
    };
}
