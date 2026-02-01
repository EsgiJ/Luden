#include "GameManager.h"
#include "ScriptAPI/AudioAPI.h"
#include <ScriptAPI/GameplayAPI.h>

namespace Luden
{
    void GameManager::OnCreate()
    {
        m_MainMusic = GetResource<Music>("MainMusic");
        if (m_MainMusic != nullptr)
        {
            SoundAPI::PlayMusic(m_MainMusic, 50.0f, true);
        }
        GameplayAPI::SetWorldGravity(worldGravity);
    }

    void GameManager::OnUpdate(TimeStep ts)
    {

    }

    void GameManager::OnDestroy()
    {
        if (m_MainMusic != nullptr)
        {
            SoundAPI::PauseMusic();
        }
    }

    void GameManager::OnCollisionBegin(const CollisionContact& contact)
    {
        // TODO: On contact begin
    }

    void GameManager::OnCollisionEnd(const CollisionContact& contact)
    {
        // TODO: On contact end
    }

    void GameManager::OnCollisionHit(const CollisionContact& contact)
    {
        // TODO: On hit(high speed)
    }

}
