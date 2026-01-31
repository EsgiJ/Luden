#include "MainMenu.h"
#include "ScriptAPI/GameplayAPI.h"
#include "ScriptAPI/AudioAPI.h"
#include "UIButton.h"

#include <iostream>

namespace Luden
{
    void MainMenu::OnCreate()
    {
        m_BackgroundMusic = GetResource<Music>("MainMenuMusic");
        if (m_BackgroundMusic)
        {
            SoundAPI::PlayMusic(m_BackgroundMusic, true);
        }

        m_PlayButton = GameplayAPI::FindEntityWithTag("PlayButton");
        m_SettingsButton = GameplayAPI::FindEntityWithTag("SettingsButton");
        m_QuitButton = GameplayAPI::FindEntityWithTag("QuitButton");

        if (m_PlayButton.IsValid())
        {
            auto playScript = GameplayAPI::GetScript<UIButton>(m_PlayButton);
            if (playScript)
            {
                playScript->OnClick = [this]() { OnPlayClicked(); };
            }
        }

        if (m_SettingsButton.IsValid())
        {
            auto settingsScript = GameplayAPI::GetScript<UIButton>(m_SettingsButton);
            if (settingsScript)
            {
                settingsScript->OnClick = [this]() { OnSettingsClicked(); };
            }
        }
        if (m_QuitButton.IsValid())
        {
            auto quitScript = GameplayAPI::GetScript<UIButton>(m_QuitButton);
            if (quitScript)
            {
                quitScript->OnClick = [this]() { OnQuitClicked(); };
            }
        }
    }

    void MainMenu::OnUpdate(TimeStep ts)
    {
        if (m_PlayButton.IsValid())
        {
            auto playScript = GameplayAPI::GetScript<UIButton>(m_PlayButton);
            if (playScript)
            {
                playScript->OnClick = [this]() { OnPlayClicked(); };
            }
        }

        if (m_SettingsButton.IsValid())
        {
            auto settingsScript = GameplayAPI::GetScript<UIButton>(m_SettingsButton);
            if (settingsScript)
            {
                settingsScript->OnClick = [this]() { OnSettingsClicked(); };
            }
        }
        if (m_QuitButton.IsValid())
        {
            auto quitScript = GameplayAPI::GetScript<UIButton>(m_QuitButton);
            if (quitScript)
            {
                quitScript->OnClick = [this]() { OnQuitClicked(); };
            }
        }
    }

    void MainMenu::OnDestroy()
    {
        SoundAPI::StopMusic();
    }

    void MainMenu::OnCollisionBegin(const CollisionContact& contact)
    {
        // TODO: On contact begin
    }

    void MainMenu::OnCollisionEnd(const CollisionContact& contact)
    {
        // TODO: On contact end
    }

    void MainMenu::OnCollisionHit(const CollisionContact& contact)
    {
        // TODO: On hit(high speed)
    }

    void MainMenu::OnPlayClicked()
    {
        std::cout << "[MainMenu] Starting game..." << std::endl;
        GameplayAPI::ChangeScene(m_StartSceneName);
    }

    void MainMenu::OnSettingsClicked()
    {
        std::cout << "[MainMenu] Opening settings..." << std::endl;
        GameplayAPI::ChangeScene(m_SettingsSceneName);
    }

    void MainMenu::OnQuitClicked()
    {
        std::cout << "[MainMenu] Quitting game..." << std::endl;
        GameplayAPI::QuitGame();
    }
}