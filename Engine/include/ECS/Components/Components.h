#pragma once

#include "ECS/IComponent.h"
#include "Graphics/Animation.h"
#include "Math/Vec2.h"
#include "Reflection/ReflectionMacros.h"
#include <rttr/registration.h>
#include <rttr/rttr_enable.h>

namespace Luden
{
	struct ENGINE_API CDamage : public IComponent 
	{
	public:
		int damage = 1;

		CDamage() = default;

		explicit CDamage(int d) : damage(d) {}
		RTTR_ENABLE(IComponent);
	};

	struct ENGINE_API CDraggable : public IComponent
	{
	public:
		bool dragging = false;

		CDraggable() = default;
		RTTR_ENABLE(IComponent);
	};

	struct ENGINE_API CFollowPlayer : public IComponent
	{
	public:
		Math::Vec2 home = { 0.0f, 0.0f };
		float speed = 0.0f;

		CFollowPlayer() = default;

		CFollowPlayer(Math::Vec2 p, float s) : home(p), speed(s) {}
		RTTR_ENABLE(IComponent);
	};

	struct ENGINE_API CGravity : public IComponent
	{
	public:
		float gravity = 0;

		CGravity() = default;

		explicit CGravity(float g) : gravity(g) {}
		RTTR_ENABLE(IComponent);
	};

	struct ENGINE_API CHealth : public IComponent
	{
	public:
		int max = 1;
		int current = 1;

		CHealth() = default;

		CHealth(int m, int c) : max(m), current(c) {}
		RTTR_ENABLE(IComponent);
	};

	struct ENGINE_API CInput : public IComponent
	{
	public:
		bool up = false;
		bool down = false;
		bool left = false;
		bool right = false;
		bool attack = false;
		bool canAttack = true;

		CInput() = default;
		RTTR_ENABLE(IComponent);
	};

	struct ENGINE_API CBoundingBox : public IComponent
	{
	public:
		Math::Vec2 size;
		Math::Vec2 halfSize;
		Math::Vec2 center;
		Math::Vec2 prevCenter;
		bool blockMove = false;
		bool blockVision = false;

		CBoundingBox() = default;

		CBoundingBox(const Math::Vec2& c, const Math::Vec2& s, bool m, bool v)
			: size(s), halfSize(s.x / 2.0f, s.y / 2.0f), center(c), prevCenter(c), blockMove(m), blockVision(v) {}
		RTTR_ENABLE(IComponent);
	};

	struct ENGINE_API CAnimation : public IComponent
	{
	public:
		Graphics::Animation animation;
		bool repeat = false;

		CAnimation() = default;

		CAnimation(Graphics::Animation a, bool r) : animation(a), repeat(r) {}
		RTTR_ENABLE(IComponent);
	};

	struct ENGINE_API CInvincibility : public IComponent
	{
	public:
		int iframes = 1;

		CInvincibility() = default;

		explicit CInvincibility(int f) : iframes(f) {}
		RTTR_ENABLE(IComponent);
	};

	struct ENGINE_API CLifespan : public IComponent
	{
	public:
		int lifespan = 0;
		int frameCreated = 0;

		CLifespan() = default;

		explicit CLifespan(int duration, int frame)
			: lifespan(duration), frameCreated(frame) {}
		RTTR_ENABLE(IComponent);
	};

	struct ENGINE_API CPatrol : public IComponent
	{
	public:
		std::vector<Math::Vec2> positions;
		size_t currentPosition = 0;
		float speed = 0;

		CPatrol() = default;

		CPatrol(std::vector<Math::Vec2>& pos, float s) : positions(pos), speed(s) {}
		RTTR_ENABLE(IComponent);
	};

	struct ENGINE_API CState : public IComponent
	{
	public:
		std::string state = "stand";
		std::string previousState = "stand";
		bool changeAnimation = false;

		CState() = default;

		explicit CState(std::string s) : state(std::move(s)) {}
		RTTR_ENABLE(IComponent);
	};

	struct ENGINE_API CTransform : public IComponent
	{
	public:
		Math::Vec2 pos = { 0.0f, 0.0f };
		Math::Vec2 prevPos = { 0.0f, 0.0f };
		Math::Vec2 velocity = { 0.0f, 0.0f };
		Math::Vec2 scale = { 1.0f, 1.0f };
		Math::Vec2 facing = { 0.0f, 1.0f };
		float angle = 0;

		CTransform() = default;

		explicit CTransform(const Math::Vec2& p) : pos(p) {}

		CTransform(const Math::Vec2& p, const Math::Vec2& speed, const Math::Vec2& s, float a)
			: pos(p), prevPos(p), velocity(speed), scale(s), angle(a) {}
		RTTR_ENABLE(IComponent);
	};
}