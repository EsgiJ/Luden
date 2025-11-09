#pragma once

#include "ECS/IComponent.h"
#include "NativeScript/NativeScript.h"
#include "Graphics/Animation.h"
#include "Graphics/Font.h"
#include "Graphics/Texture.h"
#include "Project/Project.h"

#include <glm/vec2.hpp>
#include <box2d/box2d.h>
#include "glm/ext/vector_float3.hpp"

namespace Luden
{
	struct ENGINE_API RelationshipComponent : public IComponent
	{
		UUID ParentHandle = 0;
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
		glm::vec2 home = { 0.0f, 0.0f };
		float speed = 0.0f;

		FollowPLayerComponent() = default;

		FollowPLayerComponent(glm::vec2 p, float s) : home(p), speed(s) {}
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

	struct ENGINE_API RigidBody2DComponent : public IComponent
	{
		enum class Type { None = -1, Static, Dynamic, Kinematic };

		Type BodyType;
		bool FixedRotation = false;
		float Mass = 1.0f;
		float LinearDrag = 0.01f;
		float AngularDrag = 0.05f;
		float GravityScale = 1.0f;
		b2BodyId RuntimeBodyId = b2_nullBodyId;

		RigidBody2DComponent() = default;

		explicit RigidBody2DComponent(Type bodyType, bool fixedRotation, float mass, float linearDrag, float angularDrag, float gravityScale, b2BodyId runtimeBodyId)
			: BodyType(bodyType), FixedRotation(fixedRotation), Mass(mass), LinearDrag(linearDrag), AngularDrag(angularDrag), GravityScale(gravityScale), RuntimeBodyId(runtimeBodyId) { }

		RigidBody2DComponent(const RigidBody2DComponent& other) = default;
	};

	struct ENGINE_API BoxCollider2DComponent : public IComponent
	{
		glm::vec2 Offset = { 0.0f,0.0f };
		glm::vec2 Size = { 0.5f, 0.5f };

		float Density = 1.0f;
		float Friction = 1.0f;

		b2ShapeId RuntimeShapeId = b2_nullShapeId;

		BoxCollider2DComponent() = default;
		BoxCollider2DComponent(const BoxCollider2DComponent& other) = default;
	};

	struct ENGINE_API CircleCollider2DComponent : public IComponent
	{
		glm::vec2 Offset = { 0.0f,0.0f };
		float Radius = 1.0f;

		float Density = 1.0f;
		float Friction = 1.0f;

		b2ShapeId RuntimeShapeId = b2_nullShapeId;

		CircleCollider2DComponent() = default;
		CircleCollider2DComponent(const CircleCollider2DComponent& other) = default;
	};

	class ScriptableEntity;
	class Entity;

	struct ENGINE_API NativeScriptComponent : public IComponent
	{
		ScriptableEntity* Instance = nullptr;
		ResourceHandle ScriptHandle = 0;

		ScriptInstantiateFunc InstantiateScript = nullptr;
		ScriptDestroyFunc DestroyScript = nullptr;

		template<typename T>
		void Bind();

		void BindFromHandle(ResourceHandle handle);
		void CreateInstance(Entity entity);
		void DestroyInstance();
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
		std::vector<glm::vec2> positions;
		size_t currentPosition = 0;
		float speed = 0;

		PatrolComponent() = default;

		PatrolComponent(std::vector<glm::vec2>& pos, float s) : positions(pos), speed(s) {}
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
		glm::vec3 Translation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Scale = { 1.0f, 1.0f, 1.0f };
		float angle = 0;

		TransformComponent() = default;

		explicit TransformComponent(const glm::vec3& t) : Translation(t) {}

		TransformComponent(const glm::vec3& t, const glm::vec3& s, float a)
			: Translation(t), Scale(s), angle(a) {}
	};
}