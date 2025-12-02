#include "Panels/DebugSettingsPanel.h"
#include "Debug/DebugManager.h"

#include <imgui.h>

namespace Luden
{
	void DebugSettingsPanel::RenderContent()
	{
		auto& config = DebugManager::Instance().GetDebugConfig();

		ImGui::Text("Box2D Debug Settings");
		ImGui::Checkbox("Enable Physics Debug", &config.enabled);
		ImGui::Separator();

		ImGui::Checkbox("Draw Shapes", &config.drawShapes);
		ImGui::Checkbox("Draw Joints", &config.drawJoints);
		ImGui::Checkbox("Draw Joint Extras", &config.drawJointExtras);
		ImGui::Checkbox("Draw Bounds", &config.drawBounds);
		ImGui::Checkbox("Draw Mass", &config.drawMass);
		ImGui::Checkbox("Draw Body Names", &config.drawBodyNames);
		ImGui::Checkbox("Draw Contact Points", &config.drawContactPoints);
		ImGui::Checkbox("Draw Graph Colors", &config.drawGraphColors);
		ImGui::Checkbox("Draw Contact Features", &config.drawContactFeatures);
		ImGui::Checkbox("Draw Contact Normals", &config.drawContactNormals);
		ImGui::Checkbox("Draw Contact Forces", &config.drawContactForces);
		ImGui::Checkbox("Draw Friction Forces", &config.drawFrictionForces);
		ImGui::Checkbox("Draw Islands", &config.drawIslands);

		ImGui::Separator();
		ImGui::DragFloat("Force Scale", &config.forceScale, 0.1f, 0.1f, 10.0f);
		ImGui::DragFloat("Joint Scale", &config.jointScale, 0.1f, 0.1f, 10.0f);
	}
}

