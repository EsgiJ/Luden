#pragma once

#include "IO/FileSystem.h"
#include "Resource/Resource.h"
#include "Resource/ResourceMetadata.h"
#include "Resource/ResourceManager.h"
#include "Utils/FileMetadata.h"
#include "Project/Project.h"

#include <string_view>

namespace Luden
{
	namespace ImGuiUtils
	{
		void PrefixLabel(std::string_view title);

		bool ResourceButton(ResourceHandle& handle, ResourceType resourceType);

		inline bool DragFloat3Colored(const char* label, float* v, float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f)
		{
			ImGui::PushID(label);

			bool modified = false;
			float spacing = ImGui::GetStyle().ItemInnerSpacing.x;
			float width = (ImGui::GetContentRegionAvail().x - spacing * 2) / 3.0f;

			// X - Red
			ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.35f, 0.15f, 0.15f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.50f, 0.20f, 0.20f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.65f, 0.25f, 0.25f, 1.0f));
			ImGui::SetNextItemWidth(width);
			if (ImGui::DragFloat("##X", &v[0], v_speed, v_min, v_max, "X: %.2f")) modified = true;
			ImGui::PopStyleColor(3);

			ImGui::SameLine(0, spacing);

			// Y - Green
			ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.15f, 0.35f, 0.15f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.20f, 0.50f, 0.20f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.25f, 0.65f, 0.25f, 1.0f));
			ImGui::SetNextItemWidth(width);
			if (ImGui::DragFloat("##Y", &v[1], v_speed, v_min, v_max, "Y: %.2f")) modified = true;
			ImGui::PopStyleColor(3);

			ImGui::SameLine(0, spacing);

			// Z - Blue
			ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.15f, 0.15f, 0.35f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.20f, 0.20f, 0.50f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.25f, 0.25f, 0.65f, 1.0f));
			ImGui::SetNextItemWidth(width);
			if (ImGui::DragFloat("##Z", &v[2], v_speed, v_min, v_max, "Z: %.2f")) modified = true;
			ImGui::PopStyleColor(3);

			ImGui::PopID();
			return modified;
		}

		inline bool DragFloat2Colored(const char* label, float* v, float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f)
		{
			ImGui::PushID(label);

			bool modified = false;
			float spacing = ImGui::GetStyle().ItemInnerSpacing.x;
			float width = (ImGui::GetContentRegionAvail().x - spacing) / 2.0f;

			// X - Red
			ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.35f, 0.15f, 0.15f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.50f, 0.20f, 0.20f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.65f, 0.25f, 0.25f, 1.0f));
			ImGui::SetNextItemWidth(width);
			if (ImGui::DragFloat("##X", &v[0], v_speed, v_min, v_max, "X: %.2f")) modified = true;
			ImGui::PopStyleColor(3);

			ImGui::SameLine(0, spacing);

			// Y - Green
			ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.15f, 0.35f, 0.15f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.20f, 0.50f, 0.20f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.25f, 0.65f, 0.25f, 1.0f));
			ImGui::SetNextItemWidth(width);
			if (ImGui::DragFloat("##Y", &v[1], v_speed, v_min, v_max, "Y: %.2f")) modified = true;
			ImGui::PopStyleColor(3);

			ImGui::PopID();
			return modified;
		}
	}
}