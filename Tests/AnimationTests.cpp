#include "TestFramework.h"

using namespace Luden::Testing;

void RunAnimationTests()
{
	TestSuite suite("Animation System");

	suite.AddTest("Animation System Placeholder", []()
		{
			// Add animation tests when system is ready
			std::cout << "    Animation tests not yet implemented\n";
		});

	suite.RunTests();
}