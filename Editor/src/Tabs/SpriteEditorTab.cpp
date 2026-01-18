#include "Tabs/SpriteEditorTab.h"
#include "Resource/ResourceImporter.h"
#include "Project/Project.h"
#include "IO/FileSystem.h"

#include <IconsFontAwesome7.h>
#include <imgui.h>
#include <imgui_internal.h>
#include "SFML/Graphics/Sprite.hpp"
#include "SFML/Graphics/CircleShape.hpp"
#include "SFML/Graphics/RectangleShape.hpp"

namespace Luden
{
	SpriteEditorTab::SpriteEditorTab(const std::filesystem::path& filepath)
		: EditorTab(filepath)
		, m_ToolbarPanelName("Toolbar##" + std::to_string(m_TabID))
		, m_TexturePanelName("Texture##" + std::to_string(m_TabID))
		, m_RectEditorPanelName("Rect Editor##" + std::to_string(m_TabID))
		, m_PivotEditorPanelName("Pivot##" + std::to_string(m_TabID))
		, m_PreviewPanelName("Preview##" + std::to_string(m_TabID))
	{
		m_Sprite = std::make_shared<Sprite>();
		m_PreviewTexture = std::make_shared<sf::RenderTexture>();

		m_ResourceBrowserPanel.SetContext(m_EditorApplication);

		SetWindowName(filepath.filename().string());
		LoadSprite(filepath);
	}

	void SpriteEditorTab::RenderContent()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
		if (ImGui::Begin(m_ToolbarPanelName.c_str()))
		{
			RenderToolbar();
		}
		ImGui::End();
		ImGui::PopStyleVar();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
		if (ImGui::Begin(m_TexturePanelName.c_str()))
		{
			RenderTextureSelector();
		}
		ImGui::End();
		ImGui::PopStyleVar();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
		if (ImGui::Begin(m_RectEditorPanelName.c_str()))
		{
			RenderRectEditor();
		}
		ImGui::End();
		ImGui::PopStyleVar();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
		if (ImGui::Begin(m_PivotEditorPanelName.c_str()))
		{
			RenderPivotEditor();
		}
		ImGui::End();
		ImGui::PopStyleVar();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
		if (ImGui::Begin(m_PreviewPanelName.c_str()))
		{
			RenderPreview();
		}
		ImGui::End();
		ImGui::PopStyleVar();

