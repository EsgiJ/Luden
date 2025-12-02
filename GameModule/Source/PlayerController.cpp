#include "PlayerController.h"
#include "Input/InputContext.h"
#include "Input/InputManager.h"
#include "ECS/Components/Components.h"
#include "ScriptAPI/DebugAPI.h"
#include "ScriptAPI/Physics2DAPI.h"

#include <iostream>
#include <glm/geometric.hpp>

namespace Luden
{
	constexpr float MOVE_SPEED = 10.0f;
	constexpr float JUMP_FORCE = 1.5f;

    void PlayerController::OnCreate()
    {
		auto gameplayContext = std::make_shared<InputContext>("Gameplay", 100);
		gameplayContext->SetEnabled(true);

		InputAction jumpAction("Jump");
		InputAction fireAction("Fire");
		InputAction moveAction("Move");

		gameplayContext->AddMapping({
			jumpAction,
			sf::Keyboard::Key::Space,
			ModifierConfig(),
			TriggerConfig(ETriggerType::Pressed)
			});

		gameplayContext->AddMapping({
			fireAction,
			sf::Keyboard::Key::P,
			ModifierConfig(),
			TriggerConfig(ETriggerType::Pressed)
			});

		ModifierConfig moveConfig;
		moveConfig.normalize = true; 

		gameplayContext->AddAxis2DMapping({
			moveAction,
			sf::Keyboard::Key::W,
			sf::Keyboard::Key::S,
			sf::Keyboard::Key::A,
			sf::Keyboard::Key::D,
			moveConfig
			});

		InputManager::Instance().PushContext(gameplayContext);

		auto& input = GetComponent<InputComponent>();
		input.priority = 100;
		input.consumeInput = true;

		input.BindAction(jumpAction, ETriggerEvent::Started, this, &PlayerController::OnJump);
		input.BindAction(moveAction, ETriggerEvent::Ongoing, this, &PlayerController::OnMove);
		input.BindAction(moveAction, ETriggerEvent::Completed, this, &PlayerController::OnMoveStop); 
		//input.BindAction(fireAction, ETriggerEvent::Started, this, &PlayerController::OnShoot);
    }

    void PlayerController::OnUpdate(TimeStep ts)
    {
		auto vel = Physics2DAPI::GetLinearVelocity(GetEntity());

		if (glm::length(m_CurrentMoveInput) > 0.0f)
		{
			vel.x = m_CurrentMoveInput.x * MOVE_SPEED;
		}
		else
		{
			vel.x = 0.0f;
		}

		Physics2DAPI::SetLinearVelocity(GetEntity(), vel);

    }

    void PlayerController::OnDestroy()
    {
        // TODO: Cleanup
    }

	void PlayerController::OnJump(const InputValue& value)
	{
		Physics2DAPI::AddImpulseToCenter(GetEntity(), glm::vec2(0.0f, JUMP_FORCE));

		std::cout << "Jump" << std::endl;
	}

	void PlayerController::OnMove(const InputValue& value)
	{
		m_CurrentMoveInput = value.GetAxis2D();
		std::cout << "Move" << std::endl;
	}

	void PlayerController::OnMoveStop(const InputValue& value)
	{
		m_CurrentMoveInput = glm::vec2(0.0f);
	}

	void PlayerController::OnShoot(const InputValue& value)
	{
		auto& transform = GetComponent<TransformComponent>().Translation;

		glm::vec3 start = transform;

		glm::vec3 direction = glm::vec3(1.0f, 0.0f, 0.0f);
		glm::vec3 end = start + (direction * 500.0f);

		DebugAPI::DrawDebugCircle(start, 50, sf::Color::Red, 1.0f); 
		DebugAPI::DrawDebugLine(start, end, sf::Color::Yellow, 3.0f); 

		std::cout << "Shoot" << std::endl;
	}
}
