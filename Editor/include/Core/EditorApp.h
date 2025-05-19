#pragma once

#include "EditorAPI.h"

#include <memory>
#include <vector>

#include "imgui.h"
#include "SFML/Graphics.hpp"
#include "SFML/System/Clock.hpp"
#include <rttr/registration_friend.h>

#include "EditorAPI.h"
#include "Panels/Panel.h"
#include "Core/GameEngine.h"

namespace Luden::Editor
{
	class EDITOR_API EditorApp
	{
	public:
		EditorApp();
		~EditorApp();
		void Init();
		void Run();
		void Shutdown();

	private:
		void Render();
		void RenderDockSpace();
		void RenderModeToolbar();
		void HandleInput();

	private:
		sf::RenderWindow m_Window;
		sf::Clock m_DeltaClock;
		std::vector<std::unique_ptr<Panel>> m_Panels;
		bool m_IsRunning = false;
		bool m_LayoutBuilt = false;

		sf::RenderTexture m_ViewportTexture;
		ImTextureID m_ViewportTextureID = 0;
	};
}