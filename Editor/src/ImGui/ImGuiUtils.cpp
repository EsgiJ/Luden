#include "ImGui/ImGuiUtils.h"

#include <imgui.h>
#include <imgui_internal.h>

namespace Luden
{
	namespace ImGuiUtils
	{
		void PrefixLabel(std::string_view title)
		{
			ImGuiWindow* window = ImGui::GetCurrentWindow();
			const ImGuiStyle& style = ImGui::GetStyle();
			float full_width = ImGui::GetContentRegionAvail().x;
			float item_width = ImGui::CalcItemWidth() + style.ItemSpacing.x;
			ImVec2 text_size = ImGui::CalcTextSize(title.data());
			ImRect text_rect;
			text_rect.Min = ImGui::GetCursorScreenPos();
			text_rect.Max = text_rect.Min;
			text_rect.Max.x += full_width * 0.4f - item_width * 0;
			// 	text_rect.Max.x += full_width - item_width;
			text_rect.Max.y += text_size.y;

			ImGui::SetCursorScreenPos(text_rect.Min);

			ImGui::AlignTextToFramePadding();
			// Adjust text rect manually because we render it directly into a drawlist instead of using public functions.
			text_rect.Min.y += window->DC.CurrLineTextBaseOffset;
			text_rect.Max.y += window->DC.CurrLineTextBaseOffset + 1; // +1 so there is no letters bottom clip

			ImGui::ItemSize(text_rect);
			if (ImGui::ItemAdd(text_rect, window->GetID(title.data(), title.data() + title.size()))) {
				ImGui::RenderTextEllipsis(ImGui::GetWindowDrawList(), text_rect.Min, text_rect.Max, text_rect.Max.x, text_rect.Max.x, title.data(), title.data() + title.size(), &text_size);

				if (text_rect.GetWidth() < text_size.x && ImGui::IsItemHovered()) {
					ImGui::SetTooltip("%.*s", (int)title.size(), title.data());
				}
			}

			ImGui::SetCursorScreenPos(ImVec2(text_rect.Max.x, text_rect.Max.y - (text_size.y + window->DC.CurrLineTextBaseOffset)));
			ImGui::SameLine();
			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
		}

		bool ResourceButton(ResourceHandle& handle, ResourceType resourceType)
		{
			bool pressed = false;

			std::string label;
			std::string tooltip = "Select Resource";

			bool hadValidResource = false;
			if (handle && ResourceManager::IsResourceHandleValid(handle))
			{
				hadValidResource = true;
				const ResourceMetadata& metadata = Project::GetEditorResourceManager()->GetMetadata(handle);
				label = metadata.FilePath.filename().string();
				tooltip = Project::GetEditorResourceManager()->GetRelativePath(metadata.FilePath).string();
			}
			else
			{
				label = FileMetadata::GetPlaceholder(resourceType);
			}

			FileMetadata::ColorFileText(resourceType);

			if (ImGui::Button(label.c_str()))
			{
				if (hadValidResource)
				{
					pressed = true;
				}
				else
				{
					auto filter = FileMetadata::GetDialogFilter(resourceType);
					auto path = FileSystem::OpenFileDialog({ filter });

					if (!path.empty())
						handle = Project::GetEditorResourceManager()->ImportResource(path);
				}
			}

			if (ImGui::BeginDragDropSource())
			{
				const ResourceMetadata& metadata = Project::GetEditorResourceManager()->GetMetadata(handle);
				ImGui::SetDragDropPayload("DND_FILE_PATH", metadata.FilePath.string().c_str(), metadata.FilePath.string().length() + 1);
				ImGui::Text("%s", metadata.FilePath.filename().string().c_str());
				ImGui::EndDragDropSource();
			}

			if (ImGui::IsItemHovered())
			{
				ImGui::SetTooltip(tooltip.c_str());
				if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
				{
					ImGui::OpenPopup("ResourceRC");
				}
			}

			ImGui::PopStyleColor();

			if (ImGui::BeginPopup("ResourceRC"))
			{
				if (ImGui::MenuItem("Select Resource"))
				{
					auto filter = FileMetadata::GetDialogFilter(resourceType);
					auto path = FileSystem::OpenFileDialog({ filter });

					if (!path.empty())
						handle = Project::GetEditorResourceManager()->ImportResource(path);
				}

				if (handle)
				{
					if (ImGui::MenuItem("Remove Resource"))
					{
						handle = 0;
					}
				}

				ImGui::EndPopup();
			}

			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_FILE_PATH"))
				{
					std::filesystem::path path = std::filesystem::path(static_cast<const char*>(payload->Data));
					if (FileSystem::Exists(path) && !FileSystem::IsDirectory(path))
					{
						if (path.extension() == FileMetadata::GetFileExtension(resourceType))
						{
							handle = Project::GetEditorResourceManager()->ImportResource(path);
						}
					}
				}
				ImGui::EndDragDropTarget();
			}

			return pressed;
		}
	}
}