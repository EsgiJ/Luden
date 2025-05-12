#include "Scene/Scene_Menu.h"

#include <iostream>

#include <SFML/Graphics/View.hpp>
#include <SFML/Window.hpp>

#include "Core/GameEngine.h"
#include "Scene/Scene_Zelda.h"

namespace Luden 
{

	Scene_Menu::Scene_Menu(GameEngine* game)
		: Scene(game),
		m_TitleMusic(m_Game->GetAssets().GetSound("STitleTheme")),
		m_MenuText(m_Game->GetAssets().GetFont("Mario"), "")
	{
		Init();
	}

	void Scene_Menu::Init() 
	{
		sf::View view = m_Game->GetWindow().getView();
		view.setCenter(sf::Vector2f(
			static_cast<float>(m_Game->GetWindow().getSize().x) / 2.0f,
			static_cast<float>(m_Game->GetWindow().getSize().y) / 2.0f)
		);
		m_Game->GetWindow().setView(view);

		RegisterAction(sf::Keyboard::Key::W, "UP");
		RegisterAction(sf::Keyboard::Key::S, "DOWN");
		RegisterAction(sf::Keyboard::Key::D, "PLAY");
		RegisterAction(sf::Keyboard::Key::M, "MUTE");
		RegisterAction(sf::Keyboard::Key::Escape, "QUIT");

		m_TitleMusic = m_Game->GetAssets().GetSound("STitleTheme");
		m_TitleMusic.setLooping(true);
		m_TitleMusic.play();

		m_Title = "Lunatics";
		int titleSize = 30;
		m_MenuText.setString(m_Title);
		m_MenuText.setFont(m_Game->GetAssets().GetFont("Mario"));
		m_MenuText.setCharacterSize(titleSize);
		m_MenuText.setFillColor(sf::Color::Black);
		m_MenuText.setPosition(sf::Vector2f(
			float(m_Game->GetWindow().getSize().x) / 2.0f - float(titleSize * (m_Title.length() + 1)) / 2.0f,
			float(titleSize * 3)
		));

		m_MenuStrings.emplace_back("LEVEL 1");
		m_MenuStrings.emplace_back("LEVEL 2");
		m_MenuStrings.emplace_back("LEVEL 3");

		m_LevelPaths.emplace_back("config/level1.txt");
		m_LevelPaths.emplace_back("config/level2.txt");
		m_LevelPaths.emplace_back("config/level3.txt");

		for (const auto& menuString: m_MenuStrings) 
		{
			sf::Text text(m_Game->GetAssets().GetFont("Mario"), menuString, 26);
			text.setFillColor(sf::Color::Black);
			m_MenuItems.push_back(text);
		}
	}

	void Scene_Menu::Update() 
	{
		sRender();
	}

	void Scene_Menu::OnEnd() 
	{
		m_Game->Quit();
	}

	void Scene_Menu::sDoAction(const Action& action) 
	{
		if (action.Type() == "START") 
		{
			if (action.Name() == "UP") 
			{
				m_SelectedMenuIndex = (m_SelectedMenuIndex > 0) ? m_SelectedMenuIndex - 1 : static_cast<int>(m_MenuStrings.size()) - 1;
			}
			else if (action.Name() == "DOWN") 
			{
				m_SelectedMenuIndex = (m_SelectedMenuIndex + 1) % m_MenuStrings.size();
			}
			else if (action.Name() == "PLAY") 
			{
				m_TitleMusic.stop();
				m_Game->ChangeScene("PLAY", std::make_shared<Scene_Zelda>(m_Game, m_LevelPaths[m_SelectedMenuIndex]));
			}
			else if (action.Name() == "MUTE") 
			{
				if (m_TitleMusic.getStatus() == sf::SoundSource::Status::Playing) 
				{
					m_TitleMusic.stop();
				}
				else 
				{
					m_TitleMusic.play();
				}
			}
			else if (action.Name() == "QUIT") {
				OnEnd();
			}
		}
	}

	void Scene_Menu::sRender()
	{
		m_Game->GetWindow().clear(sf::Color(100, 100, 255));
		m_Game->GetWindow().draw(m_MenuText);

		for (size_t i = 0; i < m_MenuItems.size(); i++) 
		{
			auto& item = m_MenuItems[i];
			item.setFillColor(i == m_SelectedMenuIndex ? sf::Color::White : sf::Color::Black);
			item.setPosition(sf::Vector2f(
				float(m_Game->GetWindow().getSize().x) / 2.0f - float(26 * (m_MenuStrings[i].length() + 1)) / 2.0f,
				m_MenuText.getGlobalBounds().position.x + 10.0f + 30.0f * float(i + 1)
			));
			m_Game->GetWindow().draw(item);
		}

		sf::Text help(m_Game->GetAssets().GetFont("Mario"), "W:UP  S:DOWN  D:PLAY  M:MUTE  ESC:QUIT", 20);
		help.setFillColor(sf::Color::Black);
		help.setPosition(sf::Vector2f(20, float(m_Game->GetWindow().getSize().y) - 40));
		m_Game->GetWindow().draw(help);
	}
}
