#pragma once

#include <imgui.h> 
#include <imgui_internal.h> 

#include "IconsFontAwesome7.h" 

namespace Luden
{
	inline void SetupImGuiStyle()
	{
		ImGuiStyle& style = ImGui::GetStyle();
		ImVec4* colors = style.Colors;

		// Kýrmýzý/Berserk Tema Renkleri Tanýmý
		// Koyu Kýrmýzý (Vurgu)
		constexpr ImVec4 RED_ACCENT = ImVec4(0.65f, 0.05f, 0.05f, 1.00f);
		// Canlý Kýrmýzý (Aktif Vurgu)
		constexpr ImVec4 RED_ACTIVE = ImVec4(0.85f, 0.10f, 0.10f, 1.00f);
		// Hafif Kýrmýzý (Hover/Baþlýk)
		constexpr ImVec4 RED_LIGHT = ImVec4(0.40f, 0.05f, 0.05f, 0.80f);


		colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
		colors[ImGuiCol_TextDisabled] = ImVec4(0.33f, 0.33f, 0.33f, 1.00f);
		colors[ImGuiCol_WindowBg] = ImVec4(0.02f, 0.02f, 0.02f, 1.00f);
		colors[ImGuiCol_ChildBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.00f);
		colors[ImGuiCol_PopupBg] = ImVec4(0.05f, 0.05f, 0.05f, 0.94f);
		colors[ImGuiCol_Border] = ImVec4(0.04f, 0.04f, 0.04f, 0.99f);
		colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_FrameBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.54f);

		// Vurgu Kýrmýzý
		colors[ImGuiCol_FrameBgHovered] = RED_LIGHT;
		colors[ImGuiCol_FrameBgActive] = ImVec4(0.03f, 0.03f, 0.04f, 0.67f);

		colors[ImGuiCol_TitleBg] = ImVec4(0.01f, 0.01f, 0.01f, 1.00f);

		// Baþlýk Kýrmýzý
		colors[ImGuiCol_TitleBgActive] = RED_LIGHT;

		colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
		colors[ImGuiCol_MenuBarBg] = ImVec4(0.02f, 0.02f, 0.02f, 1.00f);
		colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
		colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.07f, 0.07f, 0.07f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.18f, 0.17f, 0.17f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);

		// Vurgu Kýrmýzý
		colors[ImGuiCol_CheckMark] = RED_ACCENT;

		// Vurgu Kýrmýzý
		colors[ImGuiCol_SliderGrab] = RED_ACCENT;

		// Aktif Vurgu Kýrmýzý
		colors[ImGuiCol_SliderGrabActive] = RED_ACTIVE;

		colors[ImGuiCol_Button] = ImVec4(0.21f, 0.22f, 0.23f, 0.40f);

		// Vurgu Kýrmýzý
		colors[ImGuiCol_ButtonHovered] = RED_LIGHT;

		// Aktif Kýrmýzý
		colors[ImGuiCol_ButtonActive] = RED_ACCENT;

		colors[ImGuiCol_Header] = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);

		// Vurgu Kýrmýzý
		colors[ImGuiCol_HeaderHovered] = RED_LIGHT;

		colors[ImGuiCol_HeaderActive] = ImVec4(0.03f, 0.03f, 0.03f, 1.00f);
		colors[ImGuiCol_Separator] = ImVec4(0.16f, 0.16f, 0.16f, 0.50f);

		// Vurgu Kýrmýzý
		colors[ImGuiCol_SeparatorHovered] = RED_ACCENT;
		colors[ImGuiCol_SeparatorActive] = RED_ACTIVE;

		colors[ImGuiCol_ResizeGrip] = ImVec4(0.26f, 0.59f, 0.98f, 0.20f);
		colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
		colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);

		// Vurgu Kýrmýzý
		colors[ImGuiCol_TabHovered] = RED_LIGHT;

		colors[ImGuiCol_Tab] = ImVec4(0.02f, 0.02f, 0.02f, 1.00f);
		colors[ImGuiCol_TabSelected] = ImVec4(0.02f, 0.02f, 0.02f, 1.00f);

		// Aktif Kýrmýzý Çizgi
		colors[ImGuiCol_TabSelectedOverline] = RED_ACTIVE;

		colors[ImGuiCol_TabDimmed] = ImVec4(0.02f, 0.02f, 0.02f, 1.00f);
		colors[ImGuiCol_TabDimmedSelected] = ImVec4(0.02f, 0.02f, 0.02f, 1.00f);

		// Sönük Kýrmýzý Çizgi
		colors[ImGuiCol_TabDimmedSelectedOverline] = RED_ACCENT;

		colors[ImGuiCol_DockingPreview] = ImVec4(0.26f, 0.59f, 0.98f, 0.70f);
		colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
		colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);

		// Vurgu Kýrmýzý
		colors[ImGuiCol_PlotLinesHovered] = RED_ACTIVE;

		// Vurgu Kýrmýzý
		colors[ImGuiCol_PlotHistogram] = RED_ACCENT;
		colors[ImGuiCol_PlotHistogramHovered] = RED_ACTIVE;

		colors[ImGuiCol_TableHeaderBg] = ImVec4(0.27f, 0.27f, 0.27f, 1.00f);
		colors[ImGuiCol_TableBorderStrong] = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);
		colors[ImGuiCol_TableBorderLight] = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);
		colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_TableRowBgAlt] = ImVec4(0.46f, 0.47f, 0.46f, 0.06f);

		// Vurgu Kýrmýzý
		colors[ImGuiCol_TextLink] = RED_ACTIVE;

		// Vurgu Kýrmýzý
		colors[ImGuiCol_TextSelectedBg] = ImVec4(0.65f, 0.05f, 0.05f, 0.35f);

		colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
		colors[ImGuiCol_NavCursor] = RED_ACTIVE;
		colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
		colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.78f, 0.69f, 0.69f, 0.20f);
		colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);

		// --- Yapýsal Stil Korundu ---

		style.WindowRounding = 4.0f;
		style.FrameRounding = 4.0f;
		style.GrabRounding = 3.0f;
		style.PopupRounding = 4.0f;
		style.TabRounding = 4.0f;
		style.WindowMenuButtonPosition = ImGuiDir_Right;
		style.ScrollbarSize = 10.0f;
		style.GrabMinSize = 10.0f;
		style.DockingSeparatorSize = 1.0f;
		style.SeparatorTextBorderSize = 2.0f;
	}
}
