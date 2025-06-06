#pragma once

#include "EditorAPI.h"
#include "Panels/Panel.h"

namespace Luden::Editor
{
	class EDITOR_API AssetBrowserPanel : public Panel
	{
	public:
		AssetBrowserPanel() : Panel("Assets") {}
		void Render() override;
	};
}