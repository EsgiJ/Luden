#include "Scene/Scene_Zelda.h"
#include "Core/GameEngine.h"
#include "ECS/Components/Components.h"
#include "ECS/EntityManager.h"
#include "Physics/Physics.h"

#include <fstream>
#include <iostream>

#include <imgui.h>
#include <imgui-SFML.h>

namespace Luden 
{
	Scene_Zelda::Scene_Zelda(GameEngine* game, std::string& levelPath)
		: Scene(game), m_LevelPath(levelPath),
		m_GridText(m_Game->GetAssets().GetFont("Tech"), "")
	{
		Init(m_LevelPath);
	}

	void Scene_Zelda::Init(const std::string& levelPath) 
	{
		LoadLevel(levelPath);

		m_GridText.setCharacterSize(12);
		m_GridText.setFont(m_Game->GetAssets().GetFont("Tech"));

		RegisterAction(sf::Keyboard::Key::Escape, "QUIT");
		RegisterAction(sf::Keyboard::Key::P, "PAUSE");
		RegisterAction(sf::Keyboard::Key::Y, "TOGGLE_FOLLOW");
		RegisterAction(sf::Keyboard::Key::T, "TOGGLE_TEXTURE");
		RegisterAction(sf::Keyboard::Key::C, "TOGGLE_COLLISION");
		RegisterAction(sf::Keyboard::Key::G, "TOGGLE_GRID");

		RegisterAction(sf::Keyboard::Key::W, "UP");
		RegisterAction(sf::Keyboard::Key::S, "DOWN");
		RegisterAction(sf::Keyboard::Key::A, "LEFT");
		RegisterAction(sf::Keyboard::Key::D, "RIGHT");
		RegisterAction(sf::Keyboard::Key::Space, "ATTACK");
	}

	void Scene_Zelda::LoadLevel(const std::string& fileName) 
	{
		std::ifstream file(fileName);
		if (!file) 
		{
			std::cerr << "Scene_Zelda::LoadLevel failed: " << fileName << std::endl;
			return;
		}

		std::string entityType;
		while (file >> entityType) 
		{
			if (entityType == "Tile") 
			{
				std::string animationName;
				int roomX, roomY, tileX, tileY, blockMovement, blockVision;
				file >> animationName >> roomX >> roomY >> tileX >> tileY >> blockMovement >> blockVision;

				auto tile = EntityManager::Instance().AddEntity("Tile");
				tile.Add<CAnimation>(m_Game->GetAssets().GetAnimation(animationName), true);
				tile.Add<CTransform>(GetPosition(roomX, roomY, tileX, tileY));
				tile.Add<CBoundingBox>(tile.Get<CTransform>().pos, tile.Get<CAnimation>().animation->GetSize(), blockMovement, blockVision);
				tile.Add<CDraggable>();
			}
			else if (entityType == "NPC") 
			{
				// benzer NPC yükleme kodlarý buraya gelir...
			}
			else if (entityType == "Player") 
			{
				file >> m_PlayerConfig.X >> m_PlayerConfig.Y >> m_PlayerConfig.CX >> m_PlayerConfig.CY >> m_PlayerConfig.SPEED >> m_PlayerConfig.HEALTH;
				SpawnPlayer();
			}
			else {
				std::cerr << "Unknown entity type: " << entityType << std::endl;
			}
		}
	}

	Math::Vec2 Scene_Zelda::GetPosition(int rx, int ry, int tx, int ty) const 
	{
		return {
			rx * Width() + m_GridSize.x * tx - m_GridSize.x / 2.0f,
			ry * Height() + m_GridSize.y * ty - m_GridSize.y / 2.0f
		};
	}

	Math::Vec2 Scene_Zelda::GetRoomXY(const Math::Vec2& pos)
	{
		return Math::Vec2();
	}

	void Scene_Zelda::SpawnPlayer()
	{
		/*
		auto player = EntityManager::Instance().AddEntity("player");
		player.Add<CTransform>(Math::Vec2(m_PlayerConfig.X, m_PlayerConfig.Y));
		player.Add<CAnimation>(m_Game->GetAssets().GetAnimation("LinkStandDown"), true);
		player.Add<CBoundingBox>(Math::Vec2(m_PlayerConfig.X, m_PlayerConfig.Y), Math::Vec2(m_PlayerConfig.CX, m_PlayerConfig.CY), true, false);
		player.Add<CDraggable>();
		player.Add<CHealth>(m_PlayerConfig.HEALTH, m_PlayerConfig.HEALTH);
		player.Add<CState>("stand_down");
		*/
	}

	void Scene_Zelda::SpawnSword(const Entity& entity)
	{
	}

	void Scene_Zelda::player()
	{
		return;
	}


	Math::Vec2 Scene_Zelda::WindowToWorld(const Math::Vec2& pos) 
	{
		auto view = m_Game->GetWindow().getView();
		float wx = view.getCenter().x - Width() / 2.0f;
		float wy = view.getCenter().y - Height() / 2.0f;
		return { pos.x + wx, pos.y + wy };
	}

	void Scene_Zelda::ChangePlayerStateTo(const std::string& state, const Math::Vec2& facing)
	{
	}

	void Scene_Zelda::sDrag()
	{
	}

	void Scene_Zelda::sCamera()
	{
	}

	void Scene_Zelda::sGUI()
	{
		ImGui::Begin("Scene - Zelda");
		ImGui::Checkbox("Draw Textures", &m_DrawTextures);
		ImGui::Checkbox("Draw Collision", &m_DrawCollision);
		ImGui::Checkbox("Draw Grid", &m_DrawGrid);
		ImGui::Checkbox("Follow Camera", &m_Follow);
		ImGui::End();
	}

	void Scene_Zelda::sDoAction(const Action& action) {
		if (action.Type() == "START") 
		{
			if (action.Name() == "QUIT") OnEnd();
			else if (action.Name() == "TOGGLE_TEXTURE") m_DrawTextures = !m_DrawTextures;
			else if (action.Name() == "TOGGLE_COLLISION") m_DrawCollision = !m_DrawCollision;
			else if (action.Name() == "TOGGLE_GRID") m_DrawGrid = !m_DrawGrid;
		}
	}

	void Scene_Zelda::Update() {
		if (m_Paused) return;
		sGUI();
		sRender();
		m_CurrentFrame++;
	}

	void Scene_Zelda::sRender() {
		m_Game->GetWindow().clear(sf::Color(60, 60, 60));
		// render entities
		m_Game->GetWindow().display();
	}

	void Scene_Zelda::OnEnd() {
		// TODO
	}
	void Scene_Zelda::sAI()
	{
	}
	void Scene_Zelda::sStatus()
	{
	}
	void Scene_Zelda::sAnimation()
	{
	}
	void Scene_Zelda::sCollision()
	{
	}
	void Scene_Zelda::sMovement()
	{
	}
}
