#include "GameManager.h"

#include "ScriptAPI/GameplayAPI.h"

namespace Luden
{
    void GameManager::OnCreate()
    {
        GameplayAPI::g
    }

    void GameManager::OnUpdate(TimeStep ts)
    {
        // TODO: Update logic
    }

    void GameManager::OnDestroy()
    {
        // TODO: Cleanup
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
