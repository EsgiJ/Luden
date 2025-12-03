#include "Panels/ResourceBrowserPanel.h"
#include "Resource/Resource.h"
#include "Utils/EditorResources.h"
#include "Project/Project.h"
#include "Core/Config.h"
#include "Resource/ResourceSerializer.h"
#include "NativeScript/NativeScriptGenerator.h"
#include "Graphics/Animation.h"
#include "Core/EditorApplication.h"

#include <IconsFontAwesome7.h>


namespace Luden
{
	void ResourceBrowserPanel::SetContext(EditorApplication* editorApplication)
	{
		m_EditorApplication = editorApplication;
	}
	void ResourceBrowserPanel::RenderContent()
	{
		auto& resources = Project::GetEditorResourceManager()->GetResourceRegistry();

		if (m_CurrentDirectory.empty())
		{
			m_CurrentDirectory = Project::GetActiveResourceDirectory();
		}

		RenderFilterAndReloadToolbar();

		ImGui::Separator();

		CollectAndSortEntries(m_Entries);

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

					if (m_SelectedFilter != ResourceType::None && entry.Type != m_SelectedFilter && !entry.IsDirectory)
						continue;

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
						else if (!entry.IsDirectory && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
						{
							if (entry.Type == ResourceType::Animation && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
							{
								if (m_EditorApplication)
								{
									m_EditorApplication->RequestOpenResource(entry.Path);
								}
							}
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
						std::string path = entry.Path.string();
						const char* pathCStr = path.c_str();
						                                            
						ImGui::SetDragDropPayload("DND_FILE_PATH", pathCStr, path.size() + 1);

						ImGui::Text("Dragging: %s", entry.Filename.c_str());
						ImGui::EndDragDropSource();
					}

					if (ImGui::IsItemVisible())
					{
						ImVec2 boxMin(pos.x, pos.y);
						ImVec2 boxMax(pos.x + m_LayoutItemSize.x, pos.y + m_LayoutItemSize.y);

						ImU32 iconBGColor;

						ImTextureID finalTextureID = 0;
						std::shared_ptr<Texture> browseEntryTexture;

						if (entry.IsDirectory)
						{
							browseEntryTexture = EditorResources::FolderIcon;
						}
						switch (entry.Type)
						{
						case ResourceType::Scene: browseEntryTexture = EditorResources::SceneIcon; break;
						case ResourceType::Prefab: browseEntryTexture = EditorResources::Anim2DIcon; break; // Placeholder için Anim2DIcon
						case ResourceType::Texture: browseEntryTexture = std::dynamic_pointer_cast<Texture>(Project::GetEditorResourceManager()->GetResource(entry.Handle)); break;
						case ResourceType::Audio: browseEntryTexture = EditorResources::AudioIcon; break;
						case ResourceType::Font: browseEntryTexture = EditorResources::FontIcon; break;
						case ResourceType::Animation: browseEntryTexture = EditorResources::Anim2DIcon; break;
						case ResourceType::NativeScript: browseEntryTexture = EditorResources::NativeScriptIcon; break;
						default: break;
						}

						if (browseEntryTexture != nullptr && browseEntryTexture->GetTexture().getNativeHandle() != 0)
						{
							ImTextureID textureId = (ImTextureID)(uintptr_t)browseEntryTexture->GetTexture().getNativeHandle();

							drawList->AddImage(
								textureId,
								boxMin,
								boxMax
							);
						}
						else
						{
							ImU32 iconBGColor = ImGui::GetColorU32(IM_COL32(80, 80, 80, 220));
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
	}

	void ResourceBrowserPanel::UpdateLayoutSizes(float availWidth)
	{
		m_LayoutItemSpacing = (float)(m_IconSpacing);
		if (m_StretchSpacing == false)
			availWidth += floorf(m_LayoutItemSpacing * 0.5f);

		m_LayoutItemSize = ImVec2(floorf(m_IconSize), floorf(m_IconSize));
		m_LayoutColumnCount = std::max((int)(availWidth / (m_LayoutItemSize.x + m_LayoutItemSpacing)), 1);
		m_LayoutLineCount = ((int)m_Entries.size() + m_LayoutColumnCount - 1) / m_LayoutColumnCount;

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

			std::string searchFilterLower = m_SearchFilter;
			std::transform(searchFilterLower.begin(), searchFilterLower.end(), searchFilterLower.begin(), ::tolower);

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

			if (!m_SearchFilter.empty())
			{
				std::string filenameLower = filename;
				std::transform(filenameLower.begin(), filenameLower.end(), filenameLower.begin(), ::tolower);

				if (filenameLower.find(searchFilterLower) == std::string::npos)
					continue;
			}

			if (m_SelectedFilter != ResourceType::None && !newEntry.IsDirectory)
			{
				if (newEntry.Type != m_SelectedFilter)
					continue;
			}

			entries.push_back(std::move(newEntry));
		}

		std::sort(entries.begin(), entries.end(), [](const BrowserEntry& a, const BrowserEntry& b) {
			if (a.IsDirectory != b.IsDirectory)
				return a.IsDirectory;

			return a.Filename < b.Filename;
			});
	}

	void ResourceBrowserPanel::RenderFilterAndReloadToolbar()
	{
		ImGui::Text(ICON_FA_MAGNIFYING_GLASS);
		ImGui::SameLine();
		char buffer[256];
		strcpy_s(buffer, m_SearchFilter.c_str());
		ImGui::SetNextItemWidth(200.0f);
		if (ImGui::InputText("##SearchFilter", buffer, sizeof(buffer)))
		{
			m_SearchFilter = buffer;
		}

		static bool disabled = true;
		disabled = m_CurrentDirectory != Project::GetActiveResourceDirectory() && m_CurrentDirectory.has_parent_path();
		// Navigation bar
		ImGui::SameLine();
		ImGui::BeginDisabled(!disabled);
		ImGui::PushID("BackFolder");
		if (ImGui::Button(ICON_FA_ARROW_LEFT))
		{
			m_CurrentDirectory = m_CurrentDirectory.parent_path();
			m_Selection.clear();
		}
		ImGui::PopID();
		ImGui::EndDisabled();

		ImGui::SameLine();

		if (ImGui::Button(ICON_FA_FILE_CODE " New Script"))
		{
			ImGui::OpenPopup("CreateScriptDialog");
		}

		if (ImGui::BeginPopupModal("CreateScriptDialog", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
		{
			static char className[256] = "";
			static char sourcePath[256] = {};
			static char headerPath[256] = {};

			snprintf(sourcePath, sizeof(sourcePath), "%s/Source", Project::GetProjectDirectory().string().c_str());
			snprintf(headerPath, sizeof(headerPath), "%s/Source", Project::GetProjectDirectory().string().c_str());

			// UI
			ImGui::InputText("Class Name", className, sizeof(className));
			ImGui::InputText("Source Path", sourcePath, sizeof(sourcePath));
			ImGui::InputText("Header Path", headerPath, sizeof(headerPath));

			if (className[0] != '\0')
			{
				char generatedHeader[256];
				char generatedSource[256];

				snprintf(generatedHeader, sizeof(generatedHeader), "%s/%s.h", headerPath, className);
				snprintf(generatedSource, sizeof(generatedSource), "%s/%s.cpp", sourcePath, className);

				ImGui::Text("Generated Files:");
				ImGui::Text("Header: %s", generatedHeader);
				ImGui::Text("Source: %s", generatedSource);

				if (ImGui::Button("Create"))
				{
					NativeScriptGenerator::CreateNewScript(className, "", generatedSource, generatedHeader);
					className[0] = '\0';
					ImGui::CloseCurrentPopup();
				}
			}

			ImGui::SameLine();

			if (ImGui::Button("Cancel"))
			{
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}

		ImGui::SameLine();

		if (ImGui::Button(ICON_FA_FILM " New Animation"))
		{
			std::filesystem::path newPath = m_CurrentDirectory / "NewAnimation.lanim";

			auto newAnim = std::make_shared<Animation>();

			ResourceMetadata metadata;
			metadata.Handle = newAnim->Handle;
			metadata.FilePath = Project::GetEditorResourceManager()->GetRelativePath(newPath);
			AnimationResourceSerializer serializer;

			serializer.Serialize(metadata, newAnim);

			Project::GetEditorResourceManager()->ReloadResources();

			if (m_EditorApplication)
			{
				m_EditorApplication->RequestOpenResource(newPath);
			}
		}

		ImGui::SameLine(); 

		if (ImGui::Button(ICON_FA_ARROW_ROTATE_RIGHT " Reload"))
		{
			ReloadResources();
		}
		ImGui::SameLine();

		ImGui::SetNextItemWidth(70.0f);
		if (ImGui::BeginCombo("##ResourceFilter", "Filter", ImGuiComboFlags_NoArrowButton))
		{
			if (ImGui::Selectable("All", m_SelectedFilter == ResourceType::None))
			{
				m_SelectedFilter = ResourceType::None;
			}

			if (ImGui::Selectable("Scene", m_SelectedFilter == ResourceType::Scene))
			{
				m_SelectedFilter = ResourceType::Scene;
			}

			if (ImGui::Selectable("Prefab", m_SelectedFilter == ResourceType::Prefab))
			{
				m_SelectedFilter = ResourceType::Prefab;
			}

			if (ImGui::Selectable("Texture", m_SelectedFilter == ResourceType::Texture))
			{
				m_SelectedFilter = ResourceType::Texture;
			}

			if (ImGui::Selectable("Audio", m_SelectedFilter == ResourceType::Audio))
			{
				m_SelectedFilter = ResourceType::Audio;
			}

			if (ImGui::Selectable("Font", m_SelectedFilter == ResourceType::Font))
			{
				m_SelectedFilter = ResourceType::Font;
			}

			if (ImGui::Selectable("Animation", m_SelectedFilter == ResourceType::Animation))
			{
				m_SelectedFilter = ResourceType::Animation;
			}

			ImGui::EndCombo();
		}
	}

	void ResourceBrowserPanel::ReloadResources()
	{
		Project::GetEditorResourceManager()->ReloadResources();
		CollectAndSortEntries(m_Entries);
	}

	void ResourceBrowserPanel::ReloadResource()
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