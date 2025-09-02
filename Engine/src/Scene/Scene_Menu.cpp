#include "Scene/Scene_Menu.h"

#include <iostream>

#include <SFML/Graphics/View.hpp>
#include <SFML/Window.hpp>

#include "Core/GameEngine.h"

namespace Luden 
{

	Scene_Menu::Scene_Menu()
	{
		Init();
	}

	void Scene_Menu::Init() 
	{
		m_View = GameEngine::Get().GetWindow().getDefaultView();

		RegisterAction(sf::Keyboard::Key::W, "UP");
		RegisterAction(sf::Keyboard::Key::S, "DOWN");
		RegisterAction(sf::Keyboard::Key::D, "PLAY");
		RegisterAction(sf::Keyboard::Key::M, "MUTE");
		RegisterAction(sf::Keyboard::Key::Escape, "QUIT");

		/*
		m_TitleMusic = GameEngine::Get().GetResources().GetSound("STitleTheme");
		m_TitleMusic.setLooping(true);
		m_TitleMusic.play();

		m_Title = "Lunatics";
		m_MenuText.setString(m_Title);
		m_MenuText.setFont(GameEngine::Get().GetResources().GetFont("Mario"));
		m_MenuText.setFillColor(sf::Color::Black);
		*/
		m_MenuStrings.emplace_back("LEVEL 1");
		m_MenuStrings.emplace_back("LEVEL 2");
		m_MenuStrings.emplace_back("LEVEL 3");

		m_LevelPaths.emplace_back("config/level1.txt");
		m_LevelPaths.emplace_back("config/level2.txt");
		m_LevelPaths.emplace_back("config/level3.txt");

		/*
		for (const auto& menuString: m_MenuStrings) 
		{
			sf::Text text(GameEngine::Get().GetResources().GetFont("Mario"), menuString, 26);
			text.setFillColor(sf::Color::Black);
			m_MenuItems.push_back(text);
		}
		*/
	}

	void Scene_Menu::Update() 
	{
		m_EntityManager.Update();
	}

	void Scene_Menu::OnEnd() 
	{
		GameEngine::Get().Quit();
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
				//m_TitleMusic.stop();
			}
			else if (action.Name() == "MUTE") 
			{
				/*
				if (m_TitleMusic.getStatus() == sf::SoundSource::Status::Playing) 
				{
					m_TitleMusic.stop();
				}
				else 
				{
					m_TitleMusic.play();
				}
				*/
			}
			else if (action.Name() == "QUIT") {
				OnEnd();
			}
		}
	}

	void Scene_Menu::sRender(sf::RenderTarget& target)
	{
        target.clear(sf::Color(100, 100, 255));

        // keep title centered relative to the viewport each frame
        /*
		int titleSize = m_MenuText.getCharacterSize();
        m_MenuText.setPosition(sf::Vector2f(
                float(target.getSize().x) / 2.0f - float(titleSize * (m_Title.length() + 1)) / 2.0f,
				target.getSize().y / 2.0f
        ));
        target.draw(m_MenuText);

		for (size_t i = 0; i < m_MenuItems.size(); i++)
		{
			auto& item = m_MenuItems[i];
			item.setFillColor(i == m_SelectedMenuIndex ? sf::Color::White : sf::Color::Black);
                        item.setPosition(sf::Vector2f(
                                float(target.getSize().x) / 2.0f - float(item.getCharacterSize() * (m_MenuStrings[i].length() + 1)) / 2.0f,
                                m_MenuText.getGlobalBounds().position.y + 10.0f + 30.0f * float(i + 1)
                        ));
			target.draw(item);
		}
		*/
		/*
		sf::Text help(GameEngine::Get().GetResources().GetFont("Mario"), "W:UP  S:DOWN  D:PLAY  M:MUTE  ESC:QUIT", 20);
		help.setFillColor(sf::Color::Black);
        help.setPosition(sf::Vector2f(20, float(target.getSize().y) - 40));
		target.draw(help);
		*/
	}
}
