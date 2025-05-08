#include "Scene/Scene_Zelda.h"
#include "Core/GameEngine.h"
#include "ECS/IComponent.h"
#include "ECS/EntityManager.h"
#include "Physics/Physics.h"

#include <fstream>
#include <iostream>
#include <imgui.h>
#include <imgui-SFML.h>

namespace Luden {

	Scene_Zelda::Scene_Zelda(GameEngine* game, std::string& levelPath)
		: Scene(game), m_LevelPath(levelPath) {
		Init(m_LevelPath);
	}

	void Scene_Zelda::Init(const std::string& levelPath) {
		LoadLevel(levelPath);

		m_GridText.setCharacterSize(12);
		m_GridText.setFont(m_Game->GetAssets().GetFont("Tech"));

		RegisterAction(sf::Keyboard::Escape, "QUIT");
		RegisterAction(sf::Keyboard::P, "PAUSE");
		RegisterAction(sf::Keyboard::Y, "TOGGLE_FOLLOW");
		RegisterAction(sf::Keyboard::T, "TOGGLE_TEXTURE");
		RegisterAction(sf::Keyboard::C, "TOGGLE_COLLISION");
		RegisterAction(sf::Keyboard::G, "TOGGLE_GRID");

		RegisterAction(sf::Keyboard::W, "UP");
		RegisterAction(sf::Keyboard::S, "DOWN");
		RegisterAction(sf::Keyboard::A, "LEFT");
		RegisterAction(sf::Keyboard::D, "RIGHT");
		RegisterAction(sf::Keyboard::Space, "ATTACK");
	}

	void Scene_Zelda::LoadLevel(const std::string& fileName) 

		std::ifstream file(fileName);
		if (!file) {
			std::cerr << "Scene_Zelda::LoadLevel failed: " << fileName << std::endl;
			return;
		}

		std::string entityType;
		while (file >> entityType) {
			if (entityType == "Tile") {
				std::string animationName;
				int roomX, roomY, tileX, tileY, blockMovement, blockVision;
				file >> animationName >> roomX >> roomY >> tileX >> tileY >> blockMovement >> blockVision;

				auto tile = EntityManager::Instance().AddEntity("Tile");
				tile.Add<CAnimation>(m_Game->GetAssets().GetAnimation(animationName), true);
				tile.Add<CTransform>(GetPosition(roomX, roomY, tileX, tileY));
				tile.Add<CBoundingBox>(tile.Get<CTransform>().pos, tile.Get<CAnimation>().animation.GetSize(), blockMovement, blockVision);
				tile.Add<CDraggable>();
			}
			else if (entityType == "NPC") {
				// benzer NPC yükleme kodlarý buraya gelir...
			}
			else if (entityType == "Player") {
				file >> m_PlayerConfig.X >> m_PlayerConfig.Y >> m_PlayerConfig.CX >> m_PlayerConfig.CY >> m_PlayerConfig.SPEED >> m_PlayerConfig.HEALTH;
				SpawnPlayer();
			}
			else {
				std::cerr << "Unknown entity type: " << entityType << std::endl;
			}
		}
	}

	vec2 Scene_Zelda::GetPosition(int rx, int ry, int tx, int ty) const {
		return {
			rx * Width() + m_GridSize.x * tx - m_GridSize.x / 2.0f,
			ry * Height() + m_GridSize.y * ty - m_GridSize.y / 2.0f
		};
	}

	void Scene_Zelda::SpawnPlayer() {
		auto player = EntityManager::Instance().AddEntity("player");
		player.Add<CTransform>(Math::Vec2(m_PlayerConfig.X, m_PlayerConfig.Y));
		player.Add<CAnimation>(m_Game->GetAssets().GetAnimation("LinkStandDown"), true);
		player.Add<CBoundingBox>(Math::Vec2(m_PlayerConfig.X, m_PlayerConfig.Y), Math::Vec2(m_PlayerConfig.CX, m_PlayerConfig.CY), true, false);
		player.Add<CDraggable>();
		player.Add<CHealth>(m_PlayerConfig.HEALTH, m_PlayerConfig.HEALTH);
		player.Add<CState>("stand_down");
	}

	std::shared_ptr<Entity> Scene_Zelda::player() {
		for (const auto& e : m_entityManager.GetEntities("player")) {
			return e;
		}
		return nullptr;
	}

	vec2 Scene_Zelda::WindowToWorld(const vec2& pos) {
		auto view = m_Game->GetWindow().getView();
		float wx = view.getCenter().x - Width() / 2.0f;
		float wy = view.getCenter().y - Height() / 2.0f;
		return { pos.x + wx, pos.y + wy };
	}

	void Scene_Zelda::sGUI() {
		ImGui::Begin("Scene - Zelda");
		ImGui::Checkbox("Draw Textures", &m_DrawTextures);
		ImGui::Checkbox("Draw Collision", &m_DrawCollision);
		ImGui::Checkbox("Draw Grid", &m_DrawGrid);
		ImGui::Checkbox("Follow Camera", &m_Follow);
		ImGui::End();
	}

	void Scene_Zelda::sDoAction(const Action& action) {
		if (action.type() == "START") {
			if (action.name() == "QUIT") OnEnd();
			else if (action.name() == "TOGGLE_TEXTURE") m_DrawTextures = !m_DrawTextures;
			else if (action.name() == "TOGGLE_COLLISION") m_DrawCollision = !m_DrawCollision;
			else if (action.name() == "TOGGLE_GRID") m_DrawGrid = !m_DrawGrid;
		}
	}

	void Scene_Zelda::Update() {
		if (m_Paused) return;
		EntityManager::Instance().Update();
		sGUI();
		sRender();
		m_CurrentFrame++;
	}

	void Scene_Zelda::sRender() {
		m_Game->GetWindow().clear(sf::Color(60, 60, 60));
		// render entities here...
		m_Game->GetWindow().display();
	}

	void Scene_Zelda::OnEnd() {
		// Scene bitiþinde yapýlacak iþlemler (örn: sahne deðiþimi)
	}

}
