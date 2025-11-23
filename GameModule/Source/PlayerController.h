#pragma once
#include "NativeScript/ScriptableEntity.h"

namespace Luden
{
	class PlayerController : public ScriptableEntity
    {
    public:
        virtual void OnCreate() override;
        virtual void OnUpdate(TimeStep ts) override;
        virtual void OnDestroy() override;

    private:
        InputAction m_MoveAction;
    };
}
