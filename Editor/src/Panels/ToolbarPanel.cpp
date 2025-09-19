#include "Panels/ToolbarPanel.h"
#include "Utils/EditorResources.h"

namespace Luden {


	ToolbarPanel::ToolbarPanel(const std::shared_ptr<Scene>& context, SceneHierarchyPanel* sceneHierarchyPanel)
	{
		SetContext(context, sceneHierarchyPanel);
	}

	void ToolbarPanel::SetContext(const std::shared_ptr<Scene>& context, SceneHierarchyPanel* sceneHierarchyPanel)
	{
		m_Context = context;
		m_SceneHierarchyPanel = sceneHierarchyPanel;
	}

	void ToolbarPanel::InitValues(const std::shared_ptr<sf::RenderTexture> renderWindow, bool& isViewportHovered)
	{
		m_RenderWindow = renderWindow;
		m_ViewportHovered = &isViewportHovered;
	}

	void ToolbarPanel::OnUpdate() 
	{
		switch (m_SelectedTool) 
		{
		case Tool::SELECT:
			HandlePickEntityWithMouse();
			break;
		case Tool::MOVE:
			Moving();
			break;
		case Tool::SCALE:
			Scaling();
			break;
		case Tool::ROTATE:
			Rotating();
			break;

		default:
			break;
		};
	}

	void ToolbarPanel::OnImGuiRender(const ImVec2& viewportBoundMin, const ImVec2& viewportBoundMax)
	{
		m_ViewportBoundMin = viewportBoundMin;
		m_ViewportBoundMax = viewportBoundMax;
		ShowToolbar();
	}

	void ToolbarPanel::OnEvent(const std::optional<sf::Event>& evt)
	{
		if (!evt) return;

		if (auto key = evt->getIf<sf::Event::KeyPressed>()) 
		{
			OnKeyPressed(*key);
		}
		else if (auto mouse = evt->getIf<sf::Event::MouseButtonPressed>()) 
		{
			OnMouseButtonPressed(*mouse);
		}
		else if (auto mouse = evt->getIf<sf::Event::MouseButtonReleased>()) 
		{
			OnMouseButtonReleased(*mouse);
		}
		else if (auto move = evt->getIf<sf::Event::MouseMoved>()) 
		{
			OnMouseMoved(*move);
		}
	}

	bool ToolbarPanel::OnKeyPressed(const sf::Event::KeyPressed& key) 
	{
		if (!*m_ViewportHovered)
			return false;

		switch (key.scancode)
		{ 
		case sf::Keyboard::Scancode::Q:
			m_SelectedTool = Tool::SELECT; break;
		case sf::Keyboard::Scancode::W:
			m_SelectedTool = Tool::MOVE; break;
		case sf::Keyboard::Scancode::E:
			m_SelectedTool = Tool::ROTATE; break;
		case sf::Keyboard::Scancode::R:
			m_SelectedTool = Tool::SCALE; break;
		default:
			return false;
		}

		m_ToolStart = false;
		m_ToolUsing = false;
		return true;
	}

	bool ToolbarPanel::OnMouseButtonPressed(const sf::Event::MouseButtonPressed& mouse) 
	{
		if (mouse.button == sf::Mouse::Button::Left) 
		{
			m_ToolStart = *m_ViewportHovered;
			m_ToolUsing = *m_ViewportHovered;
		}
		return false;
	}

	bool ToolbarPanel::OnMouseButtonReleased(const sf::Event::MouseButtonReleased& mouse) 
	{
		if (mouse.button == sf::Mouse::Button::Left) 
		{
			m_ToolStart = false;
			m_ToolUsing = false;
		}
		return false;
	}

	bool ToolbarPanel::OnMouseMoved(const sf::Event::MouseMoved& move) 
	{
		if (m_ToolStart) 
		{
			m_MouseStart = Math::Vec2((float)move.position.x, (float)move.position.y);
			m_ToolStart = false;
		}
		return false;
	}

