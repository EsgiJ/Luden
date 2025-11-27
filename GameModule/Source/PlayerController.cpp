#include "PlayerController.h"
#include "Input/InputContext.h"
#include "Input/InputManager.h"
#include "ECS/Components/Components.h"
#include "ScriptAPI/DebugAPI.h"

#include <iostream>

namespace Luden
{
    void PlayerController::OnCreate()
    {
        // TODO: Initialize
		auto gameplayContext = std::make_shared<InputContext>("Gameplay", 100);
		gameplayContext->SetEnabled(true);
		InputAction jumpAction("Jump");

		gameplayContext->AddMapping({
			jumpAction,
			sf::Keyboard::Key::Space,
			ModifierConfig(),
			TriggerConfig(ETriggerType::Pressed)
			});

		InputAction fireAction("Fire");

		gameplayContext->AddMapping({
			fireAction,
			sf::Keyboard::Key::P,
			ModifierConfig(),
			TriggerConfig(ETriggerType::Pressed)
			});

		InputAction moveAction("Move");
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

		input.BindAction(
			jumpAction,
			ETriggerEvent::Started,
			this,
			&PlayerController::OnJump
		);

		input.BindAction(
			moveAction,
			ETriggerEvent::Ongoing,
			this,
			&PlayerController::OnMove
		);

		input.BindAction(
			fireAction,
			ETriggerEvent::Started,
			this,
			&PlayerController::OnShoot
		);
    }

    void PlayerController::OnUpdate(TimeStep ts)
    {
        // TODO: Update logic
    }

    void PlayerController::OnDestroy()
    {
        // TODO: Cleanup
    }

	void PlayerController::OnJump(const InputValue& value)
	{

	}

	void PlayerController::OnMove(const InputValue& value)
	{
	}

	void PlayerController::OnShoot(const InputValue& value)
	{
		auto& transform = GetComponent<TransformComponent>().Translation;

		glm::vec3 start = transform;
		glm::vec3 direction = glm::vec3(1.0f, 0.0f, 0.0f); 
		glm::vec3 end = start + direction * 500.0f;        

		DebugAPI::DrawDebugLine(start, end, sf::Color::Red, 10000.0f);
	}
}
