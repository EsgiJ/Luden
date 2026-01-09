#include "GameManager.h"
#include <iostream>
#include "ScriptAPI/GameplayAPI.h"
#include "ScriptAPI/AudioAPI.h"
#include "ScriptAPI/MathAPI.h"

namespace Luden
{
    void GameManager::OnCreate()
    {
		std::cout << "[GameManager] Initializing..." << std::endl;

		// Load resources
		m_BrickPrefab = GetResource<Prefab>("Brick");
		m_BackgroundMusic = GetResource<Music>("background");
		m_GameOverSound = GetResource<Sound>("game_over");
		m_LevelUpSound = GetResource<Sound>("level_up");

		// Find UI
		m_ScoreText = GameplayAPI::FindEntityWithTag("ScoreText");
		m_LivesText = GameplayAPI::FindEntityWithTag("LivesText");
		m_LevelText = GameplayAPI::FindEntityWithTag("LevelText");

		// Start music
		if (m_BackgroundMusic)
			SoundAPI::PlayMusic(m_BackgroundMusic, 0.3f, true);

		// Spawn initial bricks
		SpawnBricks();

		UpdateUI();

		std::cout << "[GameManager] Ready!" << std::endl;
    }

    void GameManager::OnUpdate(TimeStep ts)
    {
		if (m_GameOver)
			return;

		// Check if all bricks destroyed
		auto bricks = GameplayAPI::FindAllEntitiesWithTag("Brick");
		if (bricks.empty())
		{
			NextLevel();
		}
    }

    void GameManager::OnDestroy()
    {
		SoundAPI::StopMusic();
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

	void GameManager::AddScore(int points)
	{
		m_Score += points * m_Level;  // Multiplier per level
		UpdateUI();
	}

	void GameManager::LoseLife()
	{
		m_Lives--;
		UpdateUI();

		std::cout << "[GameManager] Life lost! Remaining: " << m_Lives << std::endl;

		if (m_Lives <= 0)
		{
			GameOver();
		}
	}

	void GameManager::NextLevel()
	{
		m_Level++;

		if (m_LevelUpSound)
			SoundAPI::PlaySound(m_LevelUpSound, 1.0f);

		// Increase difficulty
		m_BrickRows = std::min(10, m_BrickRows + 1);

		SpawnBricks();
		UpdateUI();

		std::cout << "[GameManager] Level " << m_Level << " started!" << std::endl;
	}

	void GameManager::GameOver()
	{
		m_GameOver = true;

		if (m_GameOverSound)
			SoundAPI::PlaySound(m_GameOverSound, 1.0f);

		SoundAPI::StopMusic();

		std::cout << "[GameManager] GAME OVER! Final Score: " << m_Score << std::endl;
	}

	void GameManager::SpawnBricks()
	{
		if (!m_BrickPrefab)
			return;
		float brickWidth = 102.4f;   
		float brickHeight = 51.2f;  

		float totalWidth = (m_BrickColumns * brickWidth) + ((m_BrickColumns - 1) * m_BrickSpacing);
		float startX = -totalWidth / 2.0f + brickWidth / 2.0f;
		float startY = -400.0f;  

		for (int row = 0; row < m_BrickRows; row++)
		{
			for (int col = 0; col < m_BrickColumns; col++)
			{
				float x = startX + col * (brickWidth + m_BrickSpacing);
				float y = startY + row * (brickHeight + m_BrickSpacing);

				Vec3 pos(x, y, 0);
				Entity brick = GameplayAPI::SpawnPrefab(m_BrickPrefab, pos);

				if (!brick.IsValid())
				{
					std::cerr << "[GameManager] Failed to spawn brick at " << x << ", " << y << std::endl;
					continue;
				}

				// Randomize health
				if (brick.Has<HealthComponent>())
				{
					auto& health = brick.Get<HealthComponent>();
					health.max = MathAPI::RandomInt(1, 3);
					health.current = health.max;
				}
			}
		}

		std::cout << "[GameManager] Spawned " << (m_BrickRows * m_BrickColumns) << " bricks" << std::endl;
	}

	void GameManager::UpdateUI()
	{
		if (m_ScoreText.IsValid() && m_ScoreText.Has<TextComponent>())
		{
			auto& text = m_ScoreText.Get<TextComponent>();
			text.text = "Score: " + std::to_string(m_Score);
		}

		if (m_LivesText.IsValid() && m_LivesText.Has<TextComponent>())
		{
			auto& text = m_LivesText.Get<TextComponent>();
			text.text = "Lives: " + std::to_string(m_Lives);
		}

		if (m_LevelText.IsValid() && m_LevelText.Has<TextComponent>())
		{
			auto& text = m_LevelText.Get<TextComponent>();
			text.text = "Level: " + std::to_string(m_Level);
		}
	}
}
