#pragma once
#include "Core/RuntimeApplication.h"

namespace Luden
{
	class SpaceWarRuntimeApp : public RuntimeApplication
	{
	public:
		SpaceWarRuntimeApp(const ApplicationSpecification& spec);
		~SpaceWarRuntimeApp() override = default;

		void OnUpdate(TimeStep ts) override;

	private:
		// Add your custom runtime application members here
		float m_UpdateTimer = 0.0f;
	};
}
