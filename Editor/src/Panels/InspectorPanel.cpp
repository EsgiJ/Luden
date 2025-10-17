#include "Panels/InspectorPanel.h"

#include "ImGui/ImGuiUtils.h"
#include "Project/Project.h"
#include "Graphics/Animation.h"

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
			strcpy_s(buffer, text.c_str());

			if (ImGui::InputText("##Tag", buffer, sizeof(buffer))) 
			{
				text = std::string(buffer);
				entity.SetTag(text);
			}

			ImGui::SetTooltip("%llu", (uint64_t)entity.UUID());
		}

		//Add Component Button
		{
			ImGui::SameLine(ImGui::GetContentRegionAvail().x - ImGui::GetFontSize());
			float lineWidth = ImGui::GetFontSize() + GImGui->Style.FramePadding.y * 2.0f;

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

			bool childOpen = ImGui::BeginChild("inspectorChild");
			bool tableOpen = false;
			if (childOpen)
			{
				tableOpen = ImGui::BeginTable("InspectorTable", 1);
			}

			if (tableOpen)
			{
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
						auto animation = std::static_pointer_cast<Graphics::Animation>(Project::GetEditorResourceManager()->GetResource(animationComponent.animationHandle));
						ImGuiUtils::PrefixLabel("Current");
						if (ImGuiUtils::ResourceButton(animationComponent.animationHandle, ResourceType::Animation))
						{
							//TODO: Animation Editor Panel
						}

						ImGuiUtils::PrefixLabel("Repeat");
						ImGui::Checkbox("##Paused", &animationComponent.repeat);

						ImGuiUtils::PrefixLabel("Speed");
						int speedTmp = static_cast<int>(animationComponent.speed);
						if (ImGui::DragInt("##Speed", &speedTmp, 1, 0, 1000)) {
							animationComponent.speed = static_cast<size_t>(std::max(0, speedTmp));
						}

						ImGuiUtils::PrefixLabel("CurrentFrame");
						ImGui::BeginDisabled();
						int currentFrameTmp = static_cast<int>(animationComponent.currentFrame);
						if (ImGui::DragInt("##CurrentFrame", &currentFrameTmp, 1, 0, 1000)) {
							animationComponent.currentFrame = static_cast<size_t>(std::max(0, currentFrameTmp));
						}
						ImGui::EndDisabled();
					});

				DisplayComponentInInspector<CFont>("Font Component", entity, true, [&]()
					{
						auto& fontComponent = entity.Get<CFont>();
						auto font = std::static_pointer_cast<Font>(Project::GetEditorResourceManager()->GetResource(fontComponent.fontHandle));
						ImGuiUtils::PrefixLabel("Current");
						if (ImGuiUtils::ResourceButton(fontComponent.fontHandle, ResourceType::Font))
						{
							//TODO: Font Editor Panel
						}
					});

				DisplayComponentInInspector<CTexture>("Texture Component", entity, true, [&]()
					{
						auto& textureComponent = entity.Get<CTexture>();
						auto texture = std::static_pointer_cast<Texture>(Project::GetEditorResourceManager()->GetResource(textureComponent.textureHandle));
						ImGuiUtils::PrefixLabel("Current");
						if (ImGuiUtils::ResourceButton(textureComponent.textureHandle, ResourceType::Texture))
						{
							//TODO: Font Editor Panel
						}
					});

				DisplayComponentInInspector<CInvincibility>("Invincibility Component", entity, true, [&]()
					{
						auto& invincibilityComponent = entity.Get<CInvincibility>();
						ImGuiUtils::PrefixLabel("Frame");
						ImGui::DragInt("##Persistent", &invincibilityComponent.iframes, 1, 0);
					});

				DisplayComponentInInspector<CLifespan>("Lifespan Component", entity, true, [&]()
					{
						auto& lifespanComponent = entity.Get<CLifespan>();
						ImGuiUtils::PrefixLabel("Lifespan");
						ImGui::DragInt("##Lifespan", &lifespanComponent.lifespan, 1, 0);

						ImGuiUtils::PrefixLabel("FrameCreated");
						ImGui::BeginDisabled();
						ImGui::DragInt("##FrameCreated", &lifespanComponent.frameCreated, 1, 0);
						ImGui::EndDisabled();
					});

				DisplayComponentInInspector<CPatrol>("Patrol Component", entity, true, [&]()
					{
						auto& patrolComponent = entity.Get<CPatrol>();

						ImGuiUtils::PrefixLabel("Speed");
						ImGui::DragFloat("##Speed", &patrolComponent.speed, 0.1f);

						ImGuiUtils::PrefixLabel("Current Position Index");
						ImGui::DragInt("##CurrentPosition", reinterpret_cast<int*>(&patrolComponent.currentPosition), 1, 0, (int)patrolComponent.positions.size() - 1);

						ImGui::Separator();
						ImGui::Text("Positions:");
						for (size_t i = 0; i < patrolComponent.positions.size(); i++)
						{
							ImGui::PushID((int)i);
							ImGuiUtils::PrefixLabel(("Pos " + std::to_string(i)).c_str());
							ImGui::DragFloat2("##Pos", &patrolComponent.positions[i].x, 0.1f);
							ImGui::PopID();
						}

						if (ImGui::Button("Add Position"))
						{
							patrolComponent.positions.emplace_back(0.0f, 0.0f);
						}
						if (!patrolComponent.positions.empty() && ImGui::Button("Remove Last Position"))
						{
							patrolComponent.positions.pop_back();
						}
					});

				DisplayComponentInInspector<CState>("State Component", entity, true, [&]()
					{
						auto& stateComponent = entity.Get<CState>();

						ImGuiUtils::PrefixLabel("State");
						ImGui::InputText("##State", stateComponent.state.data(), stateComponent.state.capacity() + 1);

						ImGuiUtils::PrefixLabel("Previous State");
						ImGui::BeginDisabled();
						ImGui::InputText("##PrevState", stateComponent.previousState.data(), stateComponent.previousState.capacity() + 1);
						ImGui::EndDisabled();

						ImGuiUtils::PrefixLabel("Change Animation");
						ImGui::Checkbox("##ChangeAnim", &stateComponent.changeAnimation);
					});

				DisplayComponentInInspector<CTransform>("Transform Component", entity, true, [&]()
					{
						auto& transformComponent = entity.Get<CTransform>();

						ImGuiUtils::PrefixLabel("Position");
						ImGui::DragFloat2("##Pos", &transformComponent.pos.x, 0.1f);

						ImGuiUtils::PrefixLabel("PrevPos");
						ImGui::BeginDisabled();
						ImGui::DragFloat2("##PrevPos", &transformComponent.prevPos.x);
						ImGui::EndDisabled();

						ImGuiUtils::PrefixLabel("Velocity");
						ImGui::DragFloat2("##Velocity", &transformComponent.velocity.x, 0.1f);

						ImGuiUtils::PrefixLabel("Scale");
						ImGui::DragFloat2("##Scale", &transformComponent.scale.x, 0.1f);

						ImGuiUtils::PrefixLabel("Facing");
						ImGui::DragFloat2("##Facing", &transformComponent.facing.x, 0.1f);

						ImGuiUtils::PrefixLabel("Angle");
						ImGui::DragFloat("##Angle", &transformComponent.angle, 0.1f);
					});

				ImGui::EndTable();
			}


			if (childOpen)
			{
				ImGui::EndChild();
				ImGui::PopID();
			}
		}
	}

	template <typename T>
	void InspectorPanel::DisplayComponentInInspector(const std::string& name, Entity& entity, const bool canDelete, const std::function<void()>& lambda) 
	{
		if (not entity.Has<T>()) 
		{
			return;
		}

		constexpr ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_OpenOnDoubleClick |
			ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_DefaultOpen |
			ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_AllowItemOverlap;

		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		ImGui::PushID(((void*)typeid(T).hash_code()));

		bool removeComponent = false;

		bool closeButtonValue = true;
		bool* closeButton = canDelete ? &closeButtonValue : NULL;
		bool open = ImGui::CollapsingHeader(name.c_str(), closeButton, treeNodeFlags);

		if (canDelete) 
		{
			if (!closeButtonValue) 
			{
				ImGui::OpenPopup("ComponentSettings");
			}
			if (ImGui::IsMouseClicked(1) and ImGui::IsItemHovered()) 
			{
				ImGui::OpenPopup("ComponentSettings");
			}
			if (ImGui::BeginPopup("ComponentSettings")) 
			{
				if (ImGui::MenuItem("Delete Component")) 
				{
					removeComponent = true;
				}
				ImGui::EndPopup();
			}
		}

		if (open) 
		{
			lambda();
		}
		ImGui::PopID();

		if (removeComponent) 
		{
			entity.Remove<T>();
		}
	}

	template <typename T>
	void InspectorPanel::DisplayComponentInPopup(const std::string& name)
	{
		Entity selected = m_SceneHierarchyPanel->GetSelectedEntity();
		if (!selected.IsValid() || selected.Has<T>())
			return;

		if (ImGui::MenuItem(name.c_str())) 
		{
			selected.Add<T>();
			ImGui::CloseCurrentPopup();
		}
	}

	bool InspectorPanel::EntityButton(UUID& id) {
		bool pressed = false;
		std::string entity_name;

		if (Entity entity = m_Context->GetEntityWithUUID(id)) {
			entity_name = entity.Tag();
		}
		else {
			entity_name = "[Entity]";
		}

		if (ImGui::Button(entity_name.c_str())) {
			pressed = true;
		}
		if (ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_ENTITY")) {
				auto payload_id = static_cast<const UUID*>(payload->Data);
				if (payload_id != nullptr) {
					id = *payload_id;
				}
			}
			ImGui::EndDragDropTarget();
		}
		if (ImGui::IsItemHovered()) {
			ImGui::SetTooltip("%s", std::to_string(id).c_str());
		}

		return pressed;
	}
}