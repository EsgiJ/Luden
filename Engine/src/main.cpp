#include <SFML/Graphics.hpp>
#include "Core/GameEngine.h"

namespace Luden
{
	int main() {
		GameEngine::Initialize("config/assets.txt");
		GameEngine::Get().Run();

		return 0;
	}
}
