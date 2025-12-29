#include "Tabs/Animation2DEditorTab.h"
#include "Resource/ResourceSerializer.h"
#include "Project/Project.h"
#include "IO/FileSystem.h"
#include "Utils/EditorColors.h"
#include "Resource/ResourceImporter.h"

#include <IconsFontAwesome7.h>
#include <imgui.h>
#include "SFML/Graphics/Sprite.hpp"
#include "SFML/Graphics/RectangleShape.hpp"
#include "glm/common.hpp"

namespace Luden
{
	AnimationEditorTab::AnimationEditorTab(const std::filesystem::path& filepath)
		: EditorTab(filepath)
		, m_ViewportPanelName("Viewport##" + std::to_string(m_TabID))
		, m_FrameListPanelName("Frame List##" + std::to_string(m_TabID))
		, m_PreviewPanelName("Preview##" + std::to_string(m_TabID))
		, m_PropertiesPanelName("Properties##" + std::to_string(m_TabID))
		, m_TimelinePanelName("Timeline##" + std::to_string(m_TabID))
		, m_ToolbarPanelName("Toolbar##" + std::to_string(m_TabID))  // NEW
	{
		m_Animation = std::make_shared<Animation>();
		m_PreviewTexture = std::make_shared<sf::RenderTexture>();

		SetWindowName(filepath.filename().string());
		LoadAnimation(filepath);
	}

	AnimationEditorTab::~AnimationEditorTab()
	{
		StopPreview();
	}

	void AnimationEditorTab::RenderContent()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
		if (ImGui::Begin(m_ToolbarPanelName.c_str()))
		{
			RenderToolbar();
		}
		ImGui::End();
		ImGui::PopStyleVar();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
		if (ImGui::Begin(m_FrameListPanelName.c_str()))
		{
			RenderFrameList();
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

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
		if (ImGui::Begin(m_PropertiesPanelName.c_str()))
		{
			RenderProperties();
		}
		ImGui::End();
		ImGui::PopStyleVar();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
		if (ImGui::Begin(m_TimelinePanelName.c_str()))
		{
			RenderTimeline();
		}
		ImGui::End();
		ImGui::PopStyleVar();
	}

	void AnimationEditorTab::RenderFrameList()
	{
		ImGui::Text(ICON_FA_LIST " Frames");
		ImGui::Separator();

		// Add frame button
		if (ImGui::Button(ICON_FA_PLUS " Add Frame", ImVec2(-1, 0)))
		{
			ImGui::OpenPopup("AddFramePopup");
		}

		if (ImGui::BeginPopup("AddFramePopup"))
		{
			ImGui::Text("Select a sprite");
			ImGui::Separator();

			// Drag & Drop target
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_FILE_PATH"))
				{
					std::filesystem::path path = std::filesystem::path(static_cast<const char*>(payload->Data));

					if (FileSystem::GetExtension(path) == ".lsprite")
					{
						ResourceHandle handle = Project::GetEditorResourceManager()->GetResourceHandleFromFilePath(path);
						m_Animation->AddFrame(handle, 0.1f);
						m_IsDirty = true;
						ImGui::CloseCurrentPopup();
					}
				}
				ImGui::EndDragDropTarget();
			}

			ImGui::Separator();

			// List all sprites
			ImGui::BeginChild("SpriteList", ImVec2(400, 300), true);
			{
				auto resourceManager = Project::GetEditorResourceManager();
				auto resourceHandles = resourceManager->GetAllResourcesWithType(ResourceType::Sprite);

				int spriteCount = 0;
				for (const auto& handle : resourceHandles)
				{
					auto sprite = ResourceManager::GetResource<Sprite>(handle);

					if (!sprite) continue;

					spriteCount++;
					ImGui::PushID((int)handle);

					std::filesystem::path filepath = resourceManager->GetFileSystemPath(handle);
					std::string filename = filepath.filename().string();

					ImGui::BeginGroup();
					{
						//TODO: fix this, just show the area that sprite rect covers
						ImGui::Image(sprite->GetTexture()->GetTexture().getNativeHandle(), ImVec2(64, 64));

						if (ImGui::IsItemHovered() && ImGui::IsItemClicked())
						{
							m_Animation->AddFrame(handle, 0.1f);
							m_IsDirty = true;
							ImGui::CloseCurrentPopup();
						}

						if (ImGui::IsItemHovered())
						{
							ImGui::BeginTooltip();
							ImGui::Text("Click to add frame");
							ImGui::Separator();
							ImGui::Text("File: %s", filepath.string().c_str());
							ImGui::Text("Size: %dx%d",
								sprite->GetSize().x,
								sprite->GetSize().y);
							ImGui::EndTooltip();
						}

						ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + 64);
						ImGui::TextWrapped("%s", filename.c_str());
						ImGui::PopTextWrapPos();
					}
					ImGui::EndGroup();

					if (spriteCount % 5 != 0)
						ImGui::SameLine();

					ImGui::PopID();
				}

				if (spriteCount == 0)
				{
					ImGui::TextDisabled("No sprites found");
				}
			}
			ImGui::EndChild();

