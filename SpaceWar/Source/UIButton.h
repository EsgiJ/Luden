#pragma once
#include "Luden.h"

namespace Luden
{
	class UIButton : public ScriptableEntity
    {
    public:
        virtual void OnCreate() override;
        virtual void OnUpdate(TimeStep ts) override;
        virtual void OnDestroy() override;
        virtual void OnCollisionBegin(const CollisionContact& contact) override;
        virtual void OnCollisionEnd(const CollisionContact& contact) override;
        virtual void OnCollisionHit(const CollisionContact& contact) override;

	public:
        Function OnClick;

        sf::Color SpriteNormalColor = sf::Color::White;
        sf::Color SpriteHoverColor = sf::Color(200, 200, 200);
        sf::Color SpritePressedColor = sf::Color(150, 150, 150);

        sf::Color TextNormalFillColor = sf::Color::White;
        sf::Color TextHoverFillColor = sf::Color(200, 200, 200);
        sf::Color TextPressedFillColor = sf::Color(150, 150, 150);

        sf::Color TextNormalOutlineColor = sf::Color::Black;
        sf::Color TextHoverOutlineColor = sf::Color::Black;
        sf::Color TextPressedOutlineColor = sf::Color::Black;

        SoundRef m_ClickSound;
        SoundRef m_HoverSound;
    private:
        bool m_IsHovered = false;
        bool m_WasHovered = false;
        bool m_IsPressed = false;
    };
}
