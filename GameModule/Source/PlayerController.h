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

		virtual void OnCollisionBegin(const CollisionContact& contact) override;
		virtual void OnCollisionEnd(const CollisionContact& contact) override;
		virtual void OnCollisionHit(const CollisionContact& contact) override;

        void OnJump(const InputValue& value);
        void OnMove(const InputValue& value);
        void OnMoveStop(const InputValue& value);
        void OnShoot(const InputValue& value);
    private:
        InputAction m_MoveAction;

        glm::vec2 m_CurrentMoveInput = glm::vec2(0.0f);
    };
}
