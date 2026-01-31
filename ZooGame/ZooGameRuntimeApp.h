#pragma once
#include "Core/RuntimeApplication.h"

namespace Luden
{
	class ZooGameRuntimeApp : public RuntimeApplication
	{
	public:
		ZooGameRuntimeApp(const ApplicationSpecification& spec);
		~ZooGameRuntimeApp() override;

		void OnUpdate(TimeStep ts) override;

	private:
		// Add your custom runtime application members here
		float m_UpdateTimer = 0.0f;
	};
}
