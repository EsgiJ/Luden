#pragma once
#include "Luden.h"

namespace Luden
{
	class CreditsGameManager : public ScriptableEntity
    {
    public:
        virtual void OnCreate() override;
        virtual void OnUpdate(TimeStep ts) override;
        virtual void OnDestroy() override;
        virtual void OnCollisionBegin(const CollisionContact& contact) override;
        virtual void OnCollisionEnd(const CollisionContact& contact) override;
        virtual void OnCollisionHit(const CollisionContact& contact) override;

        MusicRef m_CreditsMusic;
        Entity creditsEntity;
        float m_CreditsSpeed = 100.0f;
    };
}
