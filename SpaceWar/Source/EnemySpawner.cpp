#include "EnemySpawner.h"

#include <ScriptAPI/MathAPI.h>

#include "ScriptAPI/GameplayAPI.h"

namespace Luden
{
    void EnemySpawner::OnCreate()
    {
        m_EnemyPrefab = GetResource<Prefab>("Enemy");
        m_SpawnTimer = m_SpawnInterval;
    }

    void EnemySpawner::OnUpdate(TimeStep ts)
    {
        m_SpawnTimer -= ts;

        Vector<Entity> enemies = GameplayAPI::FindAllEntitiesWithTag("Enemy");
        m_CurrentEnemyCount = static_cast<int>(enemies.size());

        if (m_SpawnTimer <= 0.0f && m_CurrentEnemyCount < m_MaxEnemies)
        {
            SpawnEnemy();
            m_SpawnTimer = m_SpawnInterval;
        }
    }

    void EnemySpawner::OnDestroy()
    {
        // TODO: Cleanup
    }

    void EnemySpawner::OnCollisionBegin(const CollisionContact& contact)
    {
        // TODO: On contact begin
    }

    void EnemySpawner::OnCollisionEnd(const CollisionContact& contact)
    {
        // TODO: On contact end
    }

    void EnemySpawner::OnCollisionHit(const CollisionContact& contact)
    {
        // TODO: On hit(high speed)
    }

    void EnemySpawner::SpawnEnemy()
    {
        Vec3 spawnPos = GetRandomOffscreenPosition();
        Entity enemy = GameplayAPI::SpawnPrefab(m_EnemyPrefab, spawnPos);

        if (enemy.IsValid())
        {
            enemy.SetTag("Enemy");
        }
    }

    Vec3 EnemySpawner::GetRandomOffscreenPosition()
    {
        Vec2 camPos = GameplayAPI::GetCameraPosition();

        float angle = MathAPI::RandomFloat(0.0f, 360.0f);
        float angleRad = angle * 3.14159265f / 180.0f;

        float x = camPos.x + std::cos(angleRad) * m_SpawnDistance;
        float y = camPos.y + std::sin(angleRad) * m_SpawnDistance;

        return Vec3(x, y, 0.0f);
    }
}
