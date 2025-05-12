#pragma once

#include <memory>

#include <SFML/Graphics/Text.hpp>

#include "Core/GameEngine.h"
#include "ECS/Entity.h"
#include "Scene/Scene.h"

namespace Luden
{
	class Scene_Zelda : public Scene 
	{
		std::string m_LevelPath;
		sf::Text m_GridText;
		Math::Vec2 m_MousePos;
		Math::Vec2 m_GridSize = { 64, 64 };

		bool m_DrawTextures = true;
		bool m_DrawCollision = false;
		bool m_DrawGrid = false;
		bool m_Follow = false;

		struct PlayerConfig 
		{
			float X, Y, CX, CY, SPEED;
			int HEALTH;
		} m_PlayerConfig;

		void Init(const std::string& levelPath);
		void LoadLevel(const std::string& fileName);
		void SpawnPlayer();
		void SpawnSword(const Entity& entity);

		void player();
		Math::Vec2 GetPosition(int rx, int ry, int tx, int ty) const;
		Math::Vec2 GetRoomXY(const Math::Vec2& pos);
		Math::Vec2 WindowToWorld(const Math::Vec2& pos);
		void ChangePlayerStateTo(const std::string& state, const Math::Vec2& facing);

		void sDrag();
		void sCamera();
		void sGUI();

	public:
		Scene_Zelda(Luden::GameEngine* game, std::string& levelPath);

		void Update() override;
		void sRender() override;
		void sDoAction(const Action& action) override;
		void OnEnd() override;

		void sAI();
		void sStatus();
		void sAnimation();
		void sCollision();
		void sMovement();
	};
}

