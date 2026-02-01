#include "CreditsGameManager.h"

#include <ScriptAPI/AudioAPI.h>
#include <ScriptAPI/GameplayAPI.h>

namespace Luden
{
    void CreditsGameManager::OnCreate()
    {
        m_CreditsMusic = GetResource<Music>("Credits");
        if (m_CreditsMusic != nullptr)
        {
            SoundAPI::PlayMusic(m_CreditsMusic, 75.0f, false);
        }
        GameplayAPI::SetWorldGravity({0.0f, 0.0f});

        creditsEntity = GameplayAPI::FindEntityWithTag("Credits");
    }

    void CreditsGameManager::OnUpdate(TimeStep ts)
    {
	    if (creditsEntity.IsValid())
	    {
            Vec3 creditsPos = GameplayAPI::GetPosition(creditsEntity);
            creditsPos = Vec3(creditsPos.x, (creditsPos.y + ts * -m_CreditsSpeed), 0.0f);
            GameplayAPI::SetPosition(creditsEntity, creditsPos);
	    }
    }

    void CreditsGameManager::OnDestroy()
    {
        SoundAPI::PauseMusic();
    }

    void CreditsGameManager::OnCollisionBegin(const CollisionContact& contact)
    {
        // TODO: On contact begin
    }

    void CreditsGameManager::OnCollisionEnd(const CollisionContact& contact)
    {
        // TODO: On contact end
    }

    void CreditsGameManager::OnCollisionHit(const CollisionContact& contact)
    {
        // TODO: On hit(high speed)
    }

}
