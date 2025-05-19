#pragma once

#include "Input/Action.h"
#include "Scene/Scene.h"

#include <SFML/Audio/Sound.hpp>
#include <SFML/Graphics/Text.hpp>
#include <rttr/rttr_enable.h>
#include <rttr/registration_friend.h>

namespace Luden
{
	class ENGINE_API Scene_Menu : public Luden::Scene 
	{
		RTTR_ENABLE(Scene)
		RTTR_REGISTRATION_FRIEND

		sf::Text m_MenuText;
		std::vector<std::string> m_MenuStrings;
		std::vector<sf::Text> m_MenuItems;
		sf::Sound& m_TitleMusic;
		std::string m_Title;
		int m_SelectedMenuIndex = 0;

		std::vector<std::string> m_LevelPaths;

		void Init();

	public:
		Scene_Menu(Luden::GameEngine* game);

		void sRender(sf::RenderTarget& target) override;
		void sDoAction(const Action& action) override;
		void Update() override;
		void OnEnd() override;
	};
}

