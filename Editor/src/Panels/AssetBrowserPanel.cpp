#include "Panels/AssetBrowserPanel.h"
#include "Core/GameEngine.h"
#include <imgui_internal.h>
#include <iostream>

namespace Luden::Editor {

	AssetBrowserPanel::AssetBrowserPanel()
		: Panel("Assets"), m_SelectedFolderPath("Assets")
	{
		m_RootFolder = BuildFolderTree("Assets");

		m_DefaultFontIcon = 0;
		m_DefaultSoundIcon = 0;
	}

	std::shared_ptr<FolderNode> AssetBrowserPanel::BuildFolderTree(const std::filesystem::path& rootPath)
	{
		auto node = std::make_shared<FolderNode>();
		node->name = rootPath.filename().string();
		for (const auto& entry : std::filesystem::directory_iterator(rootPath)) {
			if (entry.is_directory())
				node->children.push_back(BuildFolderTree(entry.path()));
		}
		return node;
	}

	bool AssetBrowserPanel::IsAssetInSelectedFolder(const std::string& assetPath)
	{
		std::filesystem::path path(assetPath);
		std::string parent = path.parent_path().string();
		return parent == m_SelectedFolderPath || (m_SelectedFolderPath == "Assets" && parent.empty());
	}

	void AssetBrowserPanel::DrawFolderTree(const std::shared_ptr<FolderNode>& node, const std::string& pathPrefix)
	{
		std::string fullPath = pathPrefix.empty() ? node->name : pathPrefix + "/" + node->name;
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
		if (m_SelectedFolderPath == fullPath)
			flags |= ImGuiTreeNodeFlags_Selected;

		bool open = ImGui::TreeNodeEx(node->name.c_str(), flags);
		if (ImGui::IsItemClicked())
			m_SelectedFolderPath = fullPath;

		if (open) {
			for (const auto& child : node->children)
				DrawFolderTree(child, fullPath);
			ImGui::TreePop();
		}
	}

	void AssetBrowserPanel::DrawThumbnailCard(const std::string& key, ImTextureID texID, const char* typeStr)
	{
		ImGui::PushID(key.c_str());
		ImGui::BeginGroup();

		ImVec2 cellSize(96, 96);
		ImVec2 thumbSize(64, 64);
		ImGui::InvisibleButton("##thumb_bg", cellSize);
		bool hovered = ImGui::IsItemHovered();

		ImDrawList* draw = ImGui::GetWindowDrawList();
		ImVec2 min = ImGui::GetItemRectMin();
		ImVec2 max = ImGui::GetItemRectMax();
		ImU32 borderColor = hovered ? IM_COL32(255, 200, 0, 255) : IM_COL32(80, 80, 80, 255);
		draw->AddRect(min, max, borderColor, 4.0f);

		ImVec2 center((min.x + max.x - thumbSize.x) * 0.5f, min.y + 4);
		draw->AddImage(texID, center, ImVec2(center.x + thumbSize.x, center.y + thumbSize.y));

		ImGui::SetCursorScreenPos(ImVec2(min.x + 4, max.y - 28));
		ImGui::TextWrapped("%s", key.c_str());
		ImGui::SetCursorScreenPos(ImVec2(min.x + 4, max.y - 14));
		ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "%s", typeStr);

		if (ImGui::IsItemClicked())
			m_SelectedAssetKey = key;

		ImGui::EndGroup();
		ImGui::PopID();
	}

	void AssetBrowserPanel::RenderInspector()
	{
		if (!m_SelectedAssetKey.empty()) {
			ImGui::Begin("Asset Inspector");
			ImGui::Text("Selected: %s", m_SelectedAssetKey.c_str());
			ImGui::End();
		}
	}

	void AssetBrowserPanel::Render()
	{
		if (!m_Visible) return;
		if (!ImGui::Begin("Assets", &m_Visible)) {
			ImGui::End();
			return;
		}

		auto& assets = GameEngine::Get().GetAssets();
		ImGui::Columns(2, nullptr, true);

		if (ImGui::BeginChild("Folders", ImVec2(0, 0), true)) {
			if (m_RootFolder)
				DrawFolderTree(m_RootFolder, "");
		}
		ImGui::EndChild();

		ImGui::NextColumn();

		if (ImGui::BeginChild("Grid", ImVec2(0, 0), false)) 
		{
			const int columns = 4;
			if (ImGui::BeginTable("AssetGrid", columns)) 
			{
				/*
				for (auto& [key, tex] : assets.GetTextures()) 
				{
					if (!IsAssetInSelectedFolder(key)) continue;
					ImGui::TableNextColumn();
					DrawThumbnailCard(key, tex.getNativeHandle(), "Texture");
				}
				for (auto& [key, _] : assets.GetFonts()) 
				{
					if (!IsAssetInSelectedFolder(key)) continue;
					ImGui::TableNextColumn();
					DrawThumbnailCard(key, m_DefaultFontIcon, "Font");
				}
				for (auto& [key, _] : assets.GetSounds()) 
				{
					if (!IsAssetInSelectedFolder(key)) continue;
					ImGui::TableNextColumn();
					DrawThumbnailCard(key, m_DefaultSoundIcon, "Sound");
				}
				*/
				ImGui::EndTable();
			}
		}
		ImGui::EndChild();

		ImGui::Columns(1);
		RenderInspector();
		ImGui::End();
	}
}