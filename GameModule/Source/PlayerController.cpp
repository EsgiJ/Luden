#include "PlayerController.h"

namespace Luden
{
    void PlayerController::OnCreate()
    {
        // TODO: Initialize
    }

    void PlayerController::OnUpdate(TimeStep ts)
    {
        // TODO: Update logic
        GetComponent<TransformComponent>().Translation.x += 5;
    }

    void PlayerController::OnDestroy()
    {
        // TODO: Cleanup
    }
}
