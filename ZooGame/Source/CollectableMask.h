#pragma once
#include "Luden.h"
#include "Mask.h"

namespace Luden
{
    class CollectableMask : public ScriptableEntity
    {
    public:
        virtual void OnCreate() override;
        virtual void OnUpdate(TimeStep ts) override;
        virtual void OnDestroy() override;
        virtual void OnCollisionBegin(const CollisionContact& contact) override;
        virtual void OnCollisionEnd(const CollisionContact& contact) override;
        virtual void OnCollisionHit(const CollisionContact& contact) override;

    public:
        MaskType Type = MaskType::None;

        SpriteRef ElephantCollectable;
        SpriteRef RabbitCollectable;
        SpriteRef MonkeyCollectable;
    private:

    };
}