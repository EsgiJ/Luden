#pragma once

#include "ECS/IComponent.h"
#include "Graphics/Animation.h"
#include "Math/Vec2.h"
#include "Reflection/ReflectionMacros.h"
#include <rttr/registration.h>

namespace Luden
{
	struct ENGINE_API COMPONENT() CDamage : public IComponent 
	{
	public:
		PROPERTY() int damage = 1;

		CDamage() = default;

		explicit CDamage(int d) : damage(d) {}
		RTTR_ENABLE(IComponent);
	};

	struct ENGINE_API COMPONENT() CDraggable : public IComponent
	{
	public:
		PROPERTY() bool dragging = false;

		CDraggable() = default;
		RTTR_ENABLE(IComponent);
	};

	struct ENGINE_API COMPONENT() CFollowPlayer : public IComponent
	{
	public:
		PROPERTY() Math::Vec2 home = { 0.0f, 0.0f };
		PROPERTY() float speed = 0.0f;

		CFollowPlayer() = default;

		CFollowPlayer(Math::Vec2 p, float s) : home(p), speed(s) {}
		RTTR_ENABLE(IComponent);
	};

	struct ENGINE_API COMPONENT() CGravity : public IComponent
	{
	public:
		PROPERTY() float gravity = 0;

		CGravity() = default;

		explicit CGravity(float g) : gravity(g) {}
		RTTR_ENABLE(IComponent);
	};

	struct ENGINE_API COMPONENT() CHealth : public IComponent
	{
	public:
		PROPERTY() int max = 1;
		PROPERTY() int current = 1;

		CHealth() = default;

		CHealth(int m, int c) : max(m), current(c) {}
		RTTR_ENABLE(IComponent);
	};

	struct ENGINE_API COMPONENT() CInput : public IComponent
	{
	public:
		PROPERTY() bool up = false;
		PROPERTY() bool down = false;
		PROPERTY() bool left = false;
		PROPERTY() bool right = false;
		PROPERTY() bool attack = false;
		PROPERTY() bool canAttack = true;

		CInput() = default;
		RTTR_ENABLE(IComponent);
	};

	struct ENGINE_API COMPONENT() CBoundingBox : public IComponent
	{
	public:
		PROPERTY() Math::Vec2 size;
		PROPERTY() Math::Vec2 halfSize;
		PROPERTY() Math::Vec2 center;
		PROPERTY() Math::Vec2 prevCenter;
		PROPERTY() bool blockMove = false;
		PROPERTY() bool blockVision = false;

		CBoundingBox() = default;

		CBoundingBox(const Math::Vec2& c, const Math::Vec2& s, bool m, bool v)
			: size(s), halfSize(s.x / 2.0f, s.y / 2.0f), center(c), prevCenter(c), blockMove(m), blockVision(v) {}
		RTTR_ENABLE(IComponent);
	};

	struct ENGINE_API COMPONENT() CAnimation : public IComponent
	{
	public:
		PROPERTY() Graphics::Animation* animation = nullptr;
		PROPERTY() bool repeat = false;

		CAnimation() = default;

		CAnimation(Graphics::Animation a, bool r) : animation(&a), repeat(r) {}
		RTTR_ENABLE(IComponent);
	};

	struct ENGINE_API COMPONENT() CInvincibility : public IComponent
	{
	public:
		PROPERTY() int iframes = 1;

		CInvincibility() = default;

		explicit CInvincibility(int f) : iframes(f) {}
		RTTR_ENABLE(IComponent);
	};

	struct ENGINE_API COMPONENT() CLifespan : public IComponent
	{
	public:
		PROPERTY() int lifespan = 0;
		PROPERTY() int frameCreated = 0;

		CLifespan() = default;

		explicit CLifespan(int duration, int frame)
			: lifespan(duration), frameCreated(frame) {}
		RTTR_ENABLE(IComponent);
	};

	struct ENGINE_API COMPONENT() CPatrol : public IComponent
	{
	public:
		PROPERTY() std::vector<Math::Vec2> positions;
		PROPERTY() size_t currentPosition = 0;
		float speed = 0;

		CPatrol() = default;

		CPatrol(std::vector<Math::Vec2>& pos, float s) : positions(pos), speed(s) {}
		RTTR_ENABLE(IComponent);
	};

	struct ENGINE_API COMPONENT() CState : public IComponent
	{
	public:
		PROPERTY() std::string state = "stand";
		PROPERTY() std::string previousState = "stand";
		bool changeAnimation = false;

		CState() = default;

		explicit CState(std::string s) : state(std::move(s)) {}
		RTTR_ENABLE(IComponent);
	};

	struct ENGINE_API COMPONENT() CTransform : public IComponent
	{
	public:
		PROPERTY() Math::Vec2 pos = { 0.0f, 0.0f };
		PROPERTY() Math::Vec2 prevPos = { 0.0f, 0.0f };
		PROPERTY() Math::Vec2 velocity = { 0.0f, 0.0f };
		PROPERTY() Math::Vec2 scale = { 1.0f, 1.0f };
		PROPERTY() Math::Vec2 facing = { 0.0f, 1.0f };
		PROPERTY() float angle = 0;

		CTransform() = default;

		explicit CTransform(const Math::Vec2& p) : pos(p) {}

		CTransform(const Math::Vec2& p, const Math::Vec2& speed, const Math::Vec2& s, float a)
			: pos(p), prevPos(p), velocity(speed), scale(s), angle(a) {}
		RTTR_ENABLE(IComponent);
	};
}