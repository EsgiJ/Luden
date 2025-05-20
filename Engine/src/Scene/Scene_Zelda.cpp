#include "Scene/Scene_Zelda.h"

#include "Core/GameEngine.h"
#include "ECS/Components/Components.h"
#include "ECS/EntityMemoryPool.h"
#include "Physics/Physics.h"
#include "Scene/Scene_Menu.h"

#include <fstream>
#include <iostream>

#include <imgui.h>
#include <imgui-SFML.h>

namespace Luden 
{
	Scene_Zelda::Scene_Zelda(GameEngine* game, const std::string& levelPath)
		: Scene(game), m_LevelPath(levelPath),
		m_GridText(m_Game->GetAssets().GetFont("Tech"), "")
	{
		Init(m_LevelPath);
	}

	void Scene_Zelda::Init(const std::string& levelPath) 
	{
		m_GridSize = Math::Vec2(64.0f, 64.0f);
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

				auto tile = m_EntityManager.AddEntity("Tile");
				tile.Add<CAnimation>(m_Game->GetAssets().GetAnimation(animationName), true);
				tile.Add<CTransform>(GetPosition(roomX, roomY, tileX, tileY));

				Math::Vec2 boxSize = tile.Get<CAnimation>().animation.GetSize();
				boxSize -= Math::Vec2(2.0f, 2.0f);

				tile.Add<CBoundingBox>(tile.Get<CTransform>().pos, boxSize, blockMovement, blockVision);
				tile.Add<CDraggable>();
			}
			else if (entityType == "NPC")
			{
				std::string animationName, aiType;
				int roomX, roomY, tileX, tileY;
				int blockMovement, blockVision;
				int maxHealth, damage;

				file >> animationName >> roomX >> roomY >> tileX >> tileY;
				file >> blockMovement >> blockVision >> maxHealth >> damage >> aiType;

				auto npc = m_EntityManager.AddEntity("NPC");
				npc.Add<CAnimation>(m_Game->GetAssets().GetAnimation(animationName), true);
				npc.Add<CTransform>(GetPosition(roomX, roomY, tileX, tileY));
				npc.Add<CBoundingBox>(npc.Get<CTransform>().pos, Math::Vec2(63.0f, 63.0f), false, false);
				npc.Add<CHealth>(maxHealth, maxHealth);
				npc.Add<CDamage>(damage);

				if (aiType == "Follow")
				{
					float speed;
					file >> speed;
					npc.Add<CFollowPlayer>(npc.Get<CTransform>().pos, speed);
				}
				else if (aiType == "Patrol")
				{
					float speed;
					int numPositions;
					file >> speed >> numPositions;

					std::vector<Math::Vec2> positions;
					for (int i = 0; i < numPositions; ++i)
					{
						int xi, yi;
						file >> xi >> yi;
						positions.emplace_back(static_cast<float>(xi), static_cast<float>(yi));
					}
					npc.Add<CPatrol>(positions, speed);
				}
			}
			else if (entityType == "Player")
			{
				file >> m_PlayerConfig.X >> m_PlayerConfig.Y
					>> m_PlayerConfig.CX >> m_PlayerConfig.CY
					>> m_PlayerConfig.SPEED >> m_PlayerConfig.HEALTH;

				SpawnPlayer();
			}
			else
			{
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
		if (!m_Game)
		{
			std::cerr << "[Scene_Zelda::GetRoomXY] m_Game is null!" << std::endl;
			return { 0, 0 };
		}

		sf::Vector2u winSize(1280, 720);
		if (m_Game->GetWindow().isOpen())
			winSize = m_Game->GetWindow().getSize();

		int roomX = static_cast<int>(pos.x) / static_cast<int>(winSize.x);
		int roomY = static_cast<int>(pos.y) / static_cast<int>(winSize.y);
		if (pos.x < 0) roomX--;
		if (pos.y < 0) roomY--;
		return { static_cast<float>(roomX), static_cast<float>(roomY) };
	}

