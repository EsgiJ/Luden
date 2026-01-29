#include "MenuPlayerController.h"

#include <iostream>

#include "ScriptAPI/GameplayAPI.h"

namespace Luden
{
    void MenuPlayerController::OnCreate()
    {
        // TODO: Initialize
    }

    void MenuPlayerController::OnUpdate(TimeStep ts)
    {
        Entity clickableEntity = GameplayAPI::GetEntityUnderMouse();

        std::cout << "Entity Tag: " << clickableEntity.Tag() << std::endl;
	    if (clickableEntity.IsValid())
	    {
		    if (clickableEntity.Tag() == "StartGame")
		    {
                GameplayAPI::ChangeScene("MainScene");
		    }
            else if (clickableEntity.Tag() == "Quit")
		    {
                GameplayAPI::QuitGame();
		    }
	    }
    }

    void MenuPlayerController::OnDestroy()
    {
        // TODO: Cleanup
    }

    void MenuPlayerController::OnCollisionBegin(const CollisionContact& contact)
    {
        // TODO: On contact begin
    }

    void MenuPlayerController::OnCollisionEnd(const CollisionContact& contact)
    {
        // TODO: On contact end
    }

    void MenuPlayerController::OnCollisionHit(const CollisionContact& contact)
    {
        // TODO: On hit(high speed)
    }

}
