#pragma once
#include "Scene/Scene.h"
#include "Input/Action.h"
#include <SFML/Graphics/Text.hpp>
#include <SFML/Audio/Sound.hpp>

namespace Luden
{
	class Scene_Menu : public Luden::Scene {
		sf::Text m_MenuText;
		std::vector<std::string> m_MenuStrings;
		std::vector<sf::Text> m_MenuItems;
		sf::Sound m_TitleMusic;
		std::string m_Title;
		int m_SelectedMenuIndex = 0;

		std::vector<std::string> m_LevelPaths;

		void Init();

	public:
		Scene_Menu(Luden::GameEngine* game);

		void sRender() override;
		void sDoAction(const Action& action) override;
		void Update() override;
		void OnEnd() override;
	};
}