			ImGui::EndPopup();
		}

		ImGui::Separator();

		// Render existing frames
		for (size_t i = 0; i < m_Animation->GetFrameCount(); i++)
		{
			ImGui::PushID((int)i);

			const auto& frame = m_Animation->GetFrame(i);
			auto sprite = m_Animation->GetSprite(i);

			bool isSelected = (m_SelectedFrameIndex == (int)i);

			ImGui::PushStyleColor(ImGuiCol_Button, isSelected ? ImVec4(0.26f, 0.59f, 0.98f, 0.40f) : ImVec4(0, 0, 0, 0));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.26f, 0.59f, 0.98f, 0.20f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.26f, 0.59f, 0.98f, 0.60f));

			bool clicked = ImGui::Button(("##FrameButton" + std::to_string(i)).c_str(), ImVec2(-1, 60));

			ImGui::PopStyleColor(3);

			if (clicked)
			{
				m_SelectedFrameIndex = (int)i;
			}

			ImVec2 itemMin = ImGui::GetItemRectMin();

			// Drag/Drop for reordering
			if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
			{
				m_IsDraggingFrame = true;
				m_DragSourceIndex = (int)i;
				ImGui::SetDragDropPayload("DND_ANIMATION_FRAME", &i, sizeof(size_t));
				ImGui::Text("Moving Frame %d", (int)i);
				ImGui::EndDragDropSource();
			}

			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_ANIMATION_FRAME"))
				{
					size_t sourceIdx = *(const size_t*)payload->Data;

					if (sourceIdx != i && sourceIdx < m_Animation->GetFrameCount())
					{
						auto movedFrame = m_Animation->GetFrame(sourceIdx);
						m_Animation->RemoveFrame(sourceIdx);
						m_Animation->InsertFrame(i, movedFrame.spriteHandle, movedFrame.duration);

						if (m_SelectedFrameIndex == (int)sourceIdx)
							m_SelectedFrameIndex = (int)i;
						else if (m_SelectedFrameIndex == (int)i)
							m_SelectedFrameIndex = sourceIdx < i ? (int)sourceIdx : (int)sourceIdx - 1;

						m_IsDirty = true;
					}
				}
				ImGui::EndDragDropTarget();
			}

			// Draw content overlay
			ImGui::SetCursorScreenPos(ImVec2(itemMin.x + 5, itemMin.y + 5));

			auto texture = sprite->GetTexture();
			if (texture)
			{
				ImGui::Image(texture->GetTexture().getNativeHandle(), ImVec2(50, 50));
			}
			else
			{
				ImGui::Dummy(ImVec2(50, 50));
			}

			ImGui::SameLine();

			ImGui::BeginGroup();
			ImGui::Text("Frame %d", (int)i);
			ImGui::Text("Duration: %.2fs", frame.duration);

			if (!sprite->UsesFullTexture())
			{
				ImGui::TextColored(ImVec4(0.5f, 1.0f, 0.5f, 1.0f), "Rect: %dx%d",
					sprite->GetTextureRect().size.x, sprite->GetTextureRect().size.y);
			}

			if (!texture)
			{
				ImGui::TextColored(ImVec4(1, 0.5f, 0, 1), "Missing!");
			}
			ImGui::EndGroup();

			if (ImGui::BeginPopupContextItem(("DebugContext" + std::to_string(i)).c_str()))
			{
				ImGui::Text("DEBUG CONTEXT");
				ImGui::EndPopup();
			}

			// Context menu
			if (ImGui::BeginPopupContextItem(("FrameContext" + std::to_string(i)).c_str()))
			{
				if (ImGui::MenuItem(ICON_FA_TRASH " Delete"))
				{
					m_Animation->RemoveFrame(i);
					if (m_SelectedFrameIndex == (int)i)
						m_SelectedFrameIndex = -1;
					else if (m_SelectedFrameIndex > (int)i)
						m_SelectedFrameIndex--;
					m_IsDirty = true;
				}

				if (ImGui::MenuItem(ICON_FA_COPY " Duplicate"))
				{
					auto& srcFrame = m_Animation->GetFrame(i);
					m_Animation->InsertFrame(i + 1, srcFrame.spriteHandle, srcFrame.duration);
					m_IsDirty = true;
				}

				ImGui::Separator();

				if (ImGui::MenuItem(ICON_FA_ARROW_UP " Move Up", nullptr, false, i > 0))
				{
					auto movedFrame = m_Animation->GetFrame(i);
					m_Animation->RemoveFrame(i);
					m_Animation->InsertFrame(i - 1, movedFrame.spriteHandle, movedFrame.duration);

					if (m_SelectedFrameIndex == (int)i)
						m_SelectedFrameIndex = (int)i - 1;
					m_IsDirty = true;
				}

				if (ImGui::MenuItem(ICON_FA_ARROW_DOWN " Move Down", nullptr, false,
					i < m_Animation->GetFrameCount() - 1))
				{
					auto movedFrame = m_Animation->GetFrame(i);
					m_Animation->RemoveFrame(i);
					m_Animation->InsertFrame(i + 1, movedFrame.spriteHandle, movedFrame.duration);

					if (m_SelectedFrameIndex == (int)i)
						m_SelectedFrameIndex = (int)i + 1;
					m_IsDirty = true;
				}

				ImGui::EndPopup();
			}

			ImGui::PopID();
		}
	}

	void AnimationEditorTab::RenderPreview()
	{
		ImGui::Text(ICON_FA_EYE " Preview");
		ImGui::Separator();

		ImGui::Checkbox("Show Grid", &m_ShowGrid);
		ImGui::SameLine();
		ImGui::SetNextItemWidth(100);
		ImGui::DragFloat("Zoom", &m_PreviewZoom, 0.1f, 0.1f, 10.0f);

		ImGui::Separator();

		if (!m_PreviewTexture->resize({ 512, 512 }))
		{
			ImGui::TextColored(ImVec4(1, 0, 0, 1), "Failed to create preview texture");
			return;
		}

		m_PreviewTexture->clear(sf::Color(50, 50, 50, 255));

		if (m_Animation->GetFrameCount() > 0 && m_CurrentFrameIndex < m_Animation->GetFrameCount())
		{
			auto sprite = m_Animation->GetSprite(m_CurrentFrameIndex);
			auto texture = sprite->GetTexture();

			if (texture)
			{
				sf::Sprite tempSprite(texture->GetTexture());

				if (!sprite->UsesFullTexture())
				{
					tempSprite.setTextureRect(sprite->GetTextureRect());
				}

				sf::Vector2u spriteSize = sprite->GetSize();

				tempSprite.setOrigin({ spriteSize.x / 2.0f, spriteSize.y / 2.0f });
				tempSprite.setPosition({ 256, 256 });
				tempSprite.setScale({ m_PreviewZoom, -m_PreviewZoom });

				m_PreviewTexture->draw(tempSprite);
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
			ImVec2(availSize.x, availSize.y - 10));
	}

	void AnimationEditorTab::RenderProperties()
	{
		ImGui::Text(ICON_FA_SLIDERS " Properties");
		ImGui::Separator();

		bool loop = m_Animation->IsLooping();
		if (ImGui::Checkbox("Loop", &loop))
		{
			m_Animation->SetLooping(loop);
			m_IsDirty = true;
		}

		ImGui::Text("Total Duration: %.2fs", m_Animation->GetTotalDuration());
		ImGui::Text("Frame Count: %d", (int)m_Animation->GetFrameCount());

		ImGui::Separator();

		if (m_SelectedFrameIndex >= 0 && m_SelectedFrameIndex < (int)m_Animation->GetFrameCount())
		{
			ImGui::Text("Frame %d Properties", m_SelectedFrameIndex);
			ImGui::Separator();

			auto& frame = m_Animation->GetFrame(m_SelectedFrameIndex);
			auto sprite = m_Animation->GetSprite(m_SelectedFrameIndex);

			if (ImGui::DragFloat("Duration", &frame.duration, 0.01f, 0.01f, 10.0f))
			{
				m_IsDirty = true;
			}

			if (ImGui::DragFloat2("Offset", &frame.offset.x, 1.0f))
			{
				m_IsDirty = true;
			}

			ImGui::Separator();
			ImGui::Text("Texture Rect");

			bool usesFullTexture = sprite->UsesFullTexture();

			if (ImGui::Checkbox("Use Full Texture", &usesFullTexture))
			{
				if (usesFullTexture)
				{
					sprite->SetTextureRect(sf::IntRect({ 0, 0 }, { 0, 0 }));
				}
				else
				{
					auto texture = sprite->GetTexture();
					if (texture)	
					{
						auto texSize = texture->GetTexture().getSize();
						sprite->SetTextureRect(sf::IntRect({ 0, 0 }, { static_cast<int>(texSize.x), static_cast<int>(texSize.y) }));
					}
				}
				m_IsDirty = true;
			}

			if (!usesFullTexture)
			{
				int rect[4] = {
					sprite->GetTextureRect().position.x,
					sprite->GetTextureRect().position.y,
					sprite->GetTextureRect().size.x,
					sprite->GetTextureRect().size.y
				};

				if (ImGui::DragInt4("Rect (X, Y, W, H)", rect, 1.0f, 0, 4096))
				{
					sprite->SetTextureRect(sf::IntRect({ rect[0], rect[1] }, { rect[2], rect[3] }));
					m_IsDirty = true;
				}

				ImGui::Separator();
				ImGui::Text("Grid Helper");

				static int gridX = 0, gridY = 0;
				static int spriteW = 32, spriteH = 32;

				ImGui::DragInt2("Grid Position (X, Y)", &gridX, 1, 0, 32);
				ImGui::DragInt2("Sprite Size (W, H)", &spriteW, 1, 1, 512);

				if (ImGui::Button("Apply Grid Position"))
				{
					sprite->SetTextureRect(sf::IntRect(
						{ gridX * spriteW, gridY * spriteH },
						{ spriteW, spriteH }
					));
					m_IsDirty = true;
				}
			}

			ImGui::Separator();

			auto texture = sprite->GetTexture();

			if (texture)
			{
				ImGui::Text("Texture Info:");
				ImGui::Text("  Full Size: %dx%d",
					texture->GetTexture().getSize().x,
					texture->GetTexture().getSize().y);
				ImGui::Text("  Handle: %llu", sprite->GetTextureHandle());
			}
			else
			{
				ImGui::TextColored(ImVec4(1, 0.5f, 0, 1), "Texture not loaded!");
			}
		}
		else
		{
			ImGui::TextDisabled("No frame selected");
			ImGui::Separator();
			ImGui::TextWrapped("Select a frame from the Frame List to edit its properties.");
		}
	}

	void AnimationEditorTab::RenderTimeline()
	{
		ImGui::Text(ICON_FA_CLOCK " Timeline");

		ImGui::SameLine();
		ImGui::SetNextItemWidth(100);
		if (ImGui::DragFloat("Zoom", &m_TimelineZoom, 0.1f, 0.1f, 5.0f))
		{
			m_TimelineZoom = glm::clamp(m_TimelineZoom, 0.1f, 5.0f);
		}

		ImGui::Separator();

		if (m_Animation->GetFrameCount() == 0)
		{
			ImGui::TextDisabled("No frames to display");
			ImGui::TextWrapped("Add frames using the Frame List panel.");
			return;
		}

		ImDrawList* drawList = ImGui::GetWindowDrawList();
		ImVec2 canvasPos = ImGui::GetCursorScreenPos();
		ImVec2 canvasSize = ImGui::GetContentRegionAvail();

		if (canvasSize.y < 100)
			canvasSize.y = 100;

		float totalDuration = m_Animation->GetTotalDuration();
		if (totalDuration <= 0.0f)
			totalDuration = 0.1f;

		float pixelsPerSecond = (canvasSize.x - 100) / totalDuration * m_TimelineZoom;

		drawList->AddRectFilled(
			canvasPos,
			ImVec2(canvasPos.x + canvasSize.x, canvasPos.y + 80),
			IM_COL32(40, 40, 40, 255)
		);

		float currentX = canvasPos.x + 50;
		float accumulatedTime = 0.0f;

		for (size_t i = 0; i < m_Animation->GetFrameCount(); i++)
		{
			const auto& frame = m_Animation->GetFrame(i);
			float frameWidth = frame.duration * pixelsPerSecond;

			if (frameWidth < 5.0f)
				frameWidth = 5.0f;

			bool isSelected = (m_SelectedFrameIndex == (int)i);
			bool isHovered = ImGui::IsMouseHoveringRect(
				ImVec2(currentX, canvasPos.y + 20),
				ImVec2(currentX + frameWidth, canvasPos.y + 60)
			);

			ImU32 color = isSelected ? IM_COL32(100, 150, 255, 255) :
				isHovered ? IM_COL32(90, 90, 90, 255) :
				IM_COL32(70, 70, 70, 255);

			drawList->AddRectFilled(
				ImVec2(currentX, canvasPos.y + 20),
				ImVec2(currentX + frameWidth, canvasPos.y + 60),
				color
			);

			drawList->AddRect(
				ImVec2(currentX, canvasPos.y + 20),
				ImVec2(currentX + frameWidth, canvasPos.y + 60),
				IM_COL32(255, 255, 255, isSelected ? 200 : 100)
			);

			char label[32];
			snprintf(label, sizeof(label), "%d", (int)i);
			drawList->AddText(
				ImVec2(currentX + 5, canvasPos.y + 25),
				IM_COL32(255, 255, 255, 255),
				label
			);

			char durationLabel[32];
			snprintf(durationLabel, sizeof(durationLabel), "%.2fs", frame.duration);
			drawList->AddText(
				ImVec2(currentX + 5, canvasPos.y + 42),
				IM_COL32(200, 200, 200, 255),
				durationLabel
			);

			if (isHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
			{
				m_SelectedFrameIndex = (int)i;
			}

			currentX += frameWidth;
			accumulatedTime += frame.duration;
		}

		if (totalDuration > 0.0f)
		{
			float playheadX = canvasPos.x + 50 + (m_CurrentTime / totalDuration) * (currentX - canvasPos.x - 50);
			drawList->AddLine(
				ImVec2(playheadX, canvasPos.y),
				ImVec2(playheadX, canvasPos.y + 80),
				IM_COL32(255, 0, 0, 255),
				2.0f
			);

			drawList->AddTriangleFilled(
				ImVec2(playheadX, canvasPos.y),
				ImVec2(playheadX - 5, canvasPos.y - 8),
				ImVec2(playheadX + 5, canvasPos.y - 8),
				IM_COL32(255, 0, 0, 255)
			);
		}

		ImGui::Dummy(ImVec2(canvasSize.x, 80));

		if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
		{
			ImVec2 mousePos = ImGui::GetMousePos();
			float relativeX = mousePos.x - canvasPos.x - 50;
			float clickedTime = (relativeX / (currentX - canvasPos.x - 50)) * totalDuration;

			m_CurrentTime = glm::clamp(clickedTime, 0.0f, totalDuration);

			float accTime = 0.0f;
			for (size_t i = 0; i < m_Animation->GetFrameCount(); i++)
			{
				accTime += m_Animation->GetFrame(i).duration;
				if (m_CurrentTime < accTime)
				{
					m_CurrentFrameIndex = i;
					break;
				}
			}
		}
	}

	void AnimationEditorTab::RenderToolbar()
	{
		if (ImGui::Button(ICON_FA_FLOPPY_DISK " Save"))
		{
			SaveAnimation();
		}

		ImGui::SameLine();

		if (ImGui::Button(m_IsPlaying ? ICON_FA_PAUSE " Pause" : ICON_FA_PLAY " Play"))
		{
			if (m_IsPlaying)
				PausePreview();
			else
				PlayPreview();
		}

		ImGui::SameLine();

		if (ImGui::Button(ICON_FA_STOP " Stop"))
		{
			StopPreview();
		}

		ImGui::SameLine();
		ImGui::SetNextItemWidth(100.0f);
		ImGui::DragFloat("Speed", &m_PlaybackSpeed, 0.1f, 0.1f, 5.0f);

		ImGui::SameLine();
		ImGui::Text("Frame: %d/%d", (int)m_CurrentFrameIndex + 1, (int)m_Animation->GetFrameCount());

		ImGui::SameLine();
		ImGui::Text("Time: %.2f/%.2f", m_CurrentTime, m_Animation->GetTotalDuration());
	}

	void AnimationEditorTab::OnUpdate(TimeStep timestep)
	{
		if (m_IsPlaying && !m_IsPaused && m_Animation->GetFrameCount() > 0)
		{
			m_CurrentTime += timestep * m_PlaybackSpeed;

			float totalDuration = m_Animation->GetTotalDuration();

			if (m_CurrentTime >= totalDuration)
			{
				if (m_Animation->IsLooping())
				{
					m_CurrentTime = fmod(m_CurrentTime, totalDuration);
				}
				else
				{
					m_CurrentTime = totalDuration;
					StopPreview();
					return;
				}
			}

			float accumulatedTime = 0.0f;
			for (size_t i = 0; i < m_Animation->GetFrameCount(); i++)
			{
				accumulatedTime += m_Animation->GetFrame(i).duration;
				if (m_CurrentTime < accumulatedTime)
				{
					m_CurrentFrameIndex = i;
					break;
				}
			}
		}
	}

	void AnimationEditorTab::PlayPreview()
	{
		m_IsPlaying = true;
		m_IsPaused = false;
	}

	void AnimationEditorTab::StopPreview()
	{
		m_IsPlaying = false;
		m_IsPaused = false;
		m_CurrentTime = 0.0f;
		m_CurrentFrameIndex = 0;
	}

	void AnimationEditorTab::PausePreview()
	{
		m_IsPaused = !m_IsPaused;
	}

	void AnimationEditorTab::SaveAnimation()
	{
		ResourceImporter::Serialize(m_Animation);
		m_IsDirty = false;
	}

	void AnimationEditorTab::LoadAnimation(const std::filesystem::path& filepath)
	{
		m_FilePath = filepath;

		if (FileSystem::Exists(filepath))
		{
			ResourceHandle handle = Project::GetEditorResourceManager()->GetResourceHandleFromFilePath(m_FilePath);
			m_Animation = ResourceManager::GetResource<Animation>(handle);
		}
	}

	void AnimationEditorTab::InitializeDockspace()
	{
		ImGuiID dockSpaceMainID = m_DockspaceID;

		ImGuiID dockTop = ImGui::DockBuilderSplitNode(dockSpaceMainID, ImGuiDir_Up, 0.08f, nullptr, &dockSpaceMainID);
		ImGuiID dockLeft = ImGui::DockBuilderSplitNode(dockSpaceMainID, ImGuiDir_Left, 0.25f, nullptr, &dockSpaceMainID);
		ImGuiID dockRight = ImGui::DockBuilderSplitNode(dockSpaceMainID, ImGuiDir_Right, 0.25f, nullptr, &dockSpaceMainID);
		ImGuiID dockDown = ImGui::DockBuilderSplitNode(dockSpaceMainID, ImGuiDir_Down, 0.30f, nullptr, &dockSpaceMainID);

		ImGui::DockBuilderDockWindow(m_ToolbarPanelName.c_str(), dockTop);     
		ImGui::DockBuilderDockWindow(m_FrameListPanelName.c_str(), dockLeft);
		ImGui::DockBuilderDockWindow(m_PreviewPanelName.c_str(), dockSpaceMainID);
		ImGui::DockBuilderDockWindow(m_PropertiesPanelName.c_str(), dockRight);
		ImGui::DockBuilderDockWindow(m_TimelinePanelName.c_str(), dockDown);

		ImGui::DockBuilderFinish(dockSpaceMainID);
	}

	void AnimationEditorTab::OnEvent(const std::optional<sf::Event>& evt)
	{
	}
}