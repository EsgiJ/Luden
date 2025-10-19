#include "Panels/ResourceBrowserPanel.h"
#include "Resource/Resource.h"
#include "Project/Project.h"

namespace Luden
{
	void ResourceBrowserPanel::RenderContent()
	{
		auto& resources = Project::GetEditorResourceManager()->GetResourceRegistry();

		if (m_CurrentDirectory.empty())
		{
			m_CurrentDirectory = Project::GetActiveResourceDirectory();
		}

		ImGui::SliderFloat("Icon Size", &m_IconSize, 32.0f, 128.0f, "%.0f");
		ImGui::SameLine();
		HelpMarker("Use CTRL+Wheel to zoom");

		ImGui::Separator();

		CollectAndSortEntries(m_Entries);

		if (m_CurrentDirectory != Project::GetActiveResourceDirectory() && m_CurrentDirectory.has_parent_path())
		{
			if (ImGui::Button(".."))
			{
				m_CurrentDirectory = m_CurrentDirectory.parent_path();
				m_Selection.clear();
			}
			ImGui::SameLine();
		}
		ImGui::Text("Current Path: %s", m_CurrentDirectory.string().c_str());
		ImGui::Separator();

		const float availdWidth = ImGui::GetContentRegionAvail().x;
		UpdateLayoutSizes(availdWidth);

		ImGui::SetNextWindowContentSize(ImVec2(0.0f, m_LayoutOuterPadding + m_LayoutLineCount * (m_LayoutItemStep.y)));
		if (ImGui::BeginChild("AssetsChild", ImVec2(0.0f, -ImGui::GetTextLineHeightWithSpacing()), ImGuiChildFlags_Border, ImGuiWindowFlags_NoMove))
		{
			bool directoryChanged = false; 

			ImVec2 startPos = ImGui::GetCursorScreenPos();
			startPos = ImVec2(startPos.x + m_LayoutOuterPadding, startPos.y + m_LayoutOuterPadding);
			ImGui::SetCursorScreenPos(startPos);

			ImDrawList* drawList = ImGui::GetWindowDrawList();

			const int columnCount = m_LayoutColumnCount;
			const int itemCount = (int)m_Entries.size();
			const bool displayLabel = (m_LayoutItemSize.x >= ImGui::CalcTextSize("999999").x);

			ImGuiListClipper clipper;
			clipper.Begin(m_LayoutLineCount, m_LayoutItemStep.y);
			while (clipper.Step())
			{
				const int startLine = clipper.DisplayStart;
				const int endLine = clipper.DisplayEnd;

				const int itemMinIdx = startLine * columnCount;
				const int itemMaxIdx = std::min(endLine * columnCount, itemCount);

				for (int itemIdx = itemMinIdx; itemIdx < itemMaxIdx; itemIdx++)
				{
					if (itemIdx >= itemCount) break; 

					const BrowserEntry& entry = m_Entries[itemIdx];

					ImGui::PushID(entry.Filename.c_str());

					int lineIdx = itemIdx / columnCount;
					ImVec2 pos = ImVec2(startPos.x + (itemIdx % columnCount) * m_LayoutItemStep.x, startPos.y + lineIdx * m_LayoutItemStep.y);
					ImGui::SetCursorScreenPos(pos);

					bool isSelected = m_Selection.count(entry.Handle);
					bool itemWasClicked = ImGui::Selectable("", isSelected, ImGuiSelectableFlags_AllowItemOverlap | ImGuiSelectableFlags_AllowDoubleClick, m_LayoutItemSize);

					if (itemWasClicked)
					{
						if (entry.IsDirectory && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
						{
							m_CurrentDirectory = entry.Path;
							m_Selection.clear();
							directoryChanged = true;
							ImGui::PopID();
							break; 
						}
						else
						{
							if (ImGui::GetIO().KeyCtrl)
							{
								if (isSelected)
									m_Selection.erase(entry.Handle);
								else
									m_Selection.insert(entry.Handle);
							}
							else
							{
								m_Selection.clear();
								m_Selection.insert(entry.Handle);
							}
						}
					}

					if (directoryChanged) break;

					if (!entry.IsDirectory && entry.Handle != 0 && ImGui::BeginDragDropSource())
					{
						ImGui::SetDragDropPayload("DND_RESOURCE_HANDLE", (void*)&entry.Handle, sizeof(ResourceHandle));
						ImGui::Text("Dragging: %s", entry.Filename.c_str());
						ImGui::EndDragDropSource();
					}

					if (ImGui::IsItemVisible())
					{
						ImVec2 boxMin(pos.x, pos.y);
						ImVec2 boxMax(pos.x + m_LayoutItemSize.x, pos.y + m_LayoutItemSize.y);

						ImU32 iconBGColor;

						if (entry.IsDirectory)
						{
							iconBGColor = ImGui::GetColorU32(IM_COL32(255, 200, 0, 220));
							drawList->AddRectFilled(boxMin, boxMax, iconBGColor);
						}
						else if (entry.Type == ResourceType::Texture)
						{
							iconBGColor = ImGui::GetColorU32(IM_COL32(80, 0, 0, 220));
							drawList->AddRectFilled(boxMin, boxMax, iconBGColor);
							/*
							std::shared_ptr<Resource> resource = resourceManager->GetResource(entry.Handle);

							if (resource && resource->IsValid())
							{
								ImTextureID textureID = GetImGuiTextureID(entry.Handle); // Gerçek SFML ID'si alýnacak

								ImGui::SetCursorScreenPos(boxMin);
								ImGui::Image(textureID, m_LayoutItemSize, ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 1, 1, 1), ImVec4(0, 0, 0, 0));
							}
							else
							{
								iconBGColor = ImGui::GetColorU32(IM_COL32(80, 0, 0, 220));
								drawList->AddRectFilled(boxMin, boxMax, iconBGColor);
							}
							*/
						}
						else
						{
							iconBGColor = ImGui::GetColorU32(IM_COL32(35, 35, 35, 220));
							drawList->AddRectFilled(boxMin, boxMax, iconBGColor);
						}

						if (m_Selection.count(entry.Handle))
							drawList->AddRect(boxMin, boxMax, ImGui::GetColorU32(ImGuiCol_NavHighlight), 0.0f, 0, 2.0f);

						if (displayLabel)
						{
							ImU32 labelCol = ImGui::GetColorU32(m_Selection.count(entry.Handle) ? ImGuiCol_Text : ImGuiCol_TextDisabled);
							drawList->AddText(ImVec2(boxMin.x + 2, boxMax.y - ImGui::GetFontSize() - 2), labelCol, entry.Filename.c_str());
						}
					}

					ImGui::PopID();
				}
				if (directoryChanged) break; 
			}
			clipper.End();

			ImGuiIO& io = ImGui::GetIO();
			if (ImGui::IsWindowHovered() && io.MouseWheel != 0.0f && ImGui::IsKeyDown(ImGuiMod_Ctrl))
			{
				m_ZoomWheelAccum += io.MouseWheel;
				if (fabsf(m_ZoomWheelAccum) >= 1.0f)
				{
					m_IconSize *= powf(1.1f, (float)(int)m_ZoomWheelAccum);
					m_IconSize = std::clamp(m_IconSize, 32.0f, 128.0f);
					m_ZoomWheelAccum -= (int)m_ZoomWheelAccum;

					UpdateLayoutSizes(availdWidth);
				}
			}
		}
		ImGui::EndChild();

		ImGui::Separator();
		ImGui::Text("Resources in Registry: %d", (int)resources.Count());
		ImGui::Text("Items in Current View: %d", (int)m_Entries.size());
		ImGui::Text("Selected: %d items", (int)m_Selection.size());
	}

	void ResourceBrowserPanel::UpdateLayoutSizes(float availWidth)
	{
		m_LayoutItemSpacing = (float)(m_IconSpacing);
		if (m_StretchSpacing == false)
			availWidth += floorf(m_LayoutItemSpacing * 0.5);

		m_LayoutItemSize = ImVec2(floorf(m_IconSize), floorf(m_IconSize));
		m_LayoutColumnCount = std::max((int)(availWidth / (m_LayoutItemSize.x + m_LayoutItemSpacing)), 1);
		m_LayoutLineCount = (m_Entries.size() + m_LayoutColumnCount - 1) / m_LayoutColumnCount;
		
		if (m_StretchSpacing && m_LayoutColumnCount > 1)
			m_LayoutItemSpacing = floorf(availWidth - m_LayoutItemSize.x * m_LayoutColumnCount) / m_LayoutColumnCount;
		
		m_LayoutItemStep = ImVec2(m_LayoutItemSize.x + m_LayoutItemSpacing, (m_LayoutItemSize.x + m_LayoutItemSpacing));
		m_LayoutSelectableSpacing = std::max(floorf(m_LayoutItemSpacing) - m_IconHitSpacing, 0.0f);
		m_LayoutOuterPadding = floorf(m_LayoutItemSpacing * 0.5f);
	}

	void ResourceBrowserPanel::CollectAndSortEntries(std::vector<BrowserEntry>& entries)
	{
		entries.clear();

		for (const auto& entry : std::filesystem::directory_iterator(m_CurrentDirectory))
		{
			const auto& path = entry.path();
			const std::string filename = path.filename().string();

			BrowserEntry newEntry;
			newEntry.Path = path;
			newEntry.Filename = filename;
			newEntry.IsDirectory = entry.is_directory();

			if (!newEntry.IsDirectory)
			{
				newEntry.Handle = Project::GetEditorResourceManager()->GetResourceHandleFromFilePath(path);
				newEntry.Type = Project::GetEditorResourceManager()->GetResourceTypeFromPath(path);
			}
			else
			{
				newEntry.Handle = 0; 
			}

			entries.push_back(std::move(newEntry));
		}

		std::sort(entries.begin(), entries.end(), [](const BrowserEntry& a, const BrowserEntry& b) {
			if (a.IsDirectory != b.IsDirectory)
				return a.IsDirectory;

			return a.Filename < b.Filename;
			});
	}

	void ResourceBrowserPanel::ReloadResources()
	{

	}

	void ResourceBrowserPanel::HelpMarker(const char* desc)
	{
		ImGui::TextDisabled("(?)");
		if (ImGui::BeginItemTooltip())
		{
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
			ImGui::TextUnformatted(desc);
			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();
		}
	}
}