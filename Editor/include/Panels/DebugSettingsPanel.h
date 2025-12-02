#pragma once

#include "Panels/EditorPanel.h"

namespace Luden
{
	class Scene;

	class DebugSettingsPanel : public EditorPanel
	{
	public:
		DebugSettingsPanel() : EditorPanel("DebugSettings") {}
		~DebugSettingsPanel() = default;

	private:
		virtual void RenderContent() override final;
	};
}