#pragma once
#include "Luden.h"

namespace Luden
{
	class GameManager : public ScriptableEntity
    {
    public:
        virtual void OnCreate() override;
        virtual void OnUpdate(TimeStep ts) override;
        virtual void OnDestroy() override;
        virtual void OnCollisionBegin(const CollisionContact& contact) override;
        virtual void OnCollisionEnd(const CollisionContact& contact) override;
        virtual void OnCollisionHit(const CollisionContact& contact) override;

		void AddScore(int points);
		void LoseLife();
		void NextLevel();
		void GameOver();

		int GetScore() const { return m_Score; }
		int GetLives() const { return m_Lives; }
		int GetLevel() const { return m_Level; }

	private:
		void SpawnBricks();
		void UpdateUI();

		int m_Score = 0;
		int m_Lives = 3;
		int m_Level = 1;
		bool m_GameOver = false;

		// Brick spawning
		int m_BrickRows = 6;
		int m_BrickColumns = 8;
		float m_BrickSpacing = 20.0f;

		// Prefabs
		PrefabRef m_BrickPrefab;

		// Sounds
		MusicRef m_BackgroundMusic;
		SoundRef m_GameOverSound;
		SoundRef m_LevelUpSound;

		// UI
		Entity m_ScoreText;
		Entity m_LivesText;
		Entity m_LevelText;
    };
}
