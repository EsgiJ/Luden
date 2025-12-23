#pragma once
#include "Tabs/EditorTab.h"
#include "Graphics/Animation.h"

#include <SFML/Graphics/RenderTexture.hpp>
#include "SFML/Graphics/Rect.hpp"

namespace Luden
{
	class AnimationEditorTab : public EditorTab
	{
	public:
		AnimationEditorTab(const std::filesystem::path& filepath);
		virtual ~AnimationEditorTab();

	private:
		virtual void RenderContent() override;
		virtual void InitializeDockspace() override;
		virtual void OnEvent(const std::optional<sf::Event>& evt) override;
		virtual void OnUpdate(TimeStep timestep) override;

		void SaveAnimation();
		void LoadAnimation(const std::filesystem::path& filepath);

		void RenderTimeline();
		void RenderToolbar();
		void RenderFrameList();
		void RenderPreview();
		void RenderProperties();

		void PlayPreview();
		void StopPreview();
		void PausePreview();

	private:
		std::shared_ptr<Animation> m_Animation;
		std::filesystem::path m_FilePath;
		const std::string m_ViewportPanelName;

		std::shared_ptr<sf::RenderTexture> m_PreviewTexture;
		bool m_IsPlaying = false;
		bool m_IsPaused = false;
		float m_CurrentTime = 0.0f;
		size_t m_CurrentFrameIndex = 0;
		float m_PlaybackSpeed = 1.0f;

		int m_SelectedFrameIndex = -1;

		float m_TimelineZoom = 1.0f;
		float m_PreviewZoom = 1.0f;
		bool m_ShowGrid = true;

		bool m_IsDraggingFrame = false;
		int m_DragSourceIndex = -1;

		std::string m_FrameListPanelName;
		std::string m_PreviewPanelName;
		std::string m_PropertiesPanelName;
		std::string m_TimelinePanelName;
		std::string m_ToolbarPanelName;

		bool m_ShowRectEditor = false;
		sf::IntRect m_EditingRect = sf::IntRect({ 0, 0 }, { 32, 32 });
	};
}