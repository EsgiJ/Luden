#pragma once
#include "Luden.h"

namespace Luden
{
	class PaddleController : public ScriptableEntity
    {
    public:
        virtual void OnCreate() override;
        virtual void OnUpdate(TimeStep ts) override;
        virtual void OnDestroy() override;
        virtual void OnCollisionBegin(const CollisionContact& contact) override;
        virtual void OnCollisionEnd(const CollisionContact& contact) override;
        virtual void OnCollisionHit(const CollisionContact& contact) override;
	private:
		void OnMoveLeft(const InputValue& value);
		void OnMoveRight(const InputValue& value);

		float m_Speed = 15.0f;
		float m_MinX = -850.0f;  
		float m_MaxX = 850.0f;
    };
}
