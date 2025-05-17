#pragma once

#include "EditorAPI.h"
#include "Panels/Panel.h"

namespace Luden::Editor
{
	class EDITOR_API SceneHierarchyPanel : public Panel
	{
	public:
		SceneHierarchyPanel() : Panel("Scene Hierarchy") {}
		void Render() override;
	};
}