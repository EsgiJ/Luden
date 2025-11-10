#include "Panels/InspectorPanel.h"

#include "ImGui/ImGuiUtils.h"
#include "Project/Project.h"
#include "Graphics/Animation.h"
#include "ECS/Components/Components.h"
#include "NativeScript/NativeScriptGenerator.h"
#include "NativeScript/NativeScript.h"

#include <IconsFontAwesome7.h>
#include <imgui.h>
#include <imgui_internal.h>

#include <memory>
#include <iostream>

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
			ImGui::SameLine();

			if (ImGui::Button(ICON_FA_PLUS " Add Component"))
			{
				ImGui::OpenPopup("AddComponent");
			}
			if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
			{
				ImGui::SetTooltip("Add Component");
			}

			if (ImGui::BeginPopup("AddComponent"))
			{
				DisplayComponentInPopup<DamageComponent>(ICON_FA_GUN " Damage Component");
				DisplayComponentInPopup<DraggableComponent>(ICON_FA_HAND " Draggable Component");
				DisplayComponentInPopup<FollowPLayerComponent>(ICON_FA_PERSON_RUNNING " Follow Player Component");
				DisplayComponentInPopup<GravityComponent>(ICON_FA_ARROW_DOWN " Gravity Component");
				DisplayComponentInPopup<HealthComponent>(ICON_FA_HEART " Health Component");
				DisplayComponentInPopup<InputComponent>(ICON_FA_GAMEPAD " Input Component");
				DisplayComponentInPopup<BoxCollider2DComponent>(ICON_FA_SQUARE " Box Collider 2D Component");
				DisplayComponentInPopup<CircleCollider2DComponent>(ICON_FA_CIRCLE " Circle Collider 2D Component");
				DisplayComponentInPopup<RigidBody2DComponent>(ICON_FA_CUBES " RigidBody 2D Component");
				DisplayComponentInPopup<NativeScriptComponent>(ICON_FA_CODE " Native Script Component");
				DisplayComponentInPopup<Animation2DComponent>(ICON_FA_PLAY " Animation Component");
				DisplayComponentInPopup<TextComponent>(ICON_FA_FONT " Font Component");
				DisplayComponentInPopup<TextureComponent>(ICON_FA_IMAGE " Texture Component");
				DisplayComponentInPopup<InvincibilityComponent>(ICON_FA_SHIELD_HALVED " Invincibility Component");
				DisplayComponentInPopup<LifespanComponent>(ICON_FA_CLOCK " Lifespan Component");
				DisplayComponentInPopup<PatrolComponent>(ICON_FA_ROAD " Patrol Component");
				DisplayComponentInPopup<StateComponent>(ICON_FA_SCROLL " State Component");
				DisplayComponentInPopup<TransformComponent>(ICON_FA_ARROWS_UP_DOWN_LEFT_RIGHT " Transform Component");
				ImGui::EndPopup();
			}

			ImGui::Spacing();

			bool childOpen = ImGui::BeginChild("inspectorChild");
			bool tableOpen = false;
			if (childOpen)
			{
				tableOpen = ImGui::BeginTable("InspectorTable", 1);
			}

			ImGui::Separator();

			if (tableOpen)
			{
				DisplayComponentInInspector<DamageComponent>(ICON_FA_GUN " Damage Component", entity, true, [&]()
					{
						auto& damageComponent = entity.Get<DamageComponent>();
						ImGuiUtils::PrefixLabel("Damage");
						ImGui::DragInt("##Persistent", &damageComponent.damage, 1, 0);
					});

				DisplayComponentInInspector<DraggableComponent>(ICON_FA_HAND " Draggable Component", entity, true, [&]()
					{
						auto& draggableComponent = entity.Get<DraggableComponent>();
						ImGuiUtils::PrefixLabel("Dragging");
						ImGui::Checkbox("##Dragging", &draggableComponent.dragging);
					});

				DisplayComponentInInspector<FollowPLayerComponent>(ICON_FA_PERSON_RUNNING " Follow Player Component", entity, true, [&]()
					{
						auto& followPlayerComponent = entity.Get<FollowPLayerComponent>();
						ImGuiUtils::PrefixLabel("Home");
						ImGuiUtils::DragFloat2Colored("##Home", &followPlayerComponent.home.x, 0.1f);

						ImGuiUtils::PrefixLabel("Speed");
						ImGui::DragFloat("##Speed", &followPlayerComponent.speed, 0.1f);

					});

				DisplayComponentInInspector<GravityComponent>(ICON_FA_ARROW_DOWN " Gravity Component", entity, true, [&]()
					{
						auto& gravityComponent = entity.Get<GravityComponent>();
						ImGuiUtils::PrefixLabel("Gravity");
						ImGui::DragFloat("##Gravity", &gravityComponent.gravity, 0.1f);
					});

				DisplayComponentInInspector<HealthComponent>(ICON_FA_HEART " Health Component", entity, true, [&]()
					{
						auto& healthComponent = entity.Get<HealthComponent>();
						ImGuiUtils::PrefixLabel("Current");
						ImGui::DragInt("##Current", &healthComponent.current, 1, 0);

						ImGuiUtils::PrefixLabel("Max");
						ImGui::DragInt("##Max", &healthComponent.max, 1, 0);
					});

				DisplayComponentInInspector<InputComponent>(ICON_FA_GAMEPAD " Input Component", entity, true, [&]()
					{
						auto& inputComponent = entity.Get<InputComponent>();
						ImGuiUtils::PrefixLabel(ICON_FA_ARROW_UP " Up");
						ImGui::Checkbox("##Up", &inputComponent.up);

						ImGuiUtils::PrefixLabel(ICON_FA_ARROW_DOWN " Down");
						ImGui::Checkbox("##Down", &inputComponent.down);

						ImGuiUtils::PrefixLabel(ICON_FA_ARROW_LEFT " Left");
						ImGui::Checkbox("##Left", &inputComponent.left);

						ImGuiUtils::PrefixLabel(ICON_FA_ARROW_RIGHT " Right");
						ImGui::Checkbox("##Right", &inputComponent.right);

						ImGuiUtils::PrefixLabel(ICON_FA_GUN " Attack");
						ImGui::Checkbox("##Attack", &inputComponent.attack);

						ImGuiUtils::PrefixLabel(ICON_FA_CHECK " CanAttack");
						ImGui::Checkbox("##Up", &inputComponent.canAttack);
					});

				DisplayComponentInInspector<RigidBody2DComponent>(ICON_FA_CUBES " RigidBody 2D Component", entity, true, [&]()
					{
						auto& rb = entity.Get<RigidBody2DComponent>();

						ImGuiUtils::PrefixLabel("Body Type");
						const char* types[] = { "None", "Static", "Dynamic", "Kinematic" };
						int currentType = static_cast<int>(rb.BodyType) + 1; // because None = -1
						if (ImGui::Combo("##BodyType", &currentType, types, IM_ARRAYSIZE(types)))
							rb.BodyType = static_cast<RigidBody2DComponent::Type>(currentType - 1);

						ImGuiUtils::PrefixLabel("Fixed Rotation");
						ImGui::Checkbox("##FixedRotation", &rb.FixedRotation);

						ImGuiUtils::PrefixLabel("Mass");
						ImGui::DragFloat("##Mass", &rb.Mass, 0.1f, 0.0f, 100.0f);

						ImGuiUtils::PrefixLabel("Linear Drag");
						ImGui::DragFloat("##LinearDrag", &rb.LinearDrag, 0.01f, 0.0f, 10.0f);

						ImGuiUtils::PrefixLabel("Angular Drag");
						ImGui::DragFloat("##AngularDrag", &rb.AngularDrag, 0.01f, 0.0f, 10.0f);

						ImGuiUtils::PrefixLabel("Gravity Scale");
						ImGui::DragFloat("##GravityScale", &rb.GravityScale, 0.1f, 0.0f, 10.0f);
					});

				DisplayComponentInInspector<BoxCollider2DComponent>(ICON_FA_SQUARE " Box Collider 2D Component", entity, true, [&]()
					{
						auto& box = entity.Get<BoxCollider2DComponent>();

						ImGuiUtils::PrefixLabel("Offset");
						ImGuiUtils::DragFloat2Colored("##Offset", &box.Offset.x, 0.1f);

						ImGuiUtils::PrefixLabel("Size");
						ImGuiUtils::DragFloat2Colored("##Size", &box.Size.x, 0.1f);

						ImGuiUtils::PrefixLabel("Density");
						ImGui::DragFloat("##Density", &box.Density, 0.1f, 0.0f, 10.0f);

						ImGuiUtils::PrefixLabel("Friction");
						ImGui::DragFloat("##Friction", &box.Friction, 0.1f, 0.0f, 10.0f);
					});

				DisplayComponentInInspector<CircleCollider2DComponent>(ICON_FA_CIRCLE " Circle Collider 2D Component", entity, true, [&]()
					{
						auto& circle = entity.Get<CircleCollider2DComponent>();

						ImGuiUtils::PrefixLabel("Offset");
						ImGuiUtils::DragFloat2Colored("##Offset", &circle.Offset.x, 0.1f);

						ImGuiUtils::PrefixLabel("Radius");
						ImGui::DragFloat("##Radius", &circle.Radius, 0.1f, 0.0f, 100.0f);

						ImGuiUtils::PrefixLabel("Density");
						ImGui::DragFloat("##Density", &circle.Density, 0.1f, 0.0f, 10.0f);

						ImGuiUtils::PrefixLabel("Friction");
						ImGui::DragFloat("##Friction", &circle.Friction, 0.1f, 0.0f, 10.0f);
					});

				DisplayComponentInInspector<NativeScriptComponent>(
					ICON_FA_CODE " Native Script", entity, true, [&]()
					{
						auto& nsc = entity.Get<NativeScriptComponent>();
						auto resourceManager = Project::GetResourceManager();

						ImGuiUtils::PrefixLabel(ICON_FA_FILE_CODE " Script");

						// Current script name
						std::string currentScriptName = "None";
						if (nsc.ScriptHandle != 0)
						{
							auto resource = resourceManager->GetResource(nsc.ScriptHandle);
							auto script = std::static_pointer_cast<NativeScript>(resource);
							if (script)
								currentScriptName = script->GetClassName();
						}

						if (ImGui::BeginCombo("##ScriptSelector", currentScriptName.c_str()))
						{
							// None option
							if (ImGui::Selectable("None", nsc.ScriptHandle == 0))
							{
								nsc.DestroyInstance();
								nsc.ScriptHandle = 0;
								nsc.InstantiateScript = nullptr;
								nsc.DestroyScript = nullptr;
							}

							// Get all NativeScript resources
							auto scriptHandles = resourceManager->GetAllResourcesWithType(ResourceType::NativeScript);

							for (auto handle : scriptHandles)
							{
								auto script = std::static_pointer_cast<NativeScript>(resourceManager->GetResource(handle));

								if (!script)
									continue;

								bool isSelected = (nsc.ScriptHandle == handle);
								bool hasBindings = (script->GetInstantiateFunc() != nullptr);

								// Disable if not compiled
								if (!hasBindings)
									ImGui::BeginDisabled();

								std::string displayName = script->GetClassName();
								if (!hasBindings)
									displayName += " (Not Compiled)";

								if (ImGui::Selectable(displayName.c_str(), isSelected))
								{
									// Destroy old instance
									nsc.DestroyInstance();

									// Bind new script
									nsc.BindFromHandle(handle);									
								}

								if (!hasBindings)
									ImGui::EndDisabled();
							}

							ImGui::EndCombo();
						}

						ImGui::SameLine();

						// Create new script button
						if (ImGui::Button(ICON_FA_PLUS))
						{
							ImGui::OpenPopup("CreateNewScript");
						}
						if (ImGui::IsItemHovered())
						{
							ImGui::SetTooltip("Create New Script");
						}

						// Create script dialog
						if (ImGui::BeginPopupModal("CreateNewScript", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
						{
							static char scriptName[256] = "";

							ImGui::Text("Create New Native Script");
							ImGui::Separator();

							ImGui::InputText("Class Name", scriptName, sizeof(scriptName));

							if (ImGui::Button("Create", ImVec2(120, 0)))
							{
								if (strlen(scriptName) > 0)
								{
									ResourceHandle newHandle = NativeScriptGenerator::CreateNewScript(scriptName);

									if (newHandle != 0)
									{
										// Auto-select the new script
										nsc.BindFromHandle(newHandle);

										scriptName[0] = '\0'; // Clear buffer
										ImGui::CloseCurrentPopup();
									}
								}
							}

							ImGui::SameLine();

							if (ImGui::Button("Cancel", ImVec2(120, 0)))
							{
								scriptName[0] = '\0';
								ImGui::CloseCurrentPopup();
							}

							ImGui::EndPopup();
						}

						// Script status
						if (nsc.ScriptHandle != 0)
						{
							auto script = std::static_pointer_cast<NativeScript>(
								resourceManager->GetResource(nsc.ScriptHandle)
							);

							ImGuiUtils::PrefixLabel("Status");

							if (!script->GetInstantiateFunc())
							{
								ImGui::TextColored(ImVec4(1, 0, 0, 1), ICON_FA_XMARK " Not Compiled");

								if (ImGui::Button(ICON_FA_HAMMER " Rebuild GameModule"))
								{
									// TODO: Trigger recompile
									std::cout << "Rebuild triggered!" << std::endl;
								}
							}
							else if (nsc.Instance)
							{
								ImGui::TextColored(ImVec4(0, 1, 0, 1), ICON_FA_CHECK " Active");
							}
							else
							{
								ImGui::TextColored(ImVec4(1, 1, 0, 1), ICON_FA_PAUSE " Not Instantiated");
							}

							// Show file paths
							if (ImGui::TreeNode("Files"))
							{
								ImGui::Text("Header: %s", script->GetHeaderPath().string().c_str());
								ImGui::Text("Source: %s", script->GetSourcePath().string().c_str());
								ImGui::TreePop();
							}
						}
					});

				DisplayComponentInInspector<Animation2DComponent>(ICON_FA_PLAY " Animation Component", entity, true, [&]()
					{
						auto& animationComponent = entity.Get<Animation2DComponent>();
						auto animation = std::static_pointer_cast<Graphics::Animation>(Project::GetEditorResourceManager()->GetResource(animationComponent.animationHandle));
						ImGuiUtils::PrefixLabel(ICON_FA_FILE_VIDEO " Current");
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

				DisplayComponentInInspector<TextComponent>(ICON_FA_FONT " Font Component", entity, true, [&]()
					{
						auto& fontComponent = entity.Get<TextComponent>();
						auto font = std::static_pointer_cast<Font>(Project::GetEditorResourceManager()->GetResource(fontComponent.fontHandle));
						ImGuiUtils::PrefixLabel("Current");
						if (ImGuiUtils::ResourceButton(fontComponent.fontHandle, ResourceType::Font))
						{
							//TODO: Font Editor Panel
						}
					});

				DisplayComponentInInspector<TextureComponent>(ICON_FA_IMAGE " Texture Component", entity, true, [&]()
					{
						auto& textureComponent = entity.Get<TextureComponent>();
						auto texture = std::static_pointer_cast<Texture>(Project::GetEditorResourceManager()->GetResource(textureComponent.textureHandle));
						ImGuiUtils::PrefixLabel(ICON_FA_FILE_IMAGE " Current");
						if (ImGuiUtils::ResourceButton(textureComponent.textureHandle, ResourceType::Texture))
						{
							//TODO: Font Editor Panel
						}
					});

				DisplayComponentInInspector<InvincibilityComponent>(ICON_FA_SHIELD_HALVED " Invincibility Component", entity, true, [&]()
					{
						auto& invincibilityComponent = entity.Get<InvincibilityComponent>();
						ImGuiUtils::PrefixLabel("Frame");
						ImGui::DragInt("##Persistent", &invincibilityComponent.iframes, 1, 0);
					});

				DisplayComponentInInspector<LifespanComponent>(ICON_FA_SHIELD_HALVED " Lifespan Component", entity, true, [&]()
					{
						auto& lifespanComponent = entity.Get<LifespanComponent>();
						ImGuiUtils::PrefixLabel("Lifespan");
						ImGui::DragInt("##Lifespan", &lifespanComponent.lifespan, 1, 0);

						ImGuiUtils::PrefixLabel("FrameCreated");
						ImGui::BeginDisabled();
						ImGui::DragInt("##FrameCreated", &lifespanComponent.frameCreated, 1, 0);
						ImGui::EndDisabled();
					});

				DisplayComponentInInspector<PatrolComponent>(ICON_FA_ROAD " Patrol Component", entity, true, [&]()
					{
						auto& patrolComponent = entity.Get<PatrolComponent>();

						ImGuiUtils::PrefixLabel("Speed");
						ImGui::DragFloat("##Speed", &patrolComponent.speed, 0.1f);

						ImGuiUtils::PrefixLabel("Current Position Index");
						ImGui::DragInt("##CurrentPosition", reinterpret_cast<int*>(&patrolComponent.currentPosition), 1, 0, (int)patrolComponent.positions.size() - 1);

						ImGui::Separator();
						ImGui::Text(ICON_FA_LOCATION_DOT " Positions:");
						for (size_t i = 0; i < patrolComponent.positions.size(); i++)
						{
							ImGui::PushID((int)i);
							ImGuiUtils::PrefixLabel(("Pos " + std::to_string(i)).c_str());
							ImGuiUtils::DragFloat2Colored("##Pos", &patrolComponent.positions[i].x, 0.1f);
							ImGui::PopID();
						}

						if (ImGui::Button(ICON_FA_PLUS " Add Position"))
						{
							patrolComponent.positions.emplace_back(0.0f, 0.0f);
						}
						if (!patrolComponent.positions.empty() && ImGui::Button(ICON_FA_MINUS " Remove Last Position"))
						{
							patrolComponent.positions.pop_back();
						}
					});

				DisplayComponentInInspector<StateComponent>(ICON_FA_SCROLL " State Component", entity, true, [&]()
					{
						auto& stateComponent = entity.Get<StateComponent>();

						ImGuiUtils::PrefixLabel("State");
						ImGui::InputText("##State", stateComponent.state.data(), stateComponent.state.capacity() + 1);

						ImGuiUtils::PrefixLabel("Previous State");
						ImGui::BeginDisabled();
						ImGui::InputText("##PrevState", stateComponent.previousState.data(), stateComponent.previousState.capacity() + 1);
						ImGui::EndDisabled();

						ImGuiUtils::PrefixLabel(ICON_FA_ARROW_RIGHT_ARROW_LEFT " Change Animation");
						ImGui::Checkbox("##ChangeAnim", &stateComponent.changeAnimation);
					});

				DisplayComponentInInspector<TransformComponent>(ICON_FA_ARROWS_UP_DOWN_LEFT_RIGHT " Transform Component", entity, true, [&]()
					{
						auto& transformComponent = entity.Get<TransformComponent>();

						ImGuiUtils::PrefixLabel(ICON_FA_LOCATION_ARROW " Position");
						ImGuiUtils::DragFloat2Colored("##Pos", &transformComponent.Translation.x, 0.1f);

						ImGuiUtils::PrefixLabel(ICON_FA_MAGNIFYING_GLASS_PLUS " Scale");
						ImGuiUtils::DragFloat2Colored("##Scale", &transformComponent.Scale.x, 0.1f);

						ImGuiUtils::PrefixLabel(ICON_FA_ROTATE " Angle" );
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
			ImGui::Indent();
			lambda();
			ImGui::Unindent();
		}
		ImGui::PopID();

		if (removeComponent) 
		{
			entity.Remove<T>();
		}

		ImGui::Spacing();
		ImGui::Separator();
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