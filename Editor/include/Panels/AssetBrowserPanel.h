#pragma once

#include "EditorAPI.h"
#include "Panels/Panel.h"
#include <imgui.h>
#include <SFML/Graphics/Texture.hpp>
#include <filesystem>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace Luden::Editor {

	struct EDITOR_API FolderNode {
		std::string name;
		std::vector<std::shared_ptr<FolderNode>> children;
	};

	class EDITOR_API AssetBrowserPanel : public Panel {
	public:
		AssetBrowserPanel();
		void Render() override;

	private:
		void DrawFolderTree(const std::shared_ptr<FolderNode>& node, const std::string& pathPrefix);
		void DrawThumbnailCard(const std::string& key, ImTextureID texID, const char* typeStr);
		void RenderInspector();
		std::shared_ptr<FolderNode> BuildFolderTree(const std::filesystem::path& rootPath);
		bool IsAssetInSelectedFolder(const std::string& assetPath);

		std::shared_ptr<FolderNode> m_RootFolder;
		std::string m_SelectedFolderPath;
		std::string m_SelectedAssetKey;
		ImTextureID m_DefaultFontIcon = 0;
		ImTextureID m_DefaultSoundIcon = 0;
	};

}