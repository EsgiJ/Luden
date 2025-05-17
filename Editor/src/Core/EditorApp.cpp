#include "Core/EditorApp.h"

#include <imgui.h>
#include <imgui-SFML.h>

#include "Panels/SceneHierarchyPanel.h"
#include "Panels/AssetBrowserPanel.h"
#include "Panels/InspectorPanel.h"
#include "Panels/ConsolePanel.h"
#include "Panels/ProfilerPanel.h"
#include "Panels/SceneSettingsPanel.h"

namespace Luden::Editor
{
	EditorApp::EditorApp() {}
	EditorApp::~EditorApp() {}

	void EditorApp::Update(float dt)
	{
	}

	void EditorApp::Render()
	{
		for (auto& p : m_Panels)
			p->Render();

		ImGui::Begin("Viewport");
		ImGui::Image(m_ViewportTextureID, ImVec2(640, 360));
		ImGui::End();
	}

	void EditorApp::RenderDockSpace()
	{
		ImGuiWindowFlags flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		auto* vp = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(vp->WorkPos);
		ImGui::SetNextWindowSize(vp->WorkSize);
		ImGui::SetNextWindowViewport(vp->ID);
		flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::Begin("#DockSpace", nullptr, flags);
		ImGui::PopStyleVar(2);

		ImGuiID dock_id = ImGui::GetID("EditorDockSpace");
		ImGui::DockSpace(dock_id, { 0,0 }, ImGuiDockNodeFlags_None);
		ImGui::End();
	}

	void EditorApp::HandleInput()
	{
		while (auto eventOpt = m_Window.pollEvent())
		{
			const auto& event = *eventOpt;
			ImGui::SFML::ProcessEvent(m_Window, event);
			if (event.is<sf::Event::Closed>())
				m_IsRunning = false;
		}
	}

	void EditorApp::Init()
	{
		GameEngine::Initialize("config/assets.txt");
		GameEngine::Get().Run();

		m_Window.create(sf::VideoMode(sf::Vector2u(1280, 720)), "Luden Engine");
		m_Window.setFramerateLimit(60);

		if (!m_ViewportTexture.resize({ 1280, 720 }))
			throw std::runtime_error("Failed to resize viewport texture");

		m_ViewportTextureID = (ImTextureID)(intptr_t)m_ViewportTexture.getTexture().getNativeHandle();

		ImGui::SFML::Init(m_Window);
		ImGui::StyleColorsDark();

		m_Panels.push_back(std::make_unique<SceneHierarchyPanel>());
		m_Panels.push_back(std::make_unique<AssetBrowserPanel>());
		//m_Panels.push_back(std::make_unique<InspectorPanel>());
		//m_Panels.push_back(std::make_unique<ConsolePanel>());
		//m_Panels.push_back(std::make_unique<ProfilerPanel>());
		//m_Panels.push_back(std::make_unique<SceneSettingsPanel>());

		m_IsRunning = true;
	}

	void EditorApp::Run()
	{
		while (m_IsRunning && m_Window.isOpen())
		{
			HandleInput();

			float dt = m_DeltaClock.restart().asSeconds();
			ImGui::SFML::Update(m_Window, sf::seconds(dt));

			GameEngine::Get().GetCurrentScene()->Update();

			m_ViewportTexture.setActive(true);
			m_ViewportTexture.clear(sf::Color::Black);
			GameEngine::Get().GetCurrentScene()->sRender(); 
			m_ViewportTexture.display();

			Update(dt);

			// Render UI
			m_Window.clear(sf::Color(45, 45, 50));
			RenderDockSpace();
			Render();
			ImGui::SFML::Render(m_Window);
			m_Window.display();
		}
	}

	void EditorApp::Shutdown()
	{
		ImGui::SFML::Shutdown();

		if (m_Window.isOpen())
			m_Window.close();

		GameEngine::Shutdown();
	}
}

