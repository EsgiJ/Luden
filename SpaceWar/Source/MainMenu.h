#pragma once
#include "Luden.h"

namespace Luden
{
	class MainMenu : public ScriptableEntity
    {
    public:
        virtual void OnCreate() override;
        virtual void OnUpdate(TimeStep ts) override;
        virtual void OnDestroy() override;
        virtual void OnCollisionBegin(const CollisionContact& contact) override;
        virtual void OnCollisionEnd(const CollisionContact& contact) override;
        virtual void OnCollisionHit(const CollisionContact& contact) override;

	public:
        String m_StartSceneName = "MainScene";
        String m_SettingsSceneName = "Settings";

        MusicRef m_BackgroundMusic;

    private:
        void OnPlayClicked();
        void OnSettingsClicked();
        void OnQuitClicked();

        Entity m_PlayButton;
        Entity m_SettingsButton;
        Entity m_QuitButton;
    };
}