	void ToolbarPanel::ShowToolbar() 
	{
		ImVec2 pos;
		pos.x = m_ViewportBoundMin.x + m_Padding;
		pos.y = m_ViewportBoundMin.y + m_Padding;
		ImGui::SetNextWindowPos(pos);
		ImGui::SetNextWindowBgAlpha(0.65f);

		ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(m_ToolbarMinSize, m_ToolbarMinSize));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(2, 2));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0));
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.1f, 0.1f, 0.5f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.3f, 0.5f));

		constexpr ImGuiWindowFlags flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse |
			ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDecoration |
			ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoNavInputs;

		if (not ImGui::Begin("##Toolbar", nullptr, flags)) {
			ImGui::End();
			ImGui::PopStyleVar(4);
			ImGui::PopStyleColor(3);
			return;
		}

		if (ToolImageButton(EditorResources::Select, Tool::SELECT)) {
			m_SelectedTool = Tool::SELECT;
		}

		ImGui::SameLine();
		if (ToolImageButton(EditorResources::Move, Tool::MOVE)) {
			m_SelectedTool = Tool::MOVE;
		}

		ImGui::SameLine();
		if (ToolImageButton(EditorResources::Rotate, Tool::ROTATE)) {
			m_SelectedTool = Tool::ROTATE;
		}

		ImGui::SameLine();
		if (ToolImageButton(EditorResources::Scale, Tool::SCALE)) {
			m_SelectedTool = Tool::SCALE;
		}

		ImGui::End();
		ImGui::PopStyleVar(4);
		ImGui::PopStyleColor(3);
	}

	void ToolbarPanel::HandlePickEntityWithMouse()
	{
		if (!m_ToolUsing)
			return;

		ImVec2 mousePos = ImGui::GetMousePos();
		mousePos.x -= m_ViewportBoundMin.x;
		mousePos.y -= m_ViewportBoundMin.y;

		ImVec2 viewportSize = {
			m_ViewportBoundMax.x - m_ViewportBoundMin.x,
			m_ViewportBoundMax.y - m_ViewportBoundMin.y
		};

		int mouseX = (int)mousePos.x;
		int mouseY = (int)(viewportSize.y - mousePos.y);

		if (mouseX < 0 || mouseY < 0 || mouseX >= (int)viewportSize.x || mouseY >= (int)viewportSize.y) 
		{
			m_ToolUsing = false;
			return;
		}

		sf::Vector2i pixel(mouseX, mouseY);
		sf::Vector2f world = m_RenderWindow->mapPixelToCoords(pixel, m_Context->GetView());

		Entity selected;

		auto& manager = m_Context->GetEntityManager();
		for (auto& entity : manager.GetEntities())
		{
			if (!entity.Has<CTransform>() || !entity.Has<CBoundingBox>())
				continue;

			auto& tr = entity.Get<CTransform>();
			auto& bb = entity.Get<CBoundingBox>();

			Math::Vec2 min = tr.pos - bb.halfSize;
			Math::Vec2 max = tr.pos + bb.halfSize;

			if (world.x >= min.x && world.x <= max.x &&
				world.y >= min.y && world.y <= max.y)
			{
				selected = entity;
				break;
			}
		}

		if (selected)
			m_SceneHierarchyPanel->SetSelectedEntity(selected);
		else
			m_SceneHierarchyPanel->SetSelectedEntity(Entity());

		m_ToolUsing = false;
	}

	Math::Vec2 ToolbarPanel::GetMouseDelta()
	{
		sf::Vector2i pixel = sf::Mouse::getPosition();
		sf::Vector2f world = m_RenderWindow->mapPixelToCoords(pixel, m_RenderWindow->getView());

		Math::Vec2 currentPos(world.x, world.y);

		Math::Vec2 diff = currentPos - m_MouseStart;

		m_MouseStart = currentPos;

		return diff;
	}


	void ToolbarPanel::Moving()
	{
		if (!m_ToolUsing || m_ToolStart)
			return;

		if (m_SceneHierarchyPanel->IsSelectedEntityValid())
		{
			Math::Vec2 diff = GetMouseDelta();

			Math::Vec2& entityPosition = m_SceneHierarchyPanel->GetSelectedEntity().Get<CTransform>().pos;
			entityPosition.x += diff.x;
			entityPosition.y += diff.y;
		}
	}

	void ToolbarPanel::Scaling()
	{
		if (!m_ToolUsing || m_ToolStart)
			return;

		if (m_SceneHierarchyPanel->IsSelectedEntityValid()) 
		{
			Math::Vec2 diff = GetMouseDelta();

			Math::Vec2& entityScale = m_SceneHierarchyPanel->GetSelectedEntity().Get<CTransform>().scale;
			entityScale.x += diff.x;
			entityScale.y += diff.y;
		}
	}

	void ToolbarPanel::Rotating()
	{
		if (!m_ToolUsing || m_ToolStart)
			return;

		if (m_SceneHierarchyPanel->IsSelectedEntityValid())
		{
			Math::Vec2 diff = GetMouseDelta();

			float rotationDirection = (diff.x - diff.y > 0) ? 1.0f : -1.0f;

			const float sensitivity = 0.05f;
			float rotationAngle = diff.Length() * rotationDirection * sensitivity;

			auto& transform = m_SceneHierarchyPanel->GetSelectedEntity().Get<CTransform>();

			transform.angle += rotationAngle;

			if (transform.angle > 180.0f)
				transform.angle -= 360.0f;
			else if (transform.angle < -180.0f)
				transform.angle += 360.0f;
		}
	}

	bool ToolbarPanel::ToolImageButton(const ResourceHandle& textureHandle, Tool tool) 
	{
		float size = m_ToolbarMinSize - m_Padding;
		auto texture = std::static_pointer_cast<Texture>(Project::GetEditorResourceManager()->GetResource(textureHandle));
		ImTextureID textureId = (ImTextureID)texture->GetTexture().getNativeHandle();

		ImVec4 tintColor = ImVec4(1, 1, 1, 1);
		if (m_SelectedTool == tool) 
		{
			tintColor = ImVec4(0.6f, 0.6f, 0.9f, 1.0f);
		}

		return ImGui::ImageButton("ToolImage", textureId, ImVec2(size, size), ImVec2(0, 1), ImVec2(1, 0), ImVec4(0, 0, 0, 0), tintColor);
	}
}