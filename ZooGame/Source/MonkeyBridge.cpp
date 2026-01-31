#include "MonkeyBridge.h"

#include <iostream>
#include <ScriptAPI/GameplayAPI.h>
#include <ScriptAPI/MathAPI.h>

namespace Luden
{
    void MonkeyBridge::OnCreate()
    {
        StartRotation = GameplayAPI::GetRotation(GetEntity());
    }

    void MonkeyBridge::OnUpdate(TimeStep ts)
    {
        if (IsActivated)
        {
            RotateToTarget(ts);
        }
        else if (IsRotated)
        {
            RotateToStart(ts);
        }
    }

    void MonkeyBridge::OnDestroy()
    {
        // TODO: Cleanup
    }

    void MonkeyBridge::OnCollisionBegin(const CollisionContact& contact)
    {
        // TODO: On contact begin
    }

    void MonkeyBridge::OnCollisionEnd(const CollisionContact& contact)
    {
        // TODO: On contact end
    }

    void MonkeyBridge::OnCollisionHit(const CollisionContact& contact)
    {
        // TODO: On hit(high speed)
    }

    void MonkeyBridge::Activate()
    {
        if (IsActivated)
            return;

        IsActivated = true;
        std::cout << "[MonkeyBridge] Activating - rotating to " << TargetRotation << " degrees" << std::endl;

        //TODO: Play rotation sound
    }

    void MonkeyBridge::Deactivate()
    {
        IsActivated = false;
        std::cout << "[MonkeyBridge] Deactivating - returning to start" << std::endl;
    }

    void MonkeyBridge::RotateToTarget(TimeStep ts)
    {
        float currentRotation = GameplayAPI::GetRotation(GetEntity());

        float newRotation = MathAPI::Lerp(currentRotation, TargetRotation, RotateSpeed * ts);
        GameplayAPI::SetRotation(GetEntity(), newRotation);

        if (abs(newRotation - TargetRotation) < 1.0f)
        {
            GameplayAPI::SetRotation(GetEntity(), TargetRotation);
            IsRotated = true;
            std::cout << "[MonkeyBridge] Rotation complete!" << std::endl;
        }
    }

    void MonkeyBridge::RotateToStart(TimeStep ts)
    {
        float currentRotation = GameplayAPI::GetRotation(GetEntity());

        float newRotation = MathAPI::Lerp(currentRotation, StartRotation, RotateSpeed * ts);
        GameplayAPI::SetRotation(GetEntity(), newRotation);

        if (abs(newRotation - StartRotation) < 1.0f)
        {
            GameplayAPI::SetRotation(GetEntity(), StartRotation);
            IsRotated = false;
        }
    }
}
