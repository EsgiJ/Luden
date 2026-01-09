#include "PaddleController.h"

#include "Input/InputContext.h"
#include "Input/InputManager.h"
#include "ScriptAPI/GameplayAPI.h"
#include "ScriptAPI/Physics2DAPI.h"
#include "ScriptAPI/MathAPI.h"
#include <iostream>

namespace Luden
{
    void PaddleController::OnCreate()
    {
		auto context = std::make_shared<InputContext>("Gameplay", 100);
		context->SetEnabled(true);

		InputAction moveLeftAction("MoveLeft");
		InputAction moveRightAction("MoveRight");

		context->AddMapping({
			moveLeftAction,
			sf::Keyboard::Key::Left,
			ModifierConfig(),
			TriggerConfig(ETriggerType::Pressed)
			});

		context->AddMapping({
			moveLeftAction,
			sf::Keyboard::Key::A,
			ModifierConfig(),
			TriggerConfig(ETriggerType::Pressed)
			});

		context->AddMapping({
			moveRightAction,
			sf::Keyboard::Key::Right,
			ModifierConfig(),
			TriggerConfig(ETriggerType::Pressed)
			});

		context->AddMapping({
			moveRightAction,
			sf::Keyboard::Key::D,
			ModifierConfig(),
			TriggerConfig(ETriggerType::Pressed)
			});

		InputManager::Instance().PushContext(context);

		auto& input = GetComponent<InputComponent>();
		input.priority = 100;
		input.consumeInput = true;

		input.BindAction(moveLeftAction, ETriggerEvent::Started, this, &PaddleController::OnMoveLeft);
		input.BindAction(moveRightAction, ETriggerEvent::Started, this, &PaddleController::OnMoveRight);
    }

    void PaddleController::OnUpdate(TimeStep ts)
    {
		Vec3 pos = GameplayAPI::GetPosition(GetEntity());

		pos.y = 0;
		GameplayAPI::SetPosition(GetEntity(), pos);
    }

    void PaddleController::OnDestroy()
    {
        // TODO: Cleanup
    }

    void PaddleController::OnCollisionBegin(const CollisionContact& contact)
    {
        // TODO: On contact begin
    }

    void PaddleController::OnCollisionEnd(const CollisionContact& contact)
    {
        // TODO: On contact end
    }

    void PaddleController::OnCollisionHit(const CollisionContact& contact)
    {
        // TODO: On hit(high speed)
    }

	void PaddleController::OnMoveLeft(const InputValue& value)
	{
		std::cout << "OnMoveLeft" << std::endl;

		Vec2 velocity(-m_Speed, 0.0f);  
		Physics2DAPI::SetLinearVelocity(GetEntity(), velocity);
	}

	void PaddleController::OnMoveRight(const InputValue& value)
	{
		std::cout << "OnMoveRight" << std::endl;

		Vec2 velocity(m_Speed, 0.0f);  
		Physics2DAPI::SetLinearVelocity(GetEntity(), velocity);
	}

}
