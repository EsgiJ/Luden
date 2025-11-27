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

        void OnJump(const InputValue& value);
        void OnMove(const InputValue& value);
    private:
        InputAction m_MoveAction;
    };
}
