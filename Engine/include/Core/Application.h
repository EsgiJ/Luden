#pragma once
/*
#include "Core/Layer.h"
#include "Core/TimeStep.h"
#include "EngineAPI.h"

#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

#include "deque"
#include <stdint.h>
#include <string>
#include <filesystem>

namespace Luden
{
	struct ENGINE_API ApplicationSpecification
	{
		std::string Name = "Luden";
		uint32_t WindowWidth = 1600, WindowHeight = 900;
		bool WindowDecorated = false;
		bool Fullscreen = false;
		bool VSync = true;
		std::string WorkingDirectory;
		bool StartMaximized = true;
		bool Resizable = true;
		bool EnableImGui = true;
		std::filesystem::path IconPath;
	};

	class ENGINE_API Application
	{
	public:
		Application(const ApplicationSpecification& specification);
		virtual ~Application();

		void Run();
		void Close();

		virtual void OnInit();
		virtual void OnShutdown();
		virtual void OnUpdate(TimeStep ts);

		virtual void OnEvent(sf::Event& event);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);
		void PopLayer(Layer* layer);
		void PopOverlay(Layer* overlay);
		void RenderImGui();

		void SetShowStats(bool show) { m_ShowStats = show; }

		inline sf::Window& GetWindow() { return *m_Window; }

		static inline Application& Get() { return *s_Instance; }

		TimeStep GetTimestep() const { return m_TimeStep; }
		TimeStep GetFrametime() const { return m_FrameTime; }
		float GetTime() const; // TODO: This should be in "Platform"

		static const char* GetConfigurationName();
		static const char* GetPlatformName();

		const ApplicationSpecification& GetSpecification() const { return m_Specification; }

		ImGuiLayer* GetImGuiLayer() { return m_ImGuiLayer; }

		uint32_t GetCurrentFrameIndex() const { return m_CurrentFrameIndex; }

		//ApplicationSettings& GetSettings() { return m_AppSettings; }
		//const ApplicationSettings& GetSettings() const { return m_AppSettings; }

		static bool IsRuntime() { return s_IsRuntime; }

	protected:
		inline static bool s_IsRuntime = false;

	private:
		void ProcessEvents();

		bool OnWindowResize(WindowResizeEvent& e);
		bool OnWindowMinimize(WindowMinimizeEvent& e);
		bool OnWindowClose(WindowCloseEvent& e);
	private:
		std::unique_ptr<sf::RenderWindow> m_Window;
		ApplicationSpecification m_Specification;
		bool m_Running = true;
		bool m_Minimized = false;
		LayerStack m_LayerStack;
		ImGuiLayer m_ImGuiLayer;
		TimeStep m_FrameTime;
		TimeStep m_TimeStep;
		bool m_ShowStats;

		float m_LastFrameTime = 0.0f;
		uint32_t m_CurrentFrameIndex = 0;

		//ApplicationSettings m_AppSettings;

		static Application* s_Instance;
	};
}*/