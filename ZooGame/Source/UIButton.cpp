#include "UIButton.h"

#include <iostream>
#include <ScriptAPI/AudioAPI.h>
#include <ScriptAPI/GameplayAPI.h>

namespace Luden
{
    void UIButton::OnCreate()
    {
        m_ClickSound = GetResource<Sound>("ButtonClickSound");
        m_HoverSound = GetResource<Sound>("ButtonHoverSound");

        if (GetEntity().Has<SpriteRendererComponent>())
        {
            auto& sprite = GetEntity().Get<SpriteRendererComponent>();
            sprite.tint = SpriteNormalColor;
        }

        if (GetEntity().Has<TextComponent>())
        {
            auto& text = GetEntity().Get<TextComponent>();
            text.fillColor = TextNormalFillColor;
            text.outlineColor = TextNormalOutlineColor;
        }
    }

    void UIButton::OnUpdate(TimeStep)
    {
        Vec2 mousePos = GameplayAPI::GetMousePosition();
        Vec3 buttonPos = GameplayAPI::GetPosition(GetEntity());
        Vec2 buttonSize = GameplayAPI::GetEntitySize(GetEntity());

        m_IsHovered = GameplayAPI::IsPointInRect(mousePos, buttonPos, buttonSize);

        if (m_IsHovered && !m_WasHovered && m_HoverSound)
            SoundAPI::PlaySound(m_HoverSound);

        m_WasHovered = m_IsHovered;

        if (GetEntity().Has<SpriteRendererComponent>())
        {
            auto& sprite = GetEntity().Get<SpriteRendererComponent>();

            if (m_IsPressed && m_IsHovered)
                sprite.tint = SpritePressedColor;
            else if (m_IsHovered)
                sprite.tint = SpriteHoverColor;
            else
                sprite.tint = SpriteNormalColor;
        }

        if (GetEntity().Has<TextComponent>())
        {
            auto& text = GetEntity().Get<TextComponent>();

            if (m_IsPressed && m_IsHovered)
            {
                text.fillColor = TextPressedFillColor;
                text.outlineColor = TextPressedOutlineColor;

                text.style |= sf::Text::Style::Bold;
                text.style |= sf::Text::Style::StrikeThrough;
            }
            else if (m_IsHovered)
            {
                text.fillColor = TextHoverFillColor;
                text.outlineColor = TextHoverOutlineColor;

                text.style |= sf::Text::Style::Bold;
            }
            else
            {
                text.fillColor = TextNormalFillColor;
                text.outlineColor = TextNormalOutlineColor;

                text.style &= ~sf::Text::Style::Bold;
                text.style &= ~sf::Text::Style::StrikeThrough;
            }
        }

        if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && m_IsHovered)
        {
            m_IsPressed = true;
        }

        if (!sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
        {
            if (m_IsPressed && m_IsHovered)
            {
                if (m_ClickSound)
                    SoundAPI::PlaySound(m_ClickSound);

                std::cout << "OnClick..." << std::endl;
                if (OnClick)
                {
                    std::cout << "OnClickTriggered!" << std::endl;
                    OnClick();
                }
            }
            m_IsPressed = false;
        }
    }

    void UIButton::OnDestroy()
    {
        // TODO: Cleanup
    }

    void UIButton::OnCollisionBegin(const CollisionContact& contact)
    {
        // TODO: On contact begin
    }

    void UIButton::OnCollisionEnd(const CollisionContact& contact)
    {
        // TODO: On contact end
    }

    void UIButton::OnCollisionHit(const CollisionContact& contact)
    {
        // TODO: On hit(high speed)
    }

}
