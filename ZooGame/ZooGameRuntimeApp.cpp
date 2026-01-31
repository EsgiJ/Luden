#include "ZooGameRuntimeApp.h"
#include <iostream>

namespace Luden
{
	ZooGameRuntimeApp::ZooGameRuntimeApp(const ApplicationSpecification& spec)
		: RuntimeApplication(spec)
	{
		std::cout << "[ZooGameRuntimeApp] Custom runtime application created!" << std::endl;
	}

	ZooGameRuntimeApp::~ZooGameRuntimeApp()
	{
		std::cout << "[ZooGameRuntimeApp] Custom runtime application destroyed!" << std::endl;
	}

	void ZooGameRuntimeApp::OnUpdate(TimeStep ts)
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
