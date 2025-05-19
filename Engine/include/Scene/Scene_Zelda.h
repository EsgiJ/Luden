#pragma once

#include <memory>

#include <SFML/Graphics/Text.hpp>
#include <rttr/rttr_enable.h>
#include <rttr/registration_friend.h>

#include "Core/GameEngine.h"
#include "ECS/Entity.h"
#include "Scene/Scene.h"

namespace Luden
{
	class ENGINE_API Scene_Zelda : public Scene 
	{
		RTTR_ENABLE(Scene)
		RTTR_REGISTRATION_FRIEND

		std::string m_LevelPath;
		sf::Text m_GridText;
		Math::Vec2 m_MousePos;
		Math::Vec2 m_GridSize = { 64, 64 };

		bool m_DrawTextures = true;
		bool m_DrawCollision = true;
		bool m_DrawGrid = true;
		bool m_Follow = false;

		struct ENGINE_API PlayerConfig 
		{
			float X, Y, CX, CY, SPEED;
			int HEALTH;
		} m_PlayerConfig;

		void Init(const std::string& levelPath);
		void LoadLevel(const std::string& fileName);
		void SpawnPlayer();
		void SpawnSword(const Entity& entity);

		Entity& Player();
		void AnimatePlayer();
		Math::Vec2 GetPosition(int rx, int ry, int tx, int ty) const;
		Math::Vec2 GetRoomXY(const Math::Vec2& pos);
		Math::Vec2 WindowToWorld(const Math::Vec2& pos);
		void ChangePlayerStateTo(const std::string& state, const Math::Vec2& facing);

		void sDrag();
		void sCamera();
		void sGUI();

	public:
		Scene_Zelda(Luden::GameEngine* game, const std::string& levelPath);

		void Update() override;
		void sRender(sf::RenderTarget& target) override;
		void sDoAction(const Action& action) override;
		void OnEnd() override;

		void sAI();
		void sStatus();
		void sAnimation();
		void sCollision();
		void sMovement();
	};
}

