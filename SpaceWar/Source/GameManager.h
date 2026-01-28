#pragma once
#include "Luden.h"

namespace Luden
{
	class EnemySpawner;

	class GameManager : public ScriptableEntity
    {
    public:
        virtual void OnCreate() override;
        virtual void OnUpdate(TimeStep ts) override;
        virtual void OnDestroy() override;
        virtual void OnCollisionBegin(const CollisionContact& contact) override;
        virtual void OnCollisionEnd(const CollisionContact& contact) override;
        virtual void OnCollisionHit(const CollisionContact& contact) override;

    public:
        Vec2 m_WorldGravity = { 0.0f, 0.0f };
        int m_CurrentWave = 0;
        int m_Score = 0;
        float m_TimeBetweenWaves = 3.0f;

        void AddScore(int points);
        void GameOver();
        int GetCurrentWave() const;
        int GetScore() const;
        void ResetGame();

    private:
        void CheckWaveComplete();
        void StartNextWave();

	private:
        float m_WaveDelayTimer = 0.0f;
        bool m_WaitingForNextWave = false;
        bool m_HasSpawnedEnemies = false;
    };
}
