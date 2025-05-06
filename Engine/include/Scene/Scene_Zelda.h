#pragma once
#include "Scene/Scene.h"
#include "ECS/Entity.h"
#include <SFML/Graphics/Text.hpp>
#include <memory>

class Scene_Zelda : public Luden::Scene {
	std::string m_LevelPath;
	sf::Text m_GridText;
	vec2 m_MousePos;
	vec2 m_GridSize = { 64, 64 };

	bool m_DrawTextures = true;
	bool m_DrawCollision = false;
	bool m_DrawGrid = false;
	bool m_Follow = false;

	struct PlayerConfig {
		float X, Y, CX, CY, SPEED;
		int HEALTH;
	} m_PlayerConfig;

	void init(const std::string& levelPath);
	void loadLevel(const std::string& fileName);
	void spawnPlayer();
	void spawnSword(std::shared_ptr<Luden::Entity> entity);

	std::shared_ptr<Luden::Entity> player();
	vec2 getPosition(int rx, int ry, int tx, int ty) const;
	vec2 getRoomXY(const vec2& pos);
	vec2 windowToWorld(const vec2& pos);
	void changePlayerStateTo(const std::string& state, const vec2& facing);

	void sDrag();
	void sCamera();
	void sGUI();

public:
	Scene_Zelda(Luden::GameEngine* game, std::string& levelPath);

	void update() override;
	void sRender() override;
	void sDoAction(const Action& action) override;
	void onEnd() override;

	// NOT: AI, Status, Animation, Collision gibi system fonksiyonlarý burada tanýmlanabilir
	void sAI();
	void sStatus();
	void sAnimation();
	void sCollision();
	void sMovement();
};
