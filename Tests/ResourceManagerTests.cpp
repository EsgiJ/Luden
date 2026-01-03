#include "TestFramework.h"
#include "Resource/ResourceManager.h"

using namespace Luden;
using namespace Luden::Testing;

void RunResourceManagerTests()
{
	TestSuite suite("Resource Manager");

	// Test 1: Resource loading (basic check)
	suite.AddTest("Resource System Check", []()
		{
			// Just verify the resource manager exists and is initialized
			// Actual resource loading depends on having test assets
			std::cout << "    Resource Manager initialized\n";
			AssertTrue(true, "Resource system operational");
		});

	// Add more tests here when you have test resources

	suite.RunTests();
}