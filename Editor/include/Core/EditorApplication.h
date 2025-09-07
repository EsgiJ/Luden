#pragma once

#include "Core/RuntimeApplication.h"
#include "Panels/Panel.h"
#include "Core/EditorState.h"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/System/Clock.hpp>

#include <vector>
#include <memory>
#include "imgui.h"

namespace Luden::Editor {

	class EditorApplication {
	public:
		EditorApplication();
		~EditorApplication();

		void Init();
		void Run();
		void Shutdown();

	private:
		void Render();
		void RenderDockSpace();
		void RenderModeToolbar();
		void RenderTitleBar();
		void LoadFonts();
		void HandleInput();

	private:
		sf::RenderWindow m_Window;
		sf::Clock m_DeltaClock;
		sf::RenderTexture m_ViewportTexture;
		ImTextureID m_ViewportTextureID = 0;

		std::vector<std::unique_ptr<Panel>> m_Panels;
		bool m_IsRunning = false;
		bool m_LayoutBuilt = false;

		std::unique_ptr<Luden::RuntimeApplication> m_RuntimeApp;
	};
}
