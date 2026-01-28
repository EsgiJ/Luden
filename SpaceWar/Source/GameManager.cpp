#include "GameManager.h"
#include "ScriptAPI/GameplayAPI.h"

#include <iostream>

#include "EnemySpawner.h"

namespace Luden
{
    void GameManager::OnCreate()
    {
        GameplayAPI::SetWorldGravity(m_WorldGravity);

        std::cout << "[GameManager] Game started!" << std::endl;

        m_WaitingForNextWave = true;
        m_WaveDelayTimer = 0.1f;
    }

    void GameManager::OnUpdate(TimeStep ts)
    {
        if (m_WaitingForNextWave)
        {
            m_WaveDelayTimer -= ts;

            if (m_WaveDelayTimer <= 0.0f)
            {
                m_WaitingForNextWave = false;
                StartNextWave();
            }
            return;
        }

        if (!m_HasSpawnedEnemies)
        {
            auto enemies = GameplayAPI::FindAllEntitiesWithTag("Enemy");
            if (!enemies.empty())
            {
                m_HasSpawnedEnemies = true;
                std::cout << "[GameManager] Enemies spawned, wave active!" << std::endl;
            }
            return;  
        }

        CheckWaveComplete();
    }

    void GameManager::OnDestroy()
    {
        std::cout << "[GameManager] Game ended!" << std::endl;
        std::cout << "[GameManager] Final Score: " << m_Score << std::endl;
    }

    void GameManager::OnCollisionBegin(const CollisionContact& contact)
    {
        // TODO: On contact begin
    }

    void GameManager::OnCollisionEnd(const CollisionContact& contact)
    {
        // TODO: On contact end
    }

    void GameManager::OnCollisionHit(const CollisionContact& contact)
    {
        // TODO: On hit(high speed)
    }

    void GameManager::CheckWaveComplete()
    {
        auto enemies = GameplayAPI::FindAllEntitiesWithTag("Enemy");

        if (enemies.empty())
        {
            std::cout << "[GameManager] Wave " << m_CurrentWave << " complete!" << std::endl;
            std::cout << "[GameManager] Score: " << m_Score << std::endl;

            m_WaitingForNextWave = true;
            m_WaveDelayTimer = m_TimeBetweenWaves;

            std::cout << "[GameManager] Next wave in " << m_TimeBetweenWaves << " seconds...\n" << std::endl;
        }
    }

    void GameManager::StartNextWave()
    {
        m_CurrentWave++;
        m_HasSpawnedEnemies = false;  
    }

    void GameManager::AddScore(int points)
    {
        m_Score += points;
        std::cout << "[GameManager] Score: " << m_Score << " (+" << points << ")" << std::endl;
    }

    void GameManager::GameOver()
    {
        std::cout << "[GameManager] GAME OVER!" << std::endl;
        std::cout << "[GameManager] Final Wave: " << m_CurrentWave << std::endl;
        std::cout << "[GameManager] Final Score: " << m_Score << std::endl;

        GameplayAPI::ReloadCurrentScene();
    }

    int GameManager::GetCurrentWave() const
    {
        return m_CurrentWave;
    }

    int GameManager::GetScore() const
    {
        return m_Score;
    }

    void GameManager::ResetGame()
    {
        m_CurrentWave = 1;
        m_Score = 0;

        std::cout << "[GameManager] Game reset!" << std::endl;

        GameplayAPI::ReloadCurrentScene();
    }

}