	void Scene_Zelda::SpawnPlayer()
	{
		auto player = m_EntityManager.AddEntity("Player");

		player.Add<CTransform>(Math::Vec2(m_PlayerConfig.X, m_PlayerConfig.Y));
		player.Add<CAnimation>(m_Game->GetAssets().GetAnimation("LinkStandDown"), true);
		player.Add<CBoundingBox>(
			Math::Vec2(m_PlayerConfig.X, m_PlayerConfig.Y),
			Math::Vec2(m_PlayerConfig.CX, m_PlayerConfig.CY),
			true, false
		);
		player.Add<CHealth>(m_PlayerConfig.HEALTH, m_PlayerConfig.HEALTH / 2);
		player.Add<CState>("stand_down");
		player.Add<CInput>();
	}


	void Scene_Zelda::SpawnSword(const Entity& entity)
	{
		auto sword = m_EntityManager.AddEntity("Sword");

		// lifespan: 8 frame
		sword.Add<CLifespan>(8, m_CurrentFrame);

		const auto& transform = entity.Get<CTransform>();
		const auto& box = entity.Get<CBoundingBox>();

		const Math::Vec2& facing = transform.facing;
		const Math::Vec2& center = box.center;

		const float swordL = 45.0f;
		const float swordW = 15.0f;

		Math::Vec2 swordCenter(
			center.x + facing.x * (swordL + 10.0f),
			center.y + facing.y * (swordL + 10.0f)
		);

		sword.Add<CTransform>(swordCenter);

		if (facing.x != 0.0f) {
			sword.Add<CBoundingBox>(swordCenter, Math::Vec2(swordL, swordW), false, false);
		}
		else if (facing.y != 0.0f) {
			sword.Add<CBoundingBox>(swordCenter, Math::Vec2(swordW, swordL), false, false);
		}

		// damage
		sword.Add<CDamage>(1);

		// play sound
		m_Game->GetAssets().GetSound("SSwordSlash").play();
	}


	Entity& Scene_Zelda::Player()
	{
		for (auto& e: m_EntityManager.GetEntities("Player"))
		{
			return e;
		};
	}

	void Scene_Zelda::AnimatePlayer()
	{
		Entity p = Player();
		if (!p.IsActive()) return;

		auto& transform = p.Get<CTransform>();
		auto& input = p.Get<CInput>();

		if (input.attack)
		{
			ChangePlayerStateTo("attack", transform.facing);
		}
		else if (transform.velocity.x > 0)
		{
			transform.facing = Math::Vec2(1, 0);
			ChangePlayerStateTo("move", transform.facing);
			transform.scale.x = 1;
		}
		else if (transform.velocity.x < 0)
		{
			transform.facing = Math::Vec2(-1, 0);
			ChangePlayerStateTo("move", transform.facing);
			transform.scale.x = -1;
		}
		else if (transform.velocity.y < 0)
		{
			transform.facing = Math::Vec2(0, -1);
			ChangePlayerStateTo("move", transform.facing);
		}
		else if (transform.velocity.y > 0)
		{
			transform.facing = Math::Vec2(0, 1);
			ChangePlayerStateTo("move", transform.facing);
		}
		else if (transform.velocity == Math::Vec2(0, 0))
		{
			ChangePlayerStateTo("stand", transform.facing);
		}

		auto& state = p.Get<CState>();
		if (state.changeAnimation)
		{
			const std::string& s = state.state;
			if (s == "stand_down")
				p.Add<CAnimation>(m_Game->GetAssets().GetAnimation("LinkStandDown"), true);
			else if (s == "stand_up")
				p.Add<CAnimation>(m_Game->GetAssets().GetAnimation("LinkStandUp"), true);
			else if (s == "stand_right" || s == "stand_left")
				p.Add<CAnimation>(m_Game->GetAssets().GetAnimation("LinkStandRight"), true);
			else if (s == "move_down")
				p.Add<CAnimation>(m_Game->GetAssets().GetAnimation("LinkMoveDown"), true);
			else if (s == "move_up")
				p.Add<CAnimation>(m_Game->GetAssets().GetAnimation("LinkMoveUp"), true);
			else if (s == "move_right" || s == "move_left")
				p.Add<CAnimation>(m_Game->GetAssets().GetAnimation("LinkMoveRight"), true);
			else if (s == "attack_down")
				p.Add<CAnimation>(m_Game->GetAssets().GetAnimation("LinkAtkDown"), true);
			else if (s == "attack_up")
				p.Add<CAnimation>(m_Game->GetAssets().GetAnimation("LinkAtkUp"), true);
			else if (s == "attack_right" || s == "attack_left")
				p.Add<CAnimation>(m_Game->GetAssets().GetAnimation("LinkAtkRight"), true);
		}
	}

