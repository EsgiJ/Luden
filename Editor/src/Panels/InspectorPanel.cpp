#include "Panels/InspectorPanel.h"

#include "ImGui/ImGuiUtils.h"
#include "Project/Project.h"

#include <imgui.h>
#include <imgui_internal.h>

namespace Luden
{
	constexpr ImGuiTreeNodeFlags innerTreeNodeFlags = ImGuiTreeNodeFlags_OpenOnDoubleClick |
		ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding;

	void InspectorPanel::SetContext(const std::shared_ptr<Scene>& context, SceneHierarchyPanel* scene_tree_panel)
	{
		m_Context = context;
		m_SceneHierarchyPanel = scene_tree_panel;
	}

	void InspectorPanel::RenderContent() 
	{
		Entity selectedEntity = m_SceneHierarchyPanel->GetSelectedEntity();
		if (selectedEntity) {
			DrawEntityInInspector(selectedEntity);
		}
	}

	void InspectorPanel::DrawEntityInInspector(Entity& entity)
	{
		if (!entity.IsValid())
			return;

		ImGui::PushID(entity.UUID());

		//Tag
		{
			std::string text = entity.Tag();

			char buffer[256];
			memset(buffer, 0, sizeof(buffer));
			strcpy(buffer, text.c_str());

			if (ImGui::InputText("##Tag", buffer, sizeof(buffer))) 
			{
				text = std::string(buffer);
			}

			ImGui::SetTooltip("%llu", (uint64_t)entity.UUID());
		}

		//Add Component Button
		{
			ImGui::SameLine(ImGui::GetContentRegionAvail().x - GImGui->Font->FontSize);
			float lineWidth = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;

			if (ImGui::Button("+", ImVec2(lineWidth, 0)))
			{
				ImGui::OpenPopup("AddComponent");
			}
			if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
			{
				ImGui::SetTooltip("Add Component");
			}
			if (ImGui::BeginPopup("AddComponent"))
			{
				DisplayComponentInPopup<CDamage>("Damage Component");
				DisplayComponentInPopup<CDraggable>("Draggable Component");
				DisplayComponentInPopup<CFollowPlayer>("Follow Player Component");
				DisplayComponentInPopup<CGravity>("Gravity Component");
				DisplayComponentInPopup<CHealth>("Health Component");
				DisplayComponentInPopup<CInput>("Input Component");
				DisplayComponentInPopup<CBoundingBox>("Bounding Box Component");
				DisplayComponentInPopup<CAnimation>("Animation Component");
				DisplayComponentInPopup<CFont>("Font Component");
				DisplayComponentInPopup<CTexture>("Texture Component");
				DisplayComponentInPopup<CInvincibility>("Invincibility Component");
				DisplayComponentInPopup<CLifespan>("Lifespan Component");
				DisplayComponentInPopup<CPatrol>("Patrol Component");
				DisplayComponentInPopup<CState>("State Component");
				DisplayComponentInPopup<CTransform>("Transform Component");
				ImGui::EndPopup();
			}

			ImGui::Spacing();

			bool isOpen = ImGui::BeginChild("inspectorChild") && ImGui::BeginTable("InspectorTable", 1);
			if (!isOpen)
			{
				ImGui::EndChild();
				ImGui::PopID();
				return;
			}

			DisplayComponentInInspector<CDamage>("Damage Component", entity, true, [&]() 
			{
				auto& damageComponent = entity.Get<CDamage>();
				ImGuiUtils::PrefixLabel("Damage");
				ImGui::DragInt("##Persistent", &damageComponent.damage, 1, 0);
			});

			DisplayComponentInInspector<CDraggable>("Draggable Component", entity, true, [&]() 
			{
				auto& draggableComponent = entity.Get<CDraggable>();
				ImGuiUtils::PrefixLabel("Dragging");
				ImGui::Checkbox("##Dragging", &draggableComponent.dragging);
			});

			DisplayComponentInInspector<CFollowPlayer>("Follow Player Component", entity, true, [&]() 
			{
				auto& followPlayerComponent = entity.Get<CFollowPlayer>();
				ImGuiUtils::PrefixLabel("Home");
				ImGui::DragFloat2("##Home", &followPlayerComponent.home.x, 0.1f);

				ImGuiUtils::PrefixLabel("Speed");
				ImGui::DragFloat2("##Speed", &followPlayerComponent.home.x, 0.1f);
			});

			DisplayComponentInInspector<CGravity>("Gravity Component", entity, true, [&]() 
			{
				auto& gravityComponent = entity.Get<CGravity>();
				ImGuiUtils::PrefixLabel("Gravity");
				ImGui::DragFloat("##Gravity", &gravityComponent.gravity, 0.1f);
			});

			DisplayComponentInInspector<CHealth>("Health Component", entity, true, [&]() 
			{
				auto& healthComponent = entity.Get<CHealth>();
				ImGuiUtils::PrefixLabel("Current");
				ImGui::DragInt("##Current", &healthComponent.current, 1, 0);

				ImGuiUtils::PrefixLabel("Max");
				ImGui::DragInt("##Max", &healthComponent.max, 1, 0);
			});

			DisplayComponentInInspector<CInput>("Input Component", entity, true, [&]() 
			{
				auto& inputComponent = entity.Get<CInput>();
				ImGuiUtils::PrefixLabel("Up");
				ImGui::Checkbox("##Up", &inputComponent.up);

				ImGuiUtils::PrefixLabel("Down");
				ImGui::Checkbox("##Down", &inputComponent.down);

				ImGuiUtils::PrefixLabel("Left");
				ImGui::Checkbox("##Left", &inputComponent.left);

				ImGuiUtils::PrefixLabel("Right");
				ImGui::Checkbox("##Right", &inputComponent.right);

				ImGuiUtils::PrefixLabel("Attack");
				ImGui::Checkbox("##Attack", &inputComponent.attack);

				ImGuiUtils::PrefixLabel("CanAttack");
				ImGui::Checkbox("##Up", &inputComponent.canAttack);
			});

			DisplayComponentInInspector<CBoundingBox>("Bounding Box Component", entity, true, [&]() 
			{
				auto& boundingBoxComponent = entity.Get<CBoundingBox>();
				ImGuiUtils::PrefixLabel("Size");
				ImGui::DragFloat2("##Size", &boundingBoxComponent.size.x, 0.1f);

				ImGuiUtils::PrefixLabel("HalfSize");
				ImGui::BeginDisabled();
				ImGui::DragFloat2("##HalfSize", &boundingBoxComponent.halfSize.x);
				ImGui::EndDisabled();

				ImGuiUtils::PrefixLabel("Center");
				ImGui::DragFloat2("##Center", &boundingBoxComponent.center.x, 0.1f);

				ImGuiUtils::PrefixLabel("PrevCenter");
				ImGui::BeginDisabled();
				ImGui::DragFloat2("##PrevCenter", &boundingBoxComponent.prevCenter.x);
				ImGui::EndDisabled();

				ImGuiUtils::PrefixLabel("BlockMove");
				ImGui::Checkbox("##BlockMove", &boundingBoxComponent.blockMove);

				ImGuiUtils::PrefixLabel("BlockVision");
				ImGui::Checkbox("##BlockVision", &boundingBoxComponent.blockVision);
			});

			DisplayComponentInInspector<CAnimation>("Animation Component", entity, true, [&]() 
			{
				auto& animationComponent = entity.Get<CAnimation>();
				auto& animation = std::static_pointer_cast<Animation>(Project::GetResourceManager()->GetResource(animationComponent.animationHandle));
				ImGuiUtils::PrefixLabel("Current");
				if (ImGuiUtils::ResourceButton<CAnimation>(animationComponent.animationHandle))
				{
					//TODO: Animation Editor Panel
				}

				ImGuiUtils::PrefixLabel("Repeat");
				ImGui::Checkbox("##Paused", &animationComponent.repeat);

				ImGuiUtils::PrefixLabel("Speed");
				ImGui::DragInt("##Speed", &animationComponent.speed);

			});
		}
	}
}