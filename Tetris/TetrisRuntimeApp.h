#pragma once
#include "Core/RuntimeApplication.h"

namespace Luden
{
	class TetrisRuntimeApp : public RuntimeApplication
	{
	public:
		TetrisRuntimeApp(const ApplicationSpecification& spec);
		~TetrisRuntimeApp() override = default;

		void OnUpdate(TimeStep ts) override;

	private:
		// Add your custom runtime application members here
		float m_UpdateTimer = 0.0f;
	};
}
