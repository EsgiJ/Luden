#include "PaddleGameRuntimeApp.h"
#include <iostream>

namespace Luden
{
	PaddleGameRuntimeApp::PaddleGameRuntimeApp(const ApplicationSpecification& spec)
		: RuntimeApplication(spec)
	{
		std::cout << "[PaddleGameRuntimeApp] Custom runtime application created!" << std::endl;
	}

	void PaddleGameRuntimeApp::OnUpdate(TimeStep ts)
	{
		RuntimeApplication::OnUpdate(ts);

		m_UpdateTimer += ts.GetSeconds();

		// TODO: Add your custom game loop logic here
		// Example:
		// if (m_UpdateTimer >= 1.0f)
		// {
		//     std::cout << "One second passed!" << std::endl;
		//     m_UpdateTimer = 0.0f;
		// }
	}
}
