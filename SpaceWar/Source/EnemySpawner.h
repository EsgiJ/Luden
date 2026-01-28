#pragma once
#include "Luden.h"

namespace Luden
{
	class EnemySpawner : public ScriptableEntity
    {
    public:
        virtual void OnCreate() override;
        virtual void OnUpdate(TimeStep ts) override;
        virtual void OnDestroy() override;
        virtual void OnCollisionBegin(const CollisionContact& contact) override;
        virtual void OnCollisionEnd(const CollisionContact& contact) override;
        virtual void OnCollisionHit(const CollisionContact& contact) override;

    public:
        PrefabRef m_EnemyPrefab;

        float m_SpawnInterval = 3.0f;
        float m_SpawnDistance = 700.0f;  
        int m_MaxEnemies = 10;

    private:
        void SpawnEnemy();
        Vec3 GetRandomOffscreenPosition();

    private:
        float m_SpawnTimer = 0.0f;
        int m_CurrentEnemyCount = 0;
    };
}
