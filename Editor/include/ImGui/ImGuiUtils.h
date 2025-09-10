#pragma once

#include <string_view>

namespace Luden
{
	namespace ImGuiUtils
	{
		void PrefixLabel(std::string_view title);

		template <typename T>
		bool ResourceButton(ResourceHandle& handle) {
			bool pressed = false;

			std::string label;
			std::string tooltip = "Select Resource";

			bool hadValidResource = false;
			if (handle && ResourceManager::IsResourceHandleValid(handle)) {
				hadValidResource = true;
				const ResourceMetadata& metadata = Project::GetEditorResourceManager()->GetMetadata(handle);
				label = metadata.FilePath.filename().string();
				tooltip = Project::GetRelativePath(metadata.FilePath).string();
			}
			else {
				label = FileMetadata::GetPlaceholder<T>();
			}

			FileMetadata::ColorFileText<T>();

			if (ImGui::Button(label.c_str())) {
				if (hadValidResource) {
					pressed = true;
				}
				else {
					DialogFile::OpenDialog(
						DialogType::OPEN_FILE, [&handle]() {
							handle = Project::GetEditorResourceManager()->ImportResource(DialogFile::GetSelectedPath());
						}
						, FileMetadata::GetFileExtension<T>()
					);
				}
			}

			if (ImGui::BeginDragDropSource()) {
				const ResourceMetadata& metadata = Project::GetEditorResourceManager()->GetMetadata(handle);
				ImGui::SetDragDropPayload("DND_FILE_PATH", metadata.FilePath.string().c_str(), metadata.FilePath.string().length() + 1);
				ImGui::Text("%s", metadata.FilePath.filename().string().c_str());
				ImGui::EndDragDropSource();
			}

			if (ImGui::IsItemHovered()) {
				ImGui::SetTooltip(tooltip.c_str());
				if (ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
					ImGui::OpenPopup("ResourceRC");
				}
			}

			ImGui::PopStyleColor();

			if (ImGui::BeginPopup("ResourceRC")) {
				if (ImGui::MenuItem("Select Resource")) {
					DialogFile::OpenDialog(
						DialogType::OPEN_FILE, [&handle]() {
							handle = Project::GetEditorResourceManager()->ImportResource(DialogFile::GetSelectedPath());
						}
						, FileMetadata::GetFileExtension<T>()
					);
				}
				if (handle) {
					if (ImGui::MenuItem("Remove Resource")) {
						handle = 0;
					}
				}
				ImGui::EndPopup();
			}

			if (ImGui::BeginDragDropTarget()) {
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_FILE_PATH")) {
					std::filesystem::path path = std::filesystem::path(static_cast<const char*>(payload->Data));
					if (path.has_extension() && path.extension() == FileMetadata::GetFileExtension<T>()) {
						handle = Project::GetEditorResourceManager()->ImportResource(Project::GetAbsolutePath(path));
					}
				}
				ImGui::EndDragDropTarget();
			}

			return pressed;
		}
	}
}