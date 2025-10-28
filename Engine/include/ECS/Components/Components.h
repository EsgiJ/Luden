#pragma once

#include "ECS/IComponent.h"
#include "Graphics/Animation.h"
#include "Graphics/Font.h"
#include "Graphics/Texture.h"
#include "Math/Vec2.h"
#include "Reflection/ReflectionMacros.h"

namespace Luden
{
	struct ENGINE_API RelationshipComponent : public IComponent
	{
		UUID ParentHandle;
		std::vector<UUID> Children;

		RelationshipComponent() = default;
		RelationshipComponent(const RelationshipComponent& other) = default;
		RelationshipComponent(UUID parent)
			: ParentHandle(parent) {}
	};
	struct ENGINE_API DamageComponent : public IComponent 
	{
	public:
		int damage = 1;

		DamageComponent() = default;

		explicit DamageComponent(int d) : damage(d) {}
	};

	struct ENGINE_API DraggableComponent : public IComponent
	{
	public:
		bool dragging = false;

		DraggableComponent() = default;

		DraggableComponent(bool d) : dragging(d) {}
	};

	struct ENGINE_API FollowPLayerComponent : public IComponent
	{
	public:
		Math::Vec2 home = { 0.0f, 0.0f };
		float speed = 0.0f;

		FollowPLayerComponent() = default;

		FollowPLayerComponent(Math::Vec2 p, float s) : home(p), speed(s) {}
	};

	struct ENGINE_API GravityComponent : public IComponent
	{
	public:
		float gravity = 0;

		GravityComponent() = default;

		explicit GravityComponent(float g) : gravity(g) {}
	};

	struct ENGINE_API HealthComponent : public IComponent
	{
	public:
		int max = 1;
		int current = 1;

		HealthComponent() = default;

		HealthComponent(int m, int c) : max(m), current(c) {}
	};

	struct ENGINE_API InputComponent : public IComponent
	{
	public:
		bool up = false;
		bool down = false;
		bool left = false;
		bool right = false;
		bool attack = false;
		bool canAttack = true;

		InputComponent() = default;

		explicit InputComponent(bool up, bool down, bool left, bool right, bool attack, bool canAttack)
			: up(up), down(down), left(left), right(right), attack(attack), canAttack(canAttack) { }
	};

	struct ENGINE_API BoxCollider2DComponent : public IComponent
	{
	public:
		Math::Vec2 size;
		Math::Vec2 halfSize;
		Math::Vec2 center;
		Math::Vec2 prevCenter;
		bool blockMove = false;
		bool blockVision = false;

		BoxCollider2DComponent() = default;

		BoxCollider2DComponent(const Math::Vec2& c, const Math::Vec2& s, bool m, bool v)
			: size(s), halfSize(s.x / 2.0f, s.y / 2.0f), center(c), prevCenter(c), blockMove(m), blockVision(v) {}
	};

	struct ENGINE_API Animation2DComponent : public IComponent
	{
	public:
		ResourceHandle animationHandle;
		size_t speed = 0;
		size_t currentFrame = 0;

		bool repeat = false;

		Animation2DComponent() = default;
		Animation2DComponent(ResourceHandle a, size_t s, size_t cf, bool r) : animationHandle(a), speed(s), currentFrame(cf), repeat(r) {};
	};

	struct ENGINE_API TextComponent : public IComponent
	{
	public:
		ResourceHandle fontHandle;
		
		TextComponent() = default;

		TextComponent(ResourceHandle f) : fontHandle(f) {};
	};

	struct ENGINE_API TextureComponent : public IComponent
	{
	public:
		ResourceHandle textureHandle;

		TextureComponent() = default;

		TextureComponent(ResourceHandle t) : textureHandle(t) {};
	};

	struct ENGINE_API InvincibilityComponent : public IComponent
	{
	public:
		int iframes = 1;

		InvincibilityComponent() = default;

		explicit InvincibilityComponent(int f) : iframes(f) {}
	};

	struct ENGINE_API LifespanComponent : public IComponent
	{
	public:
		int lifespan = 0;
		int frameCreated = 0;

		LifespanComponent() = default;

		explicit LifespanComponent(int duration, int frame)
			: lifespan(duration), frameCreated(frame) {}
	};

	struct ENGINE_API PatrolComponent : public IComponent
	{
	public:
		std::vector<Math::Vec2> positions;
		size_t currentPosition = 0;
		float speed = 0;

		PatrolComponent() = default;

		PatrolComponent(std::vector<Math::Vec2>& pos, float s) : positions(pos), speed(s) {}
	};

	struct ENGINE_API StateComponent : public IComponent
	{
	public:
		std::string state = "stand";
		std::string previousState = "stand";
		bool changeAnimation = false;

		StateComponent() = default;

		explicit StateComponent(std::string s) : state(std::move(s)) {}
	};

	struct ENGINE_API TransformComponent : public IComponent
	{
	public:
		Math::Vec2 pos = { 0.0f, 0.0f };
		Math::Vec2 prevPos = { 0.0f, 0.0f };
		Math::Vec2 velocity = { 0.0f, 0.0f };
		Math::Vec2 scale = { 1.0f, 1.0f };
		Math::Vec2 facing = { 0.0f, 1.0f };
		float angle = 0;

		TransformComponent() = default;

		explicit TransformComponent(const Math::Vec2& p) : pos(p) {}

		TransformComponent(const Math::Vec2& p, const Math::Vec2& speed, const Math::Vec2& s, float a)
			: pos(p), prevPos(p), velocity(speed), scale(s), angle(a) {}
	};
}