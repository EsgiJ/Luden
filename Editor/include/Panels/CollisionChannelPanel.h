#pragma once
#include "Panels/EditorPanel.h"
#include "Physics2D/CollisionChannelRegistry.h"

namespace Luden
{
	class CollisionChannelPanel : public EditorPanel
	{
	public:
		CollisionChannelPanel() : EditorPanel("Collision Channels") {}

		void RenderContent() override;

	private:
		char m_NewChannelName[256] = "";
		std::string m_SelectedChannel = "";
	};
}