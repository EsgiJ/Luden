#pragma once

#include "Panels/EditorPanel.h"
#include "Resource/ResourceManagerBase.h"
#include "EditorAPI.h"
#include "Graphics/Texture.h"
#include <unordered_set>
#include <memory>

class ResourceHandle;
class Resource; 
namespace Luden {

	struct BrowserEntry
	{
		std::filesystem::path Path;
		std::string Filename;
		bool IsDirectory;
		ResourceHandle Handle;
		ResourceType Type = ResourceType::None;
	};

	class ResourceBrowserPanel : public EditorPanel
	{
		using ResourceItem = std::pair<ResourceHandle, ResourceMetadata>;
	public:
		ResourceBrowserPanel() : EditorPanel("Resource Browser"){}
		~ResourceBrowserPanel() = default;

	protected:
		void RenderContent() override;

	private:
		void UpdateLayoutSizes(float availWidth);
		void CollectAndSortEntries(std::vector<BrowserEntry>& entries);

		void RenderFilterAndReloadToolbar();

		void ReloadResources();
		void ReloadResource();

		void HelpMarker(const char* desc);
	private:
		// Options
		std::string m_SearchFilter;
		ResourceType m_SelectedFilter = ResourceType::None;
		bool m_ShowTypeOverlay = true;
		bool m_AllowSorting = true;
		bool m_AllowDragUnselected = true;

		float m_IconSize = 100.0f;
		int m_IconSpacing = 10;
		int m_IconHitSpacing = 4;
		bool m_StretchSpacing = true;

		// Calculated sizes for layout, output of UpdateLayoutSizes()
		ImVec2 m_LayoutItemSize;
		ImVec2 m_LayoutItemStep;            // == m_LayoutItemSize + m_LayoutItemSpacing
		float m_LayoutItemSpacing = 0.0f;
		float m_LayoutSelectableSpacing = 0.0f;
		float m_LayoutOuterPadding = 0.0f;
		int m_LayoutColumnCount = 0;
		int m_LayoutLineCount = 0;

		// State
		std::vector<BrowserEntry> m_Entries;
		std::unordered_set<ResourceHandle> m_Selection;

		ImGuiTableSortSpecs* m_SortSpecs = nullptr;
		float m_ZoomWheelAccum = 0.0f;
		bool m_RequestSort = false;
		std::filesystem::path m_CurrentDirectory;
	};
}