	Math::Vec2 Scene_Zelda::WindowToWorld(const Math::Vec2& pos)
	{
		if (!m_Game || !m_Game->GetWindow().isOpen())
			return pos;

		auto view = m_Game->GetWindow().getView();
		float wx = view.getCenter().x - Width() / 2.0f;
		float wy = view.getCenter().y - Height() / 2.0f;
		return { pos.x + wx, pos.y + wy };
	}


	void Scene_Zelda::ChangePlayerStateTo(const std::string& state, const Math::Vec2& facing)
	{
		auto player = m_EntityManager.GetEntities("Player");
		if (player.empty()) return;

		auto& p = player[0];
		std::string newState;

		if (state == "stand")
		{
			if (facing == Math::Vec2(1, 0)) newState = "stand_right";
			else if (facing == Math::Vec2(-1, 0)) newState = "stand_left";
			else if (facing == Math::Vec2(0, -1)) newState = "stand_up";
			else if (facing == Math::Vec2(0, 1)) newState = "stand_down";
		}
		else if (state == "move")
		{
			if (facing == Math::Vec2(1, 0)) newState = "move_right";
			else if (facing == Math::Vec2(-1, 0)) newState = "move_left";
			else if (facing == Math::Vec2(0, -1)) newState = "move_up";
			else if (facing == Math::Vec2(0, 1)) newState = "move_down";
		}
		else if (state == "attack")
		{
			if (facing == Math::Vec2(1, 0)) newState = "attack_right";
			else if (facing == Math::Vec2(-1, 0)) newState = "attack_left";
			else if (facing == Math::Vec2(0, -1)) newState = "attack_up";
			else if (facing == Math::Vec2(0, 1)) newState = "attack_down";
		}

		auto& stateComp = p.Get<CState>();
		if (stateComp.previousState != newState)
		{
			stateComp.previousState = stateComp.state;
			stateComp.state = newState;
			stateComp.changeAnimation = true;
		}
		else
		{
			stateComp.changeAnimation = false;
		}
	}


	void Scene_Zelda::sDrag()
	{
		for (auto& entity : m_EntityManager.GetEntities())
		{
			if (entity.Has<CDraggable>() && entity.Get<CDraggable>().dragging)
			{
				Math::Vec2 wPos = WindowToWorld(m_MousePos);
				entity.Get<CTransform>().pos = wPos;
				entity.Get<CBoundingBox>().center = wPos;
			}
		}
	}


	void Scene_Zelda::sCamera()
	{
		if (!m_Game || !m_Game->GetWindow().isOpen())
			return;

		sf::View view = m_Game->GetWindow().getView();

		auto p = Player();
		if (!p.IsActive()) return;

		const auto& pPos = p.Get<CTransform>().pos;

		if (m_Follow)
		{
			view.setCenter(sf::Vector2f(pPos.x, pPos.y));
		}
		else
		{
			Math::Vec2 r = GetRoomXY(pPos);
			view.setCenter(sf::Vector2f(
				r.x * Width() + Width() / 2.0f,
				r.y * Height() + Height() / 2.0f
			));
		}

		m_Game->GetWindow().setView(view);
	}


