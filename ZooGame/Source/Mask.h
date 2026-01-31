#pragma once
#include "Luden.h"
#include <functional>

namespace Luden
{
    enum class MaskType : uint8_t
    {
        None = 0,
        Elephant = 1,
        Rabbit = 2,
        Monkey = 3
    };

    class Mask : public ScriptableEntity
    {
    public:
        virtual void OnCreate() override;
        virtual void OnUpdate(TimeStep ts) override;
        virtual void OnDestroy() override;
        virtual void OnCollisionBegin(const CollisionContact& contact) override;
        virtual void OnCollisionEnd(const CollisionContact& contact) override;
        virtual void OnCollisionHit(const CollisionContact& contact) override;

        MaskType m_Type = MaskType::None;

        std::function<void()> OnAbilityUse;

        float AbilityCooldown = 0.0f;
        float MaxCooldown = 2.0f;

        void UseAbility();
        bool CanUseAbility() const;

        // Animations
        AnimationRef m_EmptyAnim = nullptr;

        AnimationRef m_CurrentIdleAnim = nullptr;
        AnimationRef m_CurrentBackAnim = nullptr;
        AnimationRef m_CurrentFrontAnim = nullptr;
        AnimationRef m_CurrentSideAnim = nullptr;

        AnimationRef m_ElephantIdleAnim = nullptr;
        AnimationRef m_ElephantBackAnim = nullptr;
        AnimationRef m_ElephantFrontAnim = nullptr;
        AnimationRef m_ElephantSideAnim = nullptr;

        AnimationRef m_RabbitIdleAnim = nullptr;
        AnimationRef m_RabbitBackAnim = nullptr;
        AnimationRef m_RabbitFrontAnim = nullptr;
        AnimationRef m_RabbitSideAnim = nullptr;

        AnimationRef m_MonkeyIdleAnim = nullptr;
        AnimationRef m_MonkeyBackAnim = nullptr;
        AnimationRef m_MonkeyFrontAnim = nullptr;
        AnimationRef m_MonkeySideAnim = nullptr;

    private:
        void UpdateMaskAnimations();
    };
}