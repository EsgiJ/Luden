#include "AudioSlider.h"
#include <ScriptAPI/MathAPI.h>
#include "ScriptAPI/GameplayAPI.h"
#include "Audio/AudioManager.h"

namespace Luden
{
    void AudioSlider::OnCreate()
    {
        DetectSliderType();

        auto children = GameplayAPI::GetChildren(GetEntity());

        for (auto child : children)
        {
            if (child.Tag() == "SliderBar")
                m_SliderBar = child;
            else if (child.Tag() == "SliderHandle")
                m_SliderHandle = child;  
            else if (child.Tag() == "ValueText")
                m_ValueText = child;
        }

        if (m_SliderBar.IsValid())
        {
            m_SliderBarSize = GameplayAPI::GetEntitySize(m_SliderBar);
            m_SliderBarPosition = GameplayAPI::GetPosition(m_SliderBar);
        }

        m_CurrentValue = 50.0f;

        UpdateHandlePosition();
        UpdateValueDisplay();
        ApplyVolumeChange();
    }

    void AudioSlider::OnUpdate(TimeStep ts)
    {
        if (!m_SliderBar.IsValid() || !m_SliderHandle.IsValid())
            return;

        Vec2 mousePos = GameplayAPI::GetMousePosition();

        if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
        {
            Vec3 handlePos = GameplayAPI::GetPosition(m_SliderHandle);
            Vec2 handleSize = GameplayAPI::GetEntitySize(m_SliderHandle);

            if (GameplayAPI::IsPointInRect(mousePos, handlePos, handleSize))
            {
                m_IsDragging = true;
            }
        }

        if (!sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
        {
            m_IsDragging = false;
        }

        if (m_IsDragging)
        {
            float halfWidth = m_SliderBarSize.x * 0.5f;
            float minX = m_SliderBarPosition.x - halfWidth;
            float maxX = m_SliderBarPosition.x + halfWidth;

            float clampedX = MathAPI::Clamp(mousePos.x, minX, maxX);

            float t = (clampedX - minX) / (maxX - minX);

            m_CurrentValue = MathAPI::Lerp(m_MinValue, m_MaxValue, t);

            UpdateHandlePosition();
            UpdateValueDisplay();
            ApplyVolumeChange();
        }
    }

    void AudioSlider::OnDestroy()
    {

    }

    void AudioSlider::OnCollisionBegin(const CollisionContact& contact)
    {
        // TODO: On contact begin
    }

    void AudioSlider::OnCollisionEnd(const CollisionContact& contact)
    {
        // TODO: On contact end
    }

    void AudioSlider::OnCollisionHit(const CollisionContact& contact)
    {
        // TODO: On hit(high speed)
    }

    void AudioSlider::UpdateHandlePosition()
    {
        if (!m_SliderHandle.IsValid() || !m_SliderBar.IsValid())
            return;

        float t = (m_CurrentValue - m_MinValue) / (m_MaxValue - m_MinValue);
        float halfWidth = m_SliderBarSize.x * 0.5f;

        float xPos = m_SliderBarPosition.x - halfWidth + (t * m_SliderBarSize.x);

        GameplayAPI::SetPosition(m_SliderHandle, Vec3(xPos, m_SliderBarPosition.y, 10.0f));
    }

    void AudioSlider::UpdateValueDisplay()
    {
        if (!m_ValueText.IsValid())
            return;

        if (m_ValueText.Has<TextComponent>())
        {
            auto& text = m_ValueText.Get<TextComponent>();
            int displayValue = (int)m_CurrentValue;
            text.text = std::to_string(displayValue) + "%";
        }
    }

    void AudioSlider::ApplyVolumeChange()
    {
        float normalizedVolume = m_CurrentValue / 100.0f;

        switch (m_Type)
        {
        case AudioType::Master:
            AudioManager::Instance().SetMasterVolume(normalizedVolume * 100.0f);
            break;
        case AudioType::Music:
            AudioManager::Instance().SetMusicVolume(normalizedVolume * 100.0f);
            break;
        case AudioType::SFX:
            AudioManager::Instance().SetSoundVolume(normalizedVolume * 100.0f);
            break;
        }
    }

    void AudioSlider::DetectSliderType()
    {
        auto tag = GetEntity().Tag();

        if (tag == "MusicSlider")
            m_Type = AudioType::Music;
        else if (tag == "SFXSlider")
            m_Type = AudioType::SFX;
        else
            m_Type = AudioType::Master;
    }
}