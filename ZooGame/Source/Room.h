#pragma once
#include "Luden.h"

namespace Luden
{
	class Room : public ScriptableEntity
    {
    public:
        virtual void OnCreate() override;
        virtual void OnUpdate(TimeStep ts) override;
        virtual void OnDestroy() override;
        virtual void OnCollisionBegin(const CollisionContact& contact) override;
        virtual void OnCollisionEnd(const CollisionContact& contact) override;
        virtual void OnCollisionHit(const CollisionContact& contact) override;

        Vec2 m_RoomSize = {1920.0f, 1080.0f};
        Vec2 m_RoomPostion = { 0.0f, 0.0f };

        bool m_Overlap = false;
	private:
    };
}
