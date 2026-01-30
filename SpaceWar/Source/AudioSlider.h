#pragma once
#include "Luden.h"

namespace Luden
{
    class AudioSlider : public ScriptableEntity
    {
    public:
        enum class AudioType
        {
            Master,
            Music,
            SFX
        };

        AudioType m_Type = AudioType::Master;
        float m_MinValue = 0.0f;
        float m_MaxValue = 100.0f;
        float m_CurrentValue = 50.0f;

        Entity m_SliderBar;      
        Entity m_SliderHandle;   
        Entity m_ValueText;      

        void OnCreate() override;
        void OnUpdate(TimeStep ts) override;
        void OnDestroy() override;
        virtual void OnCollisionBegin(const CollisionContact& contact) override;
        virtual void OnCollisionEnd(const CollisionContact& contact) override;
        virtual void OnCollisionHit(const CollisionContact& contact) override;
    private:
        void UpdateHandlePosition();
        void UpdateValueDisplay();
        void ApplyVolumeChange();
        void DetectSliderType();

        bool m_IsDragging = false;
        Vec2 m_SliderBarSize;
        Vec3 m_SliderBarPosition;
    };
}