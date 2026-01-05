#include "TetrisRuntimeApp.h"
#include <iostream>

namespace Luden
{
	TetrisRuntimeApp::TetrisRuntimeApp(const ApplicationSpecification& spec)
		: RuntimeApplication(spec)
	{
		std::cout << "[TetrisRuntimeApp] Custom runtime application created!" << std::endl;
	}

	void TetrisRuntimeApp::OnUpdate(TimeStep ts)
	{
		//RuntimeApplication::OnUpdate(ts);

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
