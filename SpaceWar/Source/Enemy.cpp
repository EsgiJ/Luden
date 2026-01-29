#include "Enemy.h"

#include <iostream>

#include "HealthBar.h"
#include "ScriptAPI/GameplayAPI.h"
#include "ScriptAPI/MathAPI.h"
#include "ScriptAPI/Physics2DAPI.h"

namespace Luden
{
    void Enemy::OnCreate()
    {
        m_BulletPrefab = GetResource<Prefab>("Bullet");
        m_HealthBarPrefab = GetResource<Prefab>("HealthBar");

        InitializeHealthBar();

        m_PlayerEntity = GameplayAPI::FindEntityWithTag("Player");
    }

    void Enemy::OnUpdate(TimeStep ts)
    {
        if (m_IsDead)
            return;

        Entity ownerEntity = GetEntity();
        if (!ownerEntity.IsValid())
            return;

        if (m_ShootTimer > 0.0f)
            m_ShootTimer -= ts;

        UpdateAI(ts);
    }

    void Enemy::OnDestroy()
    {
        // TODO: Cleanup
    }

    void Enemy::OnCollisionBegin(const CollisionContact& contact)
    {
        Entity other = contact.otherEntity;

        std::cout << "[Enemy]Bullet hit" << std::endl;

        if (other.Tag() == "Bullet")
        {
            TakeDamage(m_Damage);
            GameplayAPI::DestroyEntity(other);
        }
    }


    void Enemy::OnCollisionEnd(const CollisionContact& contact)
    {
        // TODO: On contact end
    }

    void Enemy::OnCollisionHit(const CollisionContact& contact)
    {
        // TODO: On hit(high speed)
    }

    void Enemy::InitializeHealthBar()
    {
        Entity ownerEntity = GetEntity();
        if (!ownerEntity.IsValid())
            return;

        Entity healthBarEntity = GameplayAPI::SpawnPrefabAsChild(m_HealthBarPrefab, ownerEntity, Vec3(0.0f, 0.0f, 0.0f));

        m_EnemyHealthBar = GameplayAPI::GetScript<HealthBar>(healthBarEntity);
        if (m_EnemyHealthBar)
        {
            m_EnemyHealthBar->m_Offset = m_HealthBarOffset;
        }
    }

    void Enemy::UpdateAI(TimeStep ts)
    {
        if (!m_PlayerEntity.IsValid())
        {
            m_PlayerEntity = GameplayAPI::FindEntityWithTag("Player");
            return;
        }

        Entity ownerEntity = GetEntity();
        Vec3 enemyPos = GameplayAPI::GetPosition(ownerEntity);
        Vec3 playerPos = GameplayAPI::GetPosition(m_PlayerEntity);

        float distanceToPlayer = GameplayAPI::Distance(enemyPos, playerPos);

        GameplayAPI::LookAtPosition(ownerEntity, playerPos);

        if (distanceToPlayer < m_ChaseRange && distanceToPlayer > m_StopDistance)
        {
            ChasePlayer(ts);
        }

        if (distanceToPlayer < m_ShootRange && m_ShootTimer <= 0.0f)
        {
            ShootAtPlayer();
            m_ShootTimer = m_ShootCooldown;
        }
    }

    void Enemy::ChasePlayer(TimeStep ts)
    {
        Entity ownerEntity = GetEntity();
        Vec3 enemyPos = GameplayAPI::GetPosition(ownerEntity);
        Vec3 playerPos = GameplayAPI::GetPosition(m_PlayerEntity);

        Vec3 direction = GameplayAPI::Direction(enemyPos, playerPos);
        Vec3 movement = direction * (m_MoveSpeed * ts);

        GameplayAPI::Move(ownerEntity, movement);
    }

    void Enemy::ShootAtPlayer()
    {
        Entity ownerEntity = GetEntity();
        Vec3 enemyPos = GameplayAPI::GetPosition(ownerEntity);
        Vec3 playerPos = GameplayAPI::GetPosition(m_PlayerEntity);

        Vec3 shootDirection = GameplayAPI::Direction(enemyPos, playerPos);
        shootDirection = MathAPI::Normalize(shootDirection);

        Vec2 enemySize = GameplayAPI::GetEntitySize(ownerEntity);
        Vec3 spawnPosition = enemyPos + Vec3(
            shootDirection.x * enemySize.x * 1.2f,
            shootDirection.y * enemySize.y * 1.2f,
            0.0f
        );

        Entity bulletEntity = GameplayAPI::SpawnPrefab(m_BulletPrefab, spawnPosition);

        if (bulletEntity.IsValid())
        {
            bulletEntity.SetTag("EnemyBullet");

            GameplayAPI::LookAtPosition(bulletEntity, playerPos);

            Physics2DAPI::SetLinearVelocity(bulletEntity, shootDirection * m_BulletSpeed);
        }
    }

    void Enemy::TakeDamage(int damage)
    {
        Entity ownerEntity = GetEntity();
        if (!ownerEntity.Has<HealthComponent>())
            return;

        auto& health = ownerEntity.Get<HealthComponent>();
        health.current -= damage;

        if (health.current <= 0)
        {
            health.current = 0;
            Die();
        }
    }

    void Enemy::Die()
    {
        m_IsDead = true;

        GameplayAPI::DestroyEntity(GetEntity());
    }
}
