#pragma once
#include "Core/RuntimeApplication.h"

namespace Luden
{
	class PaddleGameRuntimeApp : public RuntimeApplication
	{
	public:
		PaddleGameRuntimeApp(const ApplicationSpecification& spec);
		~PaddleGameRuntimeApp() override = default;

		void OnUpdate(TimeStep ts) override;

	private:
		// Add your custom runtime application members here
		float m_UpdateTimer = 0.0f;
	};
}