	void Scene_Zelda::sGUI()
	{
		/*
		ImGui::Begin("Scene Properties");

		if (ImGui::BeginTabBar("MyTabBar")) 
		{

			if (ImGui::BeginTabItem("Debug")) {
				ImGui::Checkbox("Draw Grid (G)", &m_DrawGrid);
				ImGui::Checkbox("Draw Textures (T)", &m_DrawTextures);
				ImGui::Checkbox("Draw Debug (C)", &m_DrawCollision);
				ImGui::Checkbox("Follow Cam (Y)", &m_Follow);
				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Animations")) 
			{
				ImGui::Text("Click a Tile to create on the Game, then drag to move.");

				static bool bm = true;
				static bool bv = true;
				ImGui::Checkbox("Block Movement", &bm);
				ImGui::Checkbox("Block Vision", &bv);

				int i = 0;
				for (const auto& [name, anim] : m_Game->GetAssets().GetAnimations()) 
				{
					if (name.find("Tile") != std::string::npos) 
					{
						if (i++ % 5 != 0) ImGui::SameLine();

						sf::Vector2f size(
							anim.GetSprite().getTextureRect().size.x,
							anim.GetSprite().getTextureRect().size.y
						);

						if (ImGui::ImageButton(name.c_str(), anim.GetSprite(), size, sf::Color::Transparent, sf::Color::White)) 
						{
							auto tile = m_EntityManager.GetEntities("Tile");
							tile.Add<CAnimation>(m_Game->GetAssets().GetAnimation(name), true);
							auto view = m_Game->GetWindow().getView().getCenter();
							if (!m_Game->GetWindow().isOpen())
								view = { 640, 360 }; 

							tile.Add<CTransform>(Math::Vec2(view.x, view.y));
							tile.Add<CBoundingBox>(
								Math::Vec2(view.x, view.y),
								tile.Get<CAnimation>().animation->GetSize() - Math::Vec2(2.0f, 2.0f),
								bm,
								bv
							);
							tile.Add<CDraggable>();
						}
						ImGui::SetItemTooltip("%s", name.c_str());
					}
				}
				ImGui::EndTabItem();
			}
			ImGui::EndTabBar();
		}

		ImGui::End();*/
	}

	void Scene_Zelda::sDoAction(const Action& action) 
	{
		if (action.Name() == "MOUSE_MOVE") 
		{
			m_MousePos = action.Pos();
		}

		if (action.Type() == "START") 
		{
			if (action.Name() == "QUIT") OnEnd();
			else if (action.Name() == "PAUSE") m_Paused = !m_Paused;
			else if (action.Name() == "TOGGLE_TEXTURE") m_DrawTextures = !m_DrawTextures;
			else if (action.Name() == "TOGGLE_COLLISION") m_DrawCollision = !m_DrawCollision;
			else if (action.Name() == "TOGGLE_GRID") m_DrawGrid = !m_DrawGrid;
			else if (action.Name() == "TOGGLE_FOLLOW") m_Follow = !m_Follow;
			else if (action.Name() == "LEFT_CLICK") {
				Math::Vec2 worldPos = WindowToWorld(m_MousePos);
				for (auto& e : m_EntityManager.GetEntities())
				{
					if (!e.Has<CDraggable>()) continue;

					if (Physics::IsInside(worldPos, e)) 
					{
						e.Get<CDraggable>().dragging = !e.Get<CDraggable>().dragging;
					}
				}
			}
			else 
			{
				// Movement input
				Entity player = Player();
				if (!player.IsActive() || !player.Has<CInput>()) return;

				auto& input = player.Get<CInput>();
				if (action.Name() == "UP") input.up = true;
				else if (action.Name() == "DOWN") input.down = true;
				else if (action.Name() == "LEFT") input.left = true;
				else if (action.Name() == "RIGHT") input.right = true;
				else if (action.Name() == "ATTACK") 
				{
					m_Game->GetAssets().GetSound("SSwordSlash").play();
					input.attack = true;
				}
			}
		}
		else if (action.Type() == "END") 
		{
			Entity player = Player();
			if (!player.IsActive() || !player.Has<CInput>()) return;

			auto& input = player.Get<CInput>();
			if (action.Name() == "UP") input.up = false;
			else if (action.Name() == "DOWN") input.down = false;
			else if (action.Name() == "LEFT") input.left = false;
			else if (action.Name() == "RIGHT") input.right = false;
			else if (action.Name() == "ATTACK") input.attack = false;
		}
	}

	void Scene_Zelda::Update() 
	{
		m_EntityManager.Update();

		if (!m_Paused) 
		{
			sDrag();
			sAI();
			sMovement();
			sStatus();
			sCollision();
			sAnimation();
			sCamera();
			m_CurrentFrame++;
		}

		//sGUI();
	}

