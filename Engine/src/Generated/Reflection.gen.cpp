#include <rttr/registration>
#include <rttr/enumeration.h>
using namespace rttr;

#include "ECS/Components/Components.h"
#include "ECS/IComponent.h"
#include "Asset/Asset.h"
#include "Input/Action.h"
#include "ECS/Entity.h"
#include "ECS/EntityManager.h"
#include "Core/GameEngine.h"
#include "Physics/Physics.h"
#include "Scene/Scene.h"
#include "Scene/Scene_Menu.h"
#include "Scene/Scene_Zelda.h"
#include "ECS/ISystem.h"

RTTR_REGISTRATION
{
	registration::class_<std::string>("std::string");
	registration::class_<std::vector<std::string>>("std::vector<std::string>");

	// Direction
	registration::enumeration<Luden::ODirection>("Direction")
		(
			value("Up", Luden::ODirection::UP),
			value("Down", Luden::ODirection::DOWN),
			value("Left", Luden::ODirection::LEFT),
			value("Right", Luden::ODirection::RIGHT)
		);

	// Player Config
	registration::class_<Luden::Scene_Zelda::PlayerConfig>("PlayerConfig")
		.property("x", &Luden::Scene_Zelda::PlayerConfig::X)
		.property("y", &Luden::Scene_Zelda::PlayerConfig::Y)
		.property("cx", &Luden::Scene_Zelda::PlayerConfig::CX)
		.property("cy", &Luden::Scene_Zelda::PlayerConfig::CY)
		.property("speed", &Luden::Scene_Zelda::PlayerConfig::SPEED)
		.property("hp", &Luden::Scene_Zelda::PlayerConfig::HEALTH);

	// IComponent
	registration::class_<Luden::IComponent>("IComponent")
		.property("has", &Luden::IComponent::has)
		(
			metadata("default", false)
		);

	registration::class_<Luden::CDamage>("CDamage")
		.constructor<>()
		.constructor<int>()
		.property("damage", &Luden::CDamage::damage)
		(
			metadata("default", 1)
		);

	registration::class_<Luden::CDraggable>("CDraggable")
		.constructor<>()
		.property("dragging", &Luden::CDraggable::dragging)
		(
			metadata("default", false)
		);

	registration::class_<Luden::CFollowPlayer>("CFollowPlayer")
		.constructor<>()
		.constructor<Luden::Math::Vec2, float>()
		.property("home", &Luden::CFollowPlayer::home)
		(
			metadata("default", Luden::Math::Vec2{ 0.0f, 0.0f })
		)
		.property("speed", &Luden::CFollowPlayer::speed)
		(
			metadata("default", 0.0f)
		);

	registration::class_<Luden::CGravity>("CGravity")
		.constructor<>()
		.constructor<float>()
		.property("gravity", &Luden::CGravity::gravity)
		(
			metadata("default", 0.0f)
		);

	registration::class_<Luden::CHealth>("CHealth")
		.constructor<>()
		.constructor<int, int>()
		.property("max", &Luden::CHealth::max)
		(
			metadata("default", 1)
		)
		.property("current", &Luden::CHealth::current)
		(
			metadata("default", 1)
		);

	registration::class_<Luden::CInput>("CInput")
		.constructor<>()
		.property("up", &Luden::CInput::up)
		(
			metadata("default", false)
		)
		.property("down", &Luden::CInput::down)
		(
			metadata("default", false)
		)
		.property("left", &Luden::CInput::left)
		(
			metadata("default", false)
		)
		.property("right", &Luden::CInput::right)
		(
			metadata("default", false)
		)
		.property("attack", &Luden::CInput::attack)
		(
			metadata("default", false)
		)
		.property("canAttack", &Luden::CInput::canAttack)
		(
			metadata("default", true)
		);

	registration::class_<Luden::CBoundingBox>("CBoundingBox")
		.constructor<>()
		.constructor<Luden::Math::Vec2&, Luden::Math::Vec2&, bool, bool>()
		.property("size", &Luden::CBoundingBox::size)
		.property("halfSize", &Luden::CBoundingBox::halfSize)
		.property("center", &Luden::CBoundingBox::center)
		.property("prevCenter", &Luden::CBoundingBox::prevCenter)
		.property("blockMove", &Luden::CBoundingBox::blockMove)
		(
			metadata("default", false)
		)
		.property("blockVision", &Luden::CBoundingBox::blockVision)
		(
			metadata("default", false)
		);

	registration::class_<Luden::CAnimation>("CAnimation")
		.constructor<>()
		.constructor<Luden::Graphics::Animation, bool>()
		.property("animation", &Luden::CAnimation::animation)
		.property("repeat", &Luden::CAnimation::repeat)
		(
			metadata("default", false)
		);

	registration::class_<Luden::CInvincibility>("CInvincibility")
		.constructor<>()
		.constructor<int>()
		.property("iframes", &Luden::CInvincibility::iframes)
		(
			metadata("default", 1)
		);

	registration::class_<Luden::CLifespan>("CLifespan")
		.constructor<>()
		.constructor<int, int>()
		.property("lifespan", &Luden::CLifespan::lifespan)
		(
			metadata("default", 0)
		)
		.property("frameCreated", &Luden::CLifespan::frameCreated)
		(
			metadata("default", 0)
		);

	registration::class_<Luden::CPatrol>("CPatrol")
		.constructor<>()
		.constructor<std::vector<Luden::Math::Vec2>&, float>()
		.property("positions", &Luden::CPatrol::positions)
		.property("currentPosition", &Luden::CPatrol::currentPosition)
		(
			metadata("default", 0)
		);

	registration::class_<Luden::CState>("CState")
		.constructor<>()
		.constructor<std::string>()
		.property("state", &Luden::CState::state)
		(
			metadata("default", "stand")
		)
		.property("previousState", &Luden::CState::previousState)
		(
			metadata("default", "stand")
		);

	registration::class_<Luden::CTransform>("CTransform")
		.constructor<>()
		.constructor<Luden::Math::Vec2&>()
		.constructor<Luden::Math::Vec2&, Luden::Math::Vec2&, Luden::Math::Vec2&, float>()
		.property("pos", &Luden::CTransform::pos)
		(
			metadata("default", Luden::Math::Vec2{ 0.0f, 0.0f })
		)
		.property("prevPos", &Luden::CTransform::prevPos)
		(
			metadata("default", Luden::Math::Vec2{ 0.0f, 0.0f })
		)
		.property("velocity", &Luden::CTransform::velocity)
		(
			metadata("default", Luden::Math::Vec2{ 0.0f, 0.0f })
		)
		.property("scale", &Luden::CTransform::scale)
		(
			metadata("default", Luden::Math::Vec2{ 1.0f, 1.0f })
		)
		.property("facing", &Luden::CTransform::facing)
		(
			metadata("default", Luden::Math::Vec2{ 0.0f, 1.0f })
		)
		.property("angle", &Luden::CTransform::angle)
		(
			metadata("default", 0.0f)
		);

	// Assets
	registration::class_<Luden::Assets>("Assets")
		.constructor<>()
		.property("m_Textures", &Luden::Assets::m_Textures)
		.property("m_Fonts", &Luden::Assets::m_Fonts)
		.property("m_SoundBuffers", &Luden::Assets::m_SoundBuffers)
		.property("m_Sounds", &Luden::Assets::m_Sounds)
		.property("m_Animations", &Luden::Assets::m_Animations);

	// Action
	registration::class_<Luden::Action>("Action")
		.constructor<>()
		.constructor<std::string, std::string>()
		.constructor<std::string, Luden::Math::Vec2>()
		.constructor<std::string, std::string, Luden::Math::Vec2>()
		.property("m_Name", &Luden::Action::m_Name)
		.property("m_Type", &Luden::Action::m_Type)
		.property("m_Pos", &Luden::Action::m_Pos);

	// Entity
	registration::class_<Luden::Entity>("Entity")
		.property("m_ID", &Luden::Entity::m_ID);

	// EntityManager
	registration::class_<Luden::EntityManager>("EntityManager")
		.property("m_NumEntities", &Luden::EntityManager::m_NumEntities)
		.property("m_Tags", &Luden::EntityManager::m_Tags)
		.property("m_Active", &Luden::EntityManager::m_Active);

	// GameEngine
	registration::class_<Luden::GameEngine>("GameEngine")
		.method("GetAssets", &Luden::GameEngine::GetAssets)
		.property("m_DeltaClock", &Luden::GameEngine::m_DeltaClock)
		.property("m_Assets", &Luden::GameEngine::m_Assets)
		.property("m_CurrentSceneName", &Luden::GameEngine::m_CurrentSceneName)
		.property("m_SceneMap", &Luden::GameEngine::m_SceneMap)
		.property("m_IsRunning", &Luden::GameEngine::m_IsRunning);

	// Scene
	registration::class_<Luden::Scene>("Scene")
		.property("m_Game", &Luden::Scene::m_Game)
		.property("m_ActionMap", &Luden::Scene::m_ActionMap)
		.property("m_Paused", &Luden::Scene::m_Paused)
		.property("m_HasEnded", &Luden::Scene::m_HasEnded)
		.property("m_CurrentFrame", &Luden::Scene::m_CurrentFrame);

	// Scene_Menu
	registration::class_<Luden::Scene_Menu>("Scene_Menu")
		.constructor<Luden::GameEngine*>()
		//.property("m_MenuText", &Luden::Scene_Menu::m_MenuText)
		.property("m_MenuStrings", &Luden::Scene_Menu::m_MenuStrings)
		//.property("m_MenuItems", &Luden::Scene_Menu::m_MenuItems)
		.property("m_Title", &Luden::Scene_Menu::m_Title)
		.property("m_SelectedMenuIndex", &Luden::Scene_Menu::m_SelectedMenuIndex)
		.property("m_LevelPaths", &Luden::Scene_Menu::m_LevelPaths);

	// Scene_Zelda
	registration::class_<Luden::Scene_Zelda>("Scene_Zelda")
		.constructor<Luden::GameEngine*, std::string&>()
		.property("m_LevelPath", &Luden::Scene_Zelda::m_LevelPath)
		.property("m_GridText", &Luden::Scene_Zelda::m_GridText)
		.property("m_MousePos", &Luden::Scene_Zelda::m_MousePos)
		.property("m_GridSize", &Luden::Scene_Zelda::m_GridSize)
		.property("m_DrawTextures", &Luden::Scene_Zelda::m_DrawTextures)
		.property("m_DrawCollision", &Luden::Scene_Zelda::m_DrawCollision)
		.property("m_DrawGrid", &Luden::Scene_Zelda::m_DrawGrid)
		.property("m_Follow", &Luden::Scene_Zelda::m_Follow)
		.property("m_PlayerConfig", &Luden::Scene_Zelda::m_PlayerConfig);

	// ISystem
	registration::class_<Luden::ISystem>("ISystem")
		.method("Update", &Luden::ISystem::Update);
}
