#include "Tabs/SceneEditorTab.h"

#include <imgui.h>

namespace Luden
{
	SceneEditorTab::SceneEditorTab(const std::filesystem::path& name)
		: EditorTab(name), m_ViewportPanelName("Viewport##" + std::to_string(m_TabID))
	{

	}
}