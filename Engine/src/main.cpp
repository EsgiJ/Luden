#include "Core/GameEngine.h"

#include <iostream>

#include <SFML/Graphics.hpp>

namespace Luden
{
	int main() {
		GameEngine::Initialize("config/assets.txt");
		GameEngine::Get().Run();

		return 0;
	}
}