	void Scene_Zelda::sRender(sf::RenderTarget& target)
	{
		target.clear(sf::Color(60, 60, 60));

		// -- Entity Texture & Animation Drawing --
		if (m_DrawTextures)
		{
			for (auto& e : m_EntityManager.GetEntities())
			{
				if (!e.Has<CTransform>()) continue;

				auto& transform = e.Get<CTransform>();
				sf::Color c = sf::Color::White;
				if (e.Has<CInvincibility>())
					c = sf::Color(255, 255, 255, 128);

				if (e.Has<CAnimation>())
				{
					auto& animation = e.Get<CAnimation>().animation;
					auto& sprite = animation.GetSprite();
					sprite.setPosition(sf::Vector2(transform.pos.x, transform.pos.y));
					sprite.setRotation(sf::degrees(transform.angle));
					sprite.setScale(sf::Vector2(transform.scale.x, transform.scale.y));
					sprite.setColor(c);
					target.draw(sprite);
				}
			}

			// -- Entity Health Bar Drawing --
			for (auto& e : m_EntityManager.GetEntities())
			{
				if (!e.Has<CTransform>() || !e.Has<CHealth>()) continue;

				auto& transform = e.Get<CTransform>();
				auto& h = e.Get<CHealth>();

				if (h.max == 0) {
					std::cerr << "[ERROR] Entity has 0 max health! Tag: " << e.Tag() << "\n";
					continue;
				}

				Math::Vec2 size(64.0f, 6.0f);
				sf::RectangleShape barBg({ size.x, size.y });
				barBg.setPosition(sf::Vector2(transform.pos.x - 32.0f, transform.pos.y - 48.0f));
				barBg.setFillColor(sf::Color(96, 96, 96));
				barBg.setOutlineColor(sf::Color::Black);
				barBg.setOutlineThickness(2.0f);
				target.draw(barBg);

				float ratio = (h.max != 0) ? static_cast<float>(h.current) / h.max : 0.0f;

				sf::RectangleShape barFill({ size.x * ratio, size.y });
				barFill.setPosition(barBg.getPosition());
				barFill.setFillColor(sf::Color(255, 0, 0));
				barFill.setOutlineThickness(0.0f);
				target.draw(barFill);

				sf::RectangleShape tick({ 1.0f, 6.0f });
				tick.setFillColor(sf::Color::Black);
				for (int i = 0; i < h.max; ++i)
				{
					tick.setPosition(
						barBg.getPosition() + sf::Vector2f(i * 64.0f / h.max, 0)
					);
					target.draw(tick);
				}
			}
		}

		// -- Grid Drawing --
		if (m_DrawGrid)
		{
			sf::RectangleShape rect({ m_GridSize.x, m_GridSize.y });
			rect.setFillColor(sf::Color::Transparent);
			rect.setOutlineColor(sf::Color::White);
			rect.setOutlineThickness(1.0f);

			for (int y = 0; y < 10; ++y)
			{
				for (int x = 0; x < 10; ++x)
				{
					rect.setPosition(sf::Vector2f(x * m_GridSize.x, y * m_GridSize.y));
					target.draw(rect);
				}
			}
		}
	}

	void Scene_Zelda::OnEnd() 
	{
		m_Game->ChangeScene("MENU", std::make_shared<Luden::Scene_Menu>(m_Game));
	}

	void Scene_Zelda::sAI() {
		for (auto& npc : m_EntityManager.GetEntities("NPC"))
		{
			Entity p = Player();
			if (!p.IsActive()) return;

			const auto& pPos = p.Get<CTransform>().pos;
			auto& npcPos = npc.Get<CTransform>().pos;

			if (GetRoomXY(pPos) == GetRoomXY(npcPos) && npc.Has<CFollowPlayer>()) 
			{
				bool blockVision = false;
				for (const auto& tile : m_EntityManager.GetEntities("Tile"))
				{
					if (tile.Has<CBoundingBox>() && tile.Get<CBoundingBox>().blockVision && Physics::EntityIntersect(pPos, npcPos, tile)) 
					{
						blockVision = true;
						break;
					}
				}
				if (!blockVision) 
				{
					npc.Get<CTransform>().velocity = Physics::GetSpeedAB(npcPos, pPos, npc.Get<CFollowPlayer>().speed);
				}
				else 
				{
					npc.Get<CTransform>().velocity = Physics::GetSpeedAB(npcPos, npc.Get<CFollowPlayer>().home, npc.Get<CFollowPlayer>().speed);
				}
			}

			if (npc.Has<CPatrol>()) {
				size_t current = npc.Get<CPatrol>().currentPosition;
				auto& positions = npc.Get<CPatrol>().positions;
				size_t next = (current + 1) % positions.size();

				Math::Vec2 to = GetPosition(
					static_cast<int>(GetRoomXY(npcPos).x),
					static_cast<int>(GetRoomXY(npcPos).y),
					static_cast<int>(positions[next].x),
					static_cast<int>(positions[next].y)
				);

				npc.Get<CTransform>().velocity = Physics::GetSpeedAB(npcPos, to, npc.Get<CPatrol>().speed);
				if (Math::Length(npcPos - to) < 2.0f) {
					current = next;
				}
			}
		}
	}

	void Scene_Zelda::sStatus() {
		for (auto& e : m_EntityManager.GetEntities()) {
			if (e.Has<CLifespan>()) {
				if (m_CurrentFrame - e.Get<CLifespan>().frameCreated > e.Get<CLifespan>().lifespan) {
					e.Destroy();
				}
			}
			if (e.Has<CInvincibility>()) {
				if (m_CurrentFrame - e.Get<CInvincibility>().iframes > 60) {
					e.Remove<CInvincibility>();
				}
			}
		}
	}

	void Scene_Zelda::sAnimation() {
		AnimatePlayer();

		for (auto& e : m_EntityManager.GetEntities()) {
			if (e.Has<CAnimation>()) {
				e.Get<CAnimation>().animation.Update();
			}
		}
	}

	void Scene_Zelda::sCollision() {
		Entity p = Player();
		if (!p.IsActive()) return;

		// player and tiles
		for (auto& tile : m_EntityManager.GetEntities("Tile")) {
			RectOverlap ro = Physics::AIsNearB(p, tile, m_GridSize);

			switch (ro.m_Direction) {
			case ODirection::UP:
				if (tile.Has<CBoundingBox>() && tile.Get<CBoundingBox>().blockMove) {
					p.Get<CTransform>().pos.y -= ro.m_Overlap.y;
					p.Get<CBoundingBox>().center.y -= ro.m_Overlap.y;
				}
				break;
			case ODirection::DOWN:
				if (tile.Has<CBoundingBox>() && tile.Get<CBoundingBox>().blockMove) {
					p.Get<CTransform>().pos.y += ro.m_Overlap.y;
					p.Get<CBoundingBox>().center.y += ro.m_Overlap.y;
				}
				break;
			case ODirection::LEFT:
				if (tile.Has<CBoundingBox>() && tile.Get<CBoundingBox>().blockMove) {
					p.Get<CTransform>().pos.x -= ro.m_Overlap.x;
					p.Get<CBoundingBox>().center.x -= ro.m_Overlap.x;
				}
				break;
			case ODirection::RIGHT:
				if (tile.Has<CBoundingBox>() && tile.Get<CBoundingBox>().blockMove) {
					p.Get<CTransform>().pos.x += ro.m_Overlap.x;
					p.Get<CBoundingBox>().center.x += ro.m_Overlap.x;
				}
				break;
			case ODirection::NONE:
				break;
			}

			// heart
			if (tile.Has<CAnimation>() && tile.Get<CAnimation>().animation.GetName() == "TileHeart") {
				for (auto& e : m_EntityManager.GetEntities()) {
					if (!e.Has<CHealth>()) continue;
					if (Physics::GetOverlap(tile, e).x > 0 && Physics::GetOverlap(tile, e).y > 0) {
						e.Get<CHealth>().current = e.Get<CHealth>().max;
						m_Game->GetAssets().GetSound("SLinkPickupHeart").play();
						tile.Destroy();
					}
				}
			}
		}

		// player and NPCs
		for (auto& npc : m_EntityManager.GetEntities("NPC")) {
			RectOverlap ro = Physics::AIsNearB(npc, p, m_GridSize);

			switch (ro.m_Direction) {
			case ODirection::UP:
				if (npc.Get<CTransform>().velocity != Math::Vec2(0, 0)) {
					npc.Get<CTransform>().pos.y -= ro.m_Overlap.y;
					npc.Get<CBoundingBox>().center.y -= ro.m_Overlap.y;
				}
				break;
			case ODirection::DOWN:
				if (npc.Get<CTransform>().velocity != Math::Vec2(0, 0)) {
					npc.Get<CTransform>().pos.y += ro.m_Overlap.y;
					npc.Get<CBoundingBox>().center.y += ro.m_Overlap.y;
				}
				break;
			case ODirection::LEFT:
				if (npc.Get<CTransform>().velocity != Math::Vec2(0, 0)) {
					npc.Get<CTransform>().pos.x -= ro.m_Overlap.x;
					npc.Get<CBoundingBox>().center.x -= ro.m_Overlap.x;
				}
				break;
			case ODirection::RIGHT:
				if (npc.Get<CTransform>().velocity != Math::Vec2(0, 0)) {
					npc.Get<CTransform>().pos.x += ro.m_Overlap.x;
					npc.Get<CBoundingBox>().center.x += ro.m_Overlap.x;
				}
				break;
			case ODirection::NONE:
				break;
			}

			// damage
			if (ro.m_Direction != ODirection::NONE) 
			{
				if (npc.Has<CDamage>() && !p.Has<CInvincibility>()) 
				{
					p.Get<CHealth>().current -= npc.Get<CDamage>().damage;
					p.Add<CInvincibility>().iframes = static_cast<int>(m_CurrentFrame);
					m_Game->GetAssets().GetSound("SLinkDamaged").play();

					if (p.Get<CHealth>().current <= 0) 
					{
						p.Destroy();
						SpawnPlayer();
						m_Game->GetAssets().GetSound("SLinkDied").play();
					}
				}
			}

			// npc and sword
			for (auto& sword : m_EntityManager.GetEntities("Sword"))
			{
				if (!sword.Has<CDamage>()) continue;

				Math::Vec2 m_Overlap = Physics::GetOverlap(sword, npc);
				if (m_Overlap.x >= 0 && m_Overlap.y >= 0) {
					npc.Get<CHealth>().current -= sword.Get<CDamage>().damage;
					m_Game->GetAssets().GetSound("SEnemyDamaged").play();

					sword.Remove<CDamage>();
					if (npc.Get<CHealth>().current <= 0) {
						npc.Destroy();
						m_Game->GetAssets().GetSound("SEnemyDied").play();
						break;
					}
				}
			}

			// npc and tiles
			for (auto& tile : m_EntityManager.GetEntities("Tile")) {
				RectOverlap tro = Physics::AIsNearB(npc, tile, m_GridSize);

				if (!tile.Has<CBoundingBox>() || !tile.Get<CBoundingBox>().blockMove) continue;
				if (npc.Get<CTransform>().velocity == Math::Vec2(0, 0)) continue;

				switch (tro.m_Direction) {
				case ODirection::UP:
					npc.Get<CTransform>().pos.y -= tro.m_Overlap.y;
					npc.Get<CBoundingBox>().center.y -= tro.m_Overlap.y;
					break;
				case ODirection::DOWN:
					npc.Get<CTransform>().pos.y += tro.m_Overlap.y;
					npc.Get<CBoundingBox>().center.y += tro.m_Overlap.y;
					break;
				case ODirection::LEFT:
					npc.Get<CTransform>().pos.x -= tro.m_Overlap.x;
					npc.Get<CBoundingBox>().center.x -= tro.m_Overlap.x;
					break;
				case ODirection::RIGHT:
					npc.Get<CTransform>().pos.x += tro.m_Overlap.x;
					npc.Get<CBoundingBox>().center.x += tro.m_Overlap.x;
					break;
				default:
					break;
				}
			}
		}
	}

	void Scene_Zelda::sMovement() 
	{
		Entity player = Player();
		if (!player.IsActive()) return;

		auto& input = player.Get<CInput>();
		auto& transform = player.Get<CTransform>();
		transform.velocity = Math::Vec2(0, 0);

		if (input.up)    
			transform.velocity.y -= m_PlayerConfig.SPEED;
		if (input.down)  
			transform.velocity.y += m_PlayerConfig.SPEED;
		if (input.left)  
			transform.velocity.x -= m_PlayerConfig.SPEED;
		if (input.right) 
			transform.velocity.x += m_PlayerConfig.SPEED;

		if (input.attack && m_EntityManager.GetEntities("Sword").empty())
		{
			SpawnSword(player);
		}

		for (auto& e : m_EntityManager.GetEntities())
		{
			if (!e.Has<CTransform>()) continue;

			auto& t = e.Get<CTransform>();
			t.prevPos = t.pos;
			t.pos += t.velocity;

			if (e.Has<CBoundingBox>()) 
			{
				e.Get<CBoundingBox>().center = t.pos;
			}
		}
	}


}
