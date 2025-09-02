#pragma once

#include "EditorAPI.h"
#include "Panels/Panel.h"

namespace Luden::Editor {

	class EDITOR_API ResourceBrowserPanel : public Panel 
	{
	public:
		ResourceBrowserPanel() : Panel("Resource Browser") {}
		void Render() override;

	private:
	};

}