		m_ResourceBrowserPanel.OnImGuiRender();
	}

	void SpriteEditorTab::InitializeDockspace()
	{
		m_ResourceBrowserPanel.SetContext(m_EditorApplication);

		ImGuiID dockMain = m_DockspaceID;

		ImGuiID dockTop = ImGui::DockBuilderSplitNode(dockMain, ImGuiDir_Up, 0.08f, nullptr, &dockMain);
		ImGuiID dockLeft = ImGui::DockBuilderSplitNode(dockMain, ImGuiDir_Left, 0.25f, nullptr, &dockMain);
		ImGuiID dockRight = ImGui::DockBuilderSplitNode(dockMain, ImGuiDir_Right, 0.25f, nullptr, &dockMain);
		ImGuiID dockBottom = ImGui::DockBuilderSplitNode(dockMain, ImGuiDir_Down, 0.25f, nullptr, &dockMain);

		ImGui::DockBuilderDockWindow(m_ToolbarPanelName.c_str(), dockTop);
		ImGui::DockBuilderDockWindow(m_TexturePanelName.c_str(), dockLeft);
		ImGui::DockBuilderDockWindow(m_PreviewPanelName.c_str(), dockMain);
		ImGui::DockBuilderDockWindow(m_RectEditorPanelName.c_str(), dockRight);
		ImGui::DockBuilderDockWindow(m_PivotEditorPanelName.c_str(), dockRight);
		m_ResourceBrowserPanel.DockTo(dockBottom);

		ImGui::DockBuilderFinish(dockMain);
	}

	void SpriteEditorTab::OnEvent(const std::optional<sf::Event>& evt)
	{
	}

	void SpriteEditorTab::OnUpdate(TimeStep timestep)
	{
	}

	void SpriteEditorTab::SaveSprite()
	{
		ResourceImporter::Serialize(m_Sprite);
		m_IsDirty = false;
	}

	void SpriteEditorTab::LoadSprite(const std::filesystem::path& filepath)
	{
		m_FilePath = filepath;

		if (FileSystem::Exists(filepath))
		{
			ResourceHandle handle = Project::GetEditorResourceManager()->GetResourceHandleFromFilePath(filepath);
			m_Sprite = ResourceManager::GetResource<Sprite>(handle);

			
			if (m_Sprite->GetTexture() != nullptr)
			{
				m_CurrentTexture = m_Sprite->GetTexture();
			}
		}
	}

	void SpriteEditorTab::RenderToolbar()
	{
		if (ImGui::Button(ICON_FA_FLOPPY_DISK " Save"))
		{
			SaveSprite();
		}

		ImGui::SameLine();
		ImGui::Text("Sprite Editor");
	}

	void SpriteEditorTab::RenderTextureSelector()
	{
		ImGui::Text(ICON_FA_IMAGE " Texture");
		ImGui::Separator();

		if (ImGui::Button("Select Texture", ImVec2(-1, 0)))
		{
			ImGui::OpenPopup("TextureSelectPopup");
		}

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_FILE_PATH"))
			{
				const char* droppedPath = static_cast<const char*>(payload->Data);
				std::filesystem::path filepath(droppedPath);

				auto resourceManager = Project::GetEditorResourceManager();
				ResourceType type = resourceManager->GetResourceTypeFromPath(filepath);

				if (type == ResourceType::Texture)
				{
					ResourceHandle handle = resourceManager->GetResourceHandleFromFilePath(filepath);
					auto texture = ResourceManager::GetResource<Texture>(handle);

					if (texture)
					{
						m_Sprite->SetTextureHandle(handle);
						m_CurrentTexture = texture;
						m_IsDirty = true;
					}
				}
			}
			ImGui::EndDragDropTarget();
		}

		if (ImGui::BeginPopup("TextureSelectPopup"))
		{
			ImGui::Text("Select a texture");
			ImGui::Separator();

			ImGui::BeginChild("TextureList", ImVec2(400, 300), true);
			{
				auto resourceManager = Project::GetEditorResourceManager();
				auto textureHandles = resourceManager->GetAllResourcesWithType(ResourceType::Texture);

				for (const auto& handle : textureHandles)
				{
					auto texture = ResourceManager::GetResource<Texture>(handle);
					if (!texture)
						continue;

					ImGui::PushID((int)handle);

					auto filepath = resourceManager->GetFileSystemPath(handle);
					std::string filename = filepath.filename().string();

					ImGui::BeginGroup();
					{
						ImGui::Image(texture->GetTexture().getNativeHandle(), ImVec2(64, 64));

						if (ImGui::IsItemHovered() && ImGui::IsItemClicked())
						{
							m_Sprite->SetTextureHandle(handle);
							m_CurrentTexture = texture;
							m_IsDirty = true;
							ImGui::CloseCurrentPopup();
						}

						ImGui::TextWrapped("%s", filename.c_str());
					}
					ImGui::EndGroup();

					ImGui::SameLine();

					ImGui::PopID();
				}
			}
			ImGui::EndChild();

			ImGui::EndPopup();
		}

		if (m_CurrentTexture)
		{
			ImGui::Separator();
			ImGui::Text("Current Texture:");
			ImGui::Text("  Handle: %llu", m_Sprite->GetTextureHandle());
			ImGui::Text("  Size: %dx%d",
				m_CurrentTexture->GetTexture().getSize().x,
				m_CurrentTexture->GetTexture().getSize().y);
		}
		else
		{
			ImGui::TextDisabled("No texture selected");
			ImGui::TextDisabled("(Drag & drop a texture here)");
		}
	}

	void SpriteEditorTab::RenderRectEditor()
	{
		ImGui::Text(ICON_FA_CROP " Texture Rect");
		ImGui::Separator();

		auto rect = m_Sprite->GetTextureRect();
		bool usesFullTexture = m_Sprite->UsesFullTexture();

		if (ImGui::Checkbox("Use Full Texture", &usesFullTexture))
		{
			if (usesFullTexture)
			{
				m_Sprite->SetTextureRect(sf::IntRect({ 0, 0 }, { 0, 0 }));
			}
			else if (m_CurrentTexture)
			{
				auto texSize = m_CurrentTexture->GetTexture().getSize();
				m_Sprite->SetTextureRect(sf::IntRect({ 0, 0 }, { (int)texSize.x, (int)texSize.y }));
			}
			m_IsDirty = true;
		}

		if (!usesFullTexture)
		{
			int rectData[4] = {
				rect.position.x, rect.position.y,
				rect.size.x, rect.size.y
			};

			if (ImGui::DragInt4("Rect (X, Y, W, H)", rectData, 1.0f, 0, 4096))
			{
				m_Sprite->SetTextureRect(sf::IntRect(
					{ rectData[0], rectData[1] },
					{ rectData[2], rectData[3] }
				));
				m_IsDirty = true;
			}

			ImGui::Separator();
			ImGui::Text("Grid Helper");

			ImGui::DragInt("Columns", &m_GridColumns, 1, 1, 32);
			ImGui::DragInt("Rows", &m_GridRows, 1, 1, 32);

			if (m_CurrentTexture && m_GridColumns > 0 && m_GridRows > 0)
			{
				auto texSize = m_CurrentTexture->GetTexture().getSize();
				int cellW = texSize.x / m_GridColumns;
				int cellH = texSize.y / m_GridRows;

				ImGui::Text("Cell Size: %dx%d", cellW, cellH);

				ImGui::DragInt("Grid X", &m_SelectedGridX, 1, 0, m_GridColumns - 1);
				ImGui::DragInt("Grid Y", &m_SelectedGridY, 1, 0, m_GridRows - 1);

				if (ImGui::Button("Apply Grid Selection"))
				{
					m_Sprite->SetTextureRect(sf::IntRect(
						{ m_SelectedGridX * cellW, m_SelectedGridY * cellH },
						{ cellW, cellH }
					));
					m_IsDirty = true;
				}
			}
		}
	}

	void SpriteEditorTab::RenderPivotEditor()
	{
		ImGui::Text(ICON_FA_CROSSHAIRS " Pivot");
		ImGui::Separator();

		auto pivot = m_Sprite->GetPivot();
		float pivotData[2] = { pivot.x, pivot.y };

		if (ImGui::DragFloat2("Pivot", pivotData, 0.01f, 0.0f, 1.0f))
		{
			m_Sprite->SetPivot({ pivotData[0], pivotData[1] });
			m_IsDirty = true;
		}

		ImGui::Text("Presets:");

		if (ImGui::Button("Center"))
		{
			m_Sprite->SetPivot({ 0.5f, 0.5f });
			m_IsDirty = true;
		}
		ImGui::SameLine();

		if (ImGui::Button("Bottom Center"))
		{
			m_Sprite->SetPivot({ 0.5f, 1.0f });
			m_IsDirty = true;
		}

		if (ImGui::Button("Top Left"))
		{
			m_Sprite->SetPivot({ 0.0f, 0.0f });
			m_IsDirty = true;
		}
		ImGui::SameLine();

		if (ImGui::Button("Top Right"))
		{
			m_Sprite->SetPivot({ 1.0f, 0.0f });
			m_IsDirty = true;
		}

		ImGui::Separator();

		ImGui::Checkbox("Show Pivot Gizmo", &m_ShowPivotGizmo);
	}

	void SpriteEditorTab::RenderPreview()
	{
		ImGui::Text(ICON_FA_EYE " Preview");
		ImGui::Separator();

		ImGui::Checkbox("Show Grid", &m_ShowGrid);
		ImGui::SameLine();
		ImGui::DragFloat("Zoom", &m_PreviewZoom, 0.1f, 0.5f, 10.0f);

		ImGui::Separator();

		if (!m_PreviewTexture->resize({ 512, 512 }))
			return;

		m_PreviewTexture->clear(sf::Color(50, 50, 50, 255));

		if (m_CurrentTexture)
		{
			sf::Sprite sprite(m_CurrentTexture->GetTexture());

			if (!m_Sprite->UsesFullTexture())
			{
				sprite.setTextureRect(m_Sprite->GetTextureRect());
			}

			sf::FloatRect bounds = sprite.getLocalBounds();

			sprite.setOrigin(
				{ bounds.size.x * m_Sprite->GetPivot().x,
				bounds.size.y * m_Sprite->GetPivot().y }
			);

			sprite.setPosition({ 256, 256 });
			sprite.setScale({ m_PreviewZoom, -m_PreviewZoom });

			m_PreviewTexture->draw(sprite);

			if (m_ShowPivotGizmo)
			{
				sf::CircleShape pivotDot(5);
				pivotDot.setFillColor(sf::Color::Red);
				pivotDot.setOrigin({ 5, 5 });
				pivotDot.setPosition({ 256, 256 });
				m_PreviewTexture->draw(pivotDot);

				sf::RectangleShape hLine(sf::Vector2f(20, 2));
				hLine.setFillColor(sf::Color::Red);
				hLine.setOrigin({ 10, 1 });
				hLine.setPosition({ 256, 256 });
				m_PreviewTexture->draw(hLine);

				sf::RectangleShape vLine(sf::Vector2f(2, 20));
				vLine.setFillColor(sf::Color::Red);
				vLine.setOrigin({ 1, 10 });
				vLine.setPosition({ 256, 256 });
				m_PreviewTexture->draw(vLine);
			}
		}

		if (m_ShowGrid)
		{
			const int gridSize = 32; 
			const int textureSize = 512;
			const int numLines = textureSize / gridSize;

			for (int i = 0; i <= numLines; i++)
			{
				sf::RectangleShape line(sf::Vector2f(textureSize, 1));
				line.setFillColor(sf::Color(80, 80, 80, 100));
				line.setPosition({ 0.0f, static_cast<float>(i * gridSize) });
				m_PreviewTexture->draw(line);
			}

			for (int i = 0; i <= numLines; i++)
			{
				sf::RectangleShape line(sf::Vector2f(1, textureSize));
				line.setFillColor(sf::Color(80, 80, 80, 100));
				line.setPosition({ static_cast<float>(i * gridSize), 0.0f });
				m_PreviewTexture->draw(line);
			}
		}

		m_PreviewTexture->display();

		ImVec2 availSize = ImGui::GetContentRegionAvail();
		ImGui::Image(m_PreviewTexture->getTexture().getNativeHandle(),
			ImVec2(availSize.x, availSize.y));
	}
}