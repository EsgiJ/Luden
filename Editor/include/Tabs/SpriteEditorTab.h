#pragma once
#include "EditorAPI.h"
#include "Tabs/EditorTab.h"
#include "Graphics/Sprite.h"
#include "Graphics/Texture.h"
#include <SFML/Graphics/RenderTexture.hpp>

namespace Luden
{
	class EDITOR_API SpriteEditorTab : public EditorTab
	{
	public:
		SpriteEditorTab(const std::filesystem::path& filepath);
		virtual ~SpriteEditorTab() = default;

	private:
		virtual void RenderContent() override;
		virtual void InitializeDockspace() override;
		virtual void OnEvent(const std::optional<sf::Event>& evt) override;
		virtual void OnUpdate(TimeStep timestep) override;

		void SaveSprite();
		void LoadSprite(const std::filesystem::path& filepath);

		void RenderToolbar();
		void RenderTextureSelector();
		void RenderRectEditor();
		void RenderPivotEditor();
		void RenderPreview();

	private:
		std::shared_ptr<Sprite> m_Sprite;
		std::shared_ptr<Texture> m_CurrentTexture;
		std::shared_ptr<sf::RenderTexture> m_PreviewTexture;
		std::filesystem::path m_FilePath;

		// Panels
		std::string m_ToolbarPanelName;
		std::string m_TexturePanelName;
		std::string m_RectEditorPanelName;
		std::string m_PivotEditorPanelName;
		std::string m_PreviewPanelName;

		int m_GridColumns = 4;
		int m_GridRows = 4;
		int m_SelectedGridX = 0;
		int m_SelectedGridY = 0;
		bool m_ShowGrid = true;

		// Preview
		float m_PreviewZoom = 2.0f;
		bool m_ShowPivotGizmo = true;
	};
}