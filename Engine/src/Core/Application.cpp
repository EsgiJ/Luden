/*
#include "Core/Application.h"
#include "Project/Project.h"

#include <SFML/System/Clock.hpp>
#include <SFML/Window/VideoMode.hpp>
#include <SFML/Graphics/Image.hpp> 
#include <SFML/Window/Event.hpp> 
#include <algorithm>
#include <filesystem>

namespace
{
	sf::Clock gAppClock;
}

namespace Luden
{
	Application* Application::s_Instance = nullptr;

	Application::Application(const ApplicationSpecification& specification)
		: m_Specification(specification)
	{
		s_Instance = this;

		if (!specification.WorkingDirectory.empty())
			std::filesystem::current_path(specification.WorkingDirectory);

		const auto desktop = sf::VideoMode::getDesktopMode();
		const unsigned bpp = desktop.bitsPerPixel;

		sf::State state = m_Specification.Fullscreen ? sf::State::Fullscreen : sf::State::Windowed;

		uint32_t style = sf::Style::None;
		if (!m_Specification.Fullscreen)
		{
			if (!m_Specification.WindowDecorated)
			{
				style = sf::Style::None;
			}
			else
			{
				style = sf::Style::Titlebar | sf::Style::Close;
				if (m_Specification.Resizable)
					style |= sf::Style::Resize;
			}
		}

		m_Window = std::make_unique<sf::RenderWindow>();
		m_Window->create(
			sf::VideoMode({ m_Specification.WindowWidth, m_Specification.WindowHeight }, bpp),
			m_Specification.Name,
			state,
			sf::ContextSettings{}
		);
		

		m_Window->setVerticalSyncEnabled(m_Specification.VSync);

		if (m_Specification.StartMaximized && !m_Specification.Fullscreen)
		{
			m_Window->setSize({ desktop.size.x, desktop.size.y });
			m_Window->setPosition({ 0, 0 });
		}

		if (!m_Specification.IconPath.empty())
		{
			sf::Image icon;
			if (icon.loadFromFile(m_Specification.IconPath.string()))
			{
				m_Window->setIcon(icon.getSize(), icon.getPixelsPtr());
			}
		}

		if (m_Specification.EnableImGui)
		{
			m_ImGuiLayer = ImGuiLayer::Create();
			PushOverlay(m_ImGuiLayer);
		}
	}

	Application::~Application()
	{
		for (Layer* layer : m_LayerStack)
		{
			layer->OnDetach();
			delete layer;
		}

		Project::SetActive(nullptr);
	}

	void Application::PushLayer(Layer* layer)
	{
		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* overlay)
	{
		m_LayerStack.PushOverlay(overlay);
		overlay->OnAttach();
	}

	void Application::PopLayer(Layer* layer)
	{
		m_LayerStack.PopLayer(layer);
		layer->OnDetach();
	}

	void Application::PopOverlay(Layer* overlay)
	{
		m_LayerStack.PopOverlay(overlay);
		overlay->OnDetach();
	}

	void Application::RenderImGui()
	{
		for (int i = 0; i < m_LayerStack.Size(); i++)
			m_LayerStack[i]->OnImGuiRender();
	}

	void Application::ProcessEvents()
	{
		while (const std::optional<sf::Event> event = m_Window->pollEvent())
		{
			if (event->is<sf::Event::Closed>())
			{
				Close();
				continue;
			}

			if (event->is<sf::Event::FocusLost>())
				m_Minimized = true;
			else if (event->is<sf::Event::FocusGained>())
				m_Minimized = false;

			// Resize
			if (const auto* resized = event->getIf<sf::Event::Resized>())
			{
				sf::View view = m_Window->getView();
				view.setSize({ static_cast<float>(resized->size.x), static_cast<float>(resized->size.y) });
				view.setCenter(view.getSize() * 0.5f);
				m_Window->setView(view);
			}

			OnEvent(*event);
		}
	}

	void Application::OnEvent(sf::Event& event)
	{
		bool handled = false;
		for (auto it = m_LayerStack.end(); it != m_LayerStack.begin(); )
		{
			Layer* layer = *--it;
			handled = layer->OnEvent(event);
			if (handled)
				break;
		}
	}


	void Application::Run()
	{
		OnInit();

		while (m_Running && m_Window && m_Window->isOpen())
		{
			ProcessEvents();

			float time = GetTime();
			m_FrameTime = time - m_LastFrameTime;
			m_TimeStep = std::min<float>(m_FrameTime, 0.0333f); // 30 FPS clamp
			m_LastFrameTime = time;

			if (!m_Minimized)
			{
				OnUpdate(m_TimeStep);

				for (Layer* layer : m_LayerStack)
					layer->OnUpdate(m_TimeStep);

				if (m_Specification.EnableImGui && m_ImGuiLayer)
					RenderImGui();

				m_Window->display();

				m_CurrentFrameIndex++;
			}
			else
			{
				sf::sleep(sf::milliseconds(5));
			}
		}

		OnShutdown();
	}

	void Application::Close()
	{
		m_Running = false;
		if (m_Window && m_Window->isOpen())
			m_Window->close();
	}

	float Application::GetTime() const
	{
		return gAppClock.getElapsedTime().asSeconds();
	}
}
*/