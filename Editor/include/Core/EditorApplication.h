#pragma once

#include "Core/RuntimeApplication.h"
#include "Core/EditorState.h"
#include "Utils/EditorResources.h"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/System/Clock.hpp>

#include <vector>
#include <memory>
#include "imgui.h"

namespace Luden 
{
	class EditorTab;
	class NativeScriptModuleLoader;
	class EditorApplication 
	{
	public:
		EditorApplication();
		~EditorApplication();

		void Init();
		void Run();
		void Shutdown();

		void OnUpdate(TimeStep timestep);
		void OnEvent(const std::optional<sf::Event>& evt);
		void OnImGuiRender();

		void RequestOpenResource(const std::filesystem::path& path);

		void HotReloadNativeScripts();

		void CreateNewScene();
		void SaveActiveScene();
		void ExitEditor();

		void ProcessOpenResourceRequests();
		void OpenResource(const std::filesystem::path& path);

		bool BeginMainDockspace();
		void RenderTitleBar();
		void RenderContent();
		void InitializeMainDockspace();

		void CreateNewProject(const std::string& projectName, const std::filesystem::path& path);
		void EmptyProject();
		void SaveProject();
		void LoadProject(const std::filesystem::path& path);
		void CloseProject();

		void ShowNewProjectPopup();
		void ShowLoadProjectPopup();

		void ReloadTab();
		void CloseAllTabs();

		bool OnKeyPressed(const sf::Event::KeyPressed& key);

		void RenderMenuBar();
	private:
		std::vector<std::shared_ptr<EditorTab>> m_EditorTabs;
		std::shared_ptr<EditorTab> m_FocusedTab;

		std::unique_ptr<NativeScriptModuleLoader> m_NativeScriptModuleLoader;
		uint64_t m_LastModuleWriteTime;

		ImGuiID m_MainDockspaceID;
		ImGuiWindowClass m_MainDockspaceClass;
		bool m_MainDockspaceInitialized = false;

		sf::RenderWindow m_Window;

		std::vector<std::filesystem::path> m_OpenResourceRequests;

		sf::Clock m_Clock;      
		float m_LastFrameTime = 0.0f;

		std::string m_ProjectPath;
		std::filesystem::path m_PersistentStoragePath;

		bool m_ShowNewProjectPopup = false;
		bool m_ShowLoadProjectPopup = false;
		char m_ProjectNameBuffer[256] = "MyGame";
		char m_ProjectPathBuffer[512] = "";
		std::string m_PopupStatusMessage;
	};
}
