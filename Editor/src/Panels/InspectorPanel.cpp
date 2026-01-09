#include "Panels/InspectorPanel.h"

#include "ImGui/ImGuiUtils.h"
#include "Project/Project.h"
#include "Graphics/Animation.h"
#include "ECS/Components/Components.h"
#include "NativeScript/NativeScriptGenerator.h"
#include "NativeScript/NativeScript.h"
#include "Core/EditorApplication.h"

#include <memory>
#include <iostream>

#include <IconsFontAwesome7.h>
#include <imgui.h>
#include <imgui_internal.h>
#include "Physics2D/CollisionChannelRegistry.h"
#include "Resource/ResourceImporter.h"

namespace Luden
{
	constexpr ImGuiTreeNodeFlags innerTreeNodeFlags = ImGuiTreeNodeFlags_OpenOnDoubleClick |
		ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding;

	void InspectorPanel::SetContext(const std::shared_ptr<Scene>& context, SceneHierarchyPanel* sceneHierarchyPanel, EditorApplication* editorApplication)
	{
		m_Context = context;
		m_SceneHierarchyPanel = sceneHierarchyPanel;
		m_EditorApplication = editorApplication;
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

		ImGui::PushID(static_cast<int>(entity.UUID()));

		//Tag
		{
			std::string text = entity.Tag();

			char buffer[256];
			memset(buffer, 0, sizeof(buffer));
			strcpy_s(buffer, text.c_str());
			
			ImGui::SetNextItemWidth(200.0f);
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
				DisplayComponentInPopup<Camera2DComponent>(ICON_FA_CAMERA " Camera2D Component");
				DisplayComponentInPopup<BoxCollider2DComponent>(ICON_FA_SQUARE " Box Collider 2D Component");
				DisplayComponentInPopup<CircleCollider2DComponent>(ICON_FA_CIRCLE " Circle Collider 2D Component");
				DisplayComponentInPopup<RigidBody2DComponent>(ICON_FA_CUBES " RigidBody 2D Component");
				DisplayComponentInPopup<PrefabComponent>(ICON_FA_CUBE " Prefab Component");
				DisplayComponentInPopup<NativeScriptComponent>(ICON_FA_CODE " Native Script Component");
				DisplayComponentInPopup<SpriteAnimatorComponent>(ICON_FA_PLAY " Animation Component");
				DisplayComponentInPopup<TextComponent>(ICON_FA_FONT " Text Component");
				DisplayComponentInPopup<SpriteRendererComponent>(ICON_FA_IMAGE " Sprite Renderer Component");
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
						//TODO: 
					});

				DisplayComponentInInspector<Camera2DComponent>(ICON_FA_CAMERA " Camera2D Component", entity, true, [&]()
					{
						auto& cameraComponent = entity.Get<Camera2DComponent>();
						auto& camera = cameraComponent.Camera; 

						ImGuiUtils::PrefixLabel("Camera Type");
						const char* types[] = { "None", "FollowXY", "Box" };
						int currentType = static_cast<int>(camera.GetCameraType()) + 1;

						if (ImGui::Combo("##CameraType", &currentType, types, IM_ARRAYSIZE(types)))
							camera.SetCameraType(static_cast<Camera2D::Type>(currentType - 1));

						ImGuiUtils::PrefixLabel("Primary");
						ImGui::Checkbox("##Primary", &cameraComponent.Primary);

						ImGui::Separator();

						ImGuiUtils::PrefixLabel("Position");
						glm::vec2 position = camera.GetPosition();
						if (ImGuiUtils::DragFloat2Colored("##CameraPosition", &position.x, 0.1f))
							camera.SetPosition(position);

						ImGuiUtils::PrefixLabel("View Center");
						glm::vec2 viewCenter = { camera.GetView().getCenter().x, camera.GetView().getCenter().y };
						if (ImGuiUtils::DragFloat2Colored("##ViewCenter", &viewCenter.x, 0.1f))
						{

						}

						float rotation = camera.GetRotation();
						ImGuiUtils::PrefixLabel("Rotation");
						if (ImGui::DragFloat("##CameraRotation", &rotation, 1.0f, -360.0f, 360.0f))
							camera.SetRotation(rotation);
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
						auto& registry = CollisionChannelRegistry::Instance();
						const auto& allChannels = registry.GetAllChannels();

						ImGuiUtils::PrefixLabel("Offset");
						ImGuiUtils::DragFloat2Colored("##Offset", &box.Offset.x, 0.1f);

						ImGuiUtils::PrefixLabel("Size");
						ImGuiUtils::DragFloat2Colored("##Size", &box.Size.x, 0.1f);

						ImGuiUtils::PrefixLabel("Density");
						ImGui::DragFloat("##Density", &box.Density, 0.1f, 0.0f, 10.0f);

						ImGuiUtils::PrefixLabel("Friction");
						ImGui::DragFloat("##Friction", &box.Friction, 0.1f, 0.0f, 10.0f);

						ImGui::Separator();
						ImGui::Text("Collision Filtering");
						ImGui::Separator();

						// Category Bits (I am...)
						ImGuiUtils::PrefixLabel("Category (I am)");
						if (ImGui::BeginCombo("##CategoryBits", "Select..."))
						{
							for (const auto& channel : allChannels)
							{
								bool isSelected = (box.CategoryBits & channel.Bit) != 0;

								if (ImGui::Checkbox(channel.Name.c_str(), &isSelected))
								{
									if (isSelected)
										box.CategoryBits |= channel.Bit;  // Add bit
									else
										box.CategoryBits &= ~channel.Bit; // Remove bit
								}
							}
							ImGui::EndCombo();
						}

						// Show selected categories
						{
							ImGui::Indent();
							ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.7f, 0.7f, 0.7f, 1.0f));

							if (box.CategoryBits == 0)
							{
								ImGui::TextWrapped("(No categories selected)");
							}
							else
							{
								std::string selectedText = "";
								for (const auto& channel : allChannels)
								{
									if (box.CategoryBits & channel.Bit)
									{
										if (!selectedText.empty())
											selectedText += ", ";
										selectedText += channel.Name;
									}
								}
								ImGui::TextWrapped("%s", selectedText.c_str());
							}

							ImGui::PopStyleColor();
							ImGui::Unindent();
						}

						// Mask Bits 
						ImGuiUtils::PrefixLabel("Mask (Collides with)");
						if (ImGui::BeginCombo("##MaskBits", "Select..."))
						{
							for (const auto& channel : allChannels)
							{
								bool isSelected = (box.MaskBits & channel.Bit) != 0;

								if (ImGui::Checkbox(channel.Name.c_str(), &isSelected))
								{
									if (isSelected)
										box.MaskBits |= channel.Bit;
									else
										box.MaskBits &= ~channel.Bit;
								}
							}
							ImGui::EndCombo();
						}

						// Show selected masks
						{
							ImGui::Indent();
							ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.7f, 0.7f, 0.7f, 1.0f));

							if (box.MaskBits == 0)
							{
								ImGui::TextWrapped("(Collides with nothing)");
							}
							else
							{
								std::string selectedText = "";
								for (const auto& channel : allChannels)
								{
									if (box.MaskBits & channel.Bit)
									{
										if (!selectedText.empty())
											selectedText += ", ";
										selectedText += channel.Name;
									}
								}
								ImGui::TextWrapped("%s", selectedText.c_str());
							}

							ImGui::PopStyleColor();
							ImGui::Unindent();
						}

						// Group Index
						ImGuiUtils::PrefixLabel("Group Index");
						int groupIndex = static_cast<int>(box.GroupIndex);
						if (ImGui::DragInt("##GroupIndex", &groupIndex, 1, -32768, 32767))
						{
							box.GroupIndex = static_cast<uint16_t>(groupIndex);
						}

						if (ImGui::IsItemHovered())
						{
							ImGui::SetTooltip(
								"Group collision rules:\n"
								" > 0: Always collide with same group\n"
								" < 0: Never collide with same group\n"
								" = 0: Use category/mask filtering"
							);
						}

						// Quick Presets
						if (ImGui::TreeNode("Quick Presets"))
						{
							if (ImGui::Button("Collide With All", ImVec2(-1, 0)))
							{
								box.MaskBits = 0xFFFF; // All bits set
							}

							if (ImGui::Button("Collide With None", ImVec2(-1, 0)))
							{
								box.MaskBits = 0;
							}

							if (ImGui::Button("Reset to Default", ImVec2(-1, 0)))
							{
								box.CategoryBits = registry.GetChannelBit("Default");
								box.MaskBits = 0xFFFF;
								box.GroupIndex = 0;
							}

							ImGui::TreePop();
						}

						// Debug Info
						if (ImGui::TreeNode("Debug Info"))
						{
							ImGui::Text("Category Bits: 0x%04X (%d)", box.CategoryBits, box.CategoryBits);
							ImGui::Text("Mask Bits: 0x%04X (%d)", box.MaskBits, box.MaskBits);
							ImGui::Text("Group Index: %d", (int16_t)box.GroupIndex);
							ImGui::TreePop();
						}
					});

				DisplayComponentInInspector<CircleCollider2DComponent>(ICON_FA_CIRCLE " Circle Collider 2D Component", entity, true, [&]()
					{
						auto& circle = entity.Get<CircleCollider2DComponent>();
						auto& registry = CollisionChannelRegistry::Instance();
						const auto& allChannels = registry.GetAllChannels();

						ImGuiUtils::PrefixLabel("Offset");
						ImGuiUtils::DragFloat2Colored("##Offset", &circle.Offset.x, 0.1f);

						ImGuiUtils::PrefixLabel("Radius");
						ImGui::DragFloat("##Radius", &circle.Radius, 0.1f, 0.0f, 100.0f);

						ImGuiUtils::PrefixLabel("Density");
						ImGui::DragFloat("##Density", &circle.Density, 0.1f, 0.0f, 10.0f);

						ImGuiUtils::PrefixLabel("Friction");
						ImGui::DragFloat("##Friction", &circle.Friction, 0.1f, 0.0f, 10.0f);

						ImGui::Separator();
						ImGui::Text("Collision Filtering");
						ImGui::Separator();

						// Category Bits
						ImGuiUtils::PrefixLabel("Category");
						if (ImGui::BeginCombo("##CategoryBits", "Select..."))
						{
							for (const auto& channel : allChannels)
							{
								bool isSelected = (circle.CategoryBits & channel.Bit) != 0;

								if (ImGui::Checkbox(channel.Name.c_str(), &isSelected))
								{
									if (isSelected)
										circle.CategoryBits |= channel.Bit;
									else
										circle.CategoryBits &= ~channel.Bit;
								}
							}
							ImGui::EndCombo();
						}

						// Show selected
						{
							ImGui::Indent();
							ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.7f, 0.7f, 0.7f, 1.0f));

							if (circle.CategoryBits == 0)
							{
								ImGui::TextWrapped("(No categories selected)");
							}
							else
							{
								std::string selectedText = "";
								for (const auto& channel : allChannels)
								{
									if (circle.CategoryBits & channel.Bit)
									{
										if (!selectedText.empty())
											selectedText += ", ";
										selectedText += channel.Name;
									}
								}
								ImGui::TextWrapped("%s", selectedText.c_str());
							}

							ImGui::PopStyleColor();
							ImGui::Unindent();
						}

						// Mask Bits
						ImGuiUtils::PrefixLabel("Mask");
						if (ImGui::BeginCombo("##MaskBits", "Select..."))
						{
							for (const auto& channel : allChannels)
							{
								bool isSelected = (circle.MaskBits & channel.Bit) != 0;

								if (ImGui::Checkbox(channel.Name.c_str(), &isSelected))
								{
									if (isSelected)
										circle.MaskBits |= channel.Bit;
									else
										circle.MaskBits &= ~channel.Bit;
								}
							}
							ImGui::EndCombo();
						}

						// Show selected
						{
							ImGui::Indent();
							ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.7f, 0.7f, 0.7f, 1.0f));

							if (circle.MaskBits == 0)
							{
								ImGui::TextWrapped("(Collides with nothing)");
							}
							else
							{
								std::string selectedText = "";
								for (const auto& channel : allChannels)
								{
									if (circle.MaskBits & channel.Bit)
									{
										if (!selectedText.empty())
											selectedText += ", ";
										selectedText += channel.Name;
									}
								}
								ImGui::TextWrapped("%s", selectedText.c_str());
							}

							ImGui::PopStyleColor();
							ImGui::Unindent();
						}

						// Group Index
						ImGuiUtils::PrefixLabel("Group Index");
						int groupIndex = static_cast<int>(circle.GroupIndex);
						if (ImGui::DragInt("##GroupIndex", &groupIndex, 1, -32768, 32767))
						{
							circle.GroupIndex = static_cast<uint16_t>(groupIndex);
						}

						if (ImGui::IsItemHovered())
						{
							ImGui::SetTooltip(
								"Group collision rules:\n"
								" > 0: Always collide with same group\n"
								" < 0: Never collide with same group\n"
								" = 0: Use category/mask filtering"
							);
						}

						// Quick Presets
						if (ImGui::TreeNode("Quick Presets"))
						{
							if (ImGui::Button("Collide With All", ImVec2(-1, 0)))
							{
								circle.MaskBits = 0xFFFF;
							}

							if (ImGui::Button("Collide With None", ImVec2(-1, 0)))
							{
								circle.MaskBits = 0;
							}

							if (ImGui::Button("Reset to Default", ImVec2(-1, 0)))
							{
								circle.CategoryBits = registry.GetChannelBit("Default");
								circle.MaskBits = 0xFFFF;
								circle.GroupIndex = 0;
							}

							ImGui::TreePop();
						}

						// Debug Info
						if (ImGui::TreeNode("Debug Info"))
						{
							ImGui::Text("Category Bits: 0x%04X (%d)", circle.CategoryBits, circle.CategoryBits);
							ImGui::Text("Mask Bits: 0x%04X (%d)", circle.MaskBits, circle.MaskBits);
							ImGui::Text("Group Index: %d", (int16_t)circle.GroupIndex);
							ImGui::TreePop();
						}
				});

				DisplayComponentInInspector<PrefabComponent>(ICON_FA_CUBE " Prefab Component", entity, true, [&]()
				{
					auto& prefabComp = entity.Get<PrefabComponent>();
					auto resourceManager = Project::GetResourceManager();

					ImGuiUtils::PrefixLabel("Prefab");

					std::string currentPrefabName = "None";
					if (prefabComp.PrefabID != 0)
					{
						auto prefab = ResourceManager::GetResource<Prefab>(prefabComp.PrefabID);
						if (prefab)
						{
							const auto& metadata = Project::GetEditorResourceManager()->GetMetadata(prefabComp.PrefabID);
							currentPrefabName = metadata.FilePath.filename().string();
						}
					}

					if (ImGui::BeginCombo("##PrefabSelector", currentPrefabName.c_str()))
					{
						if (ImGui::Selectable("None", prefabComp.PrefabID == 0))
						{
							prefabComp.PrefabID = 0;
						}

						auto prefabHandles = resourceManager->GetAllResourcesWithType(ResourceType::Prefab);

						for (auto handle : prefabHandles)
						{
							auto prefab = ResourceManager::GetResource<Prefab>(handle);
							if (!prefab)
								continue;

							const auto& metadata = Project::GetEditorResourceManager()->GetMetadata(handle);
							std::string displayName = metadata.FilePath.filename().string();

							bool isSelected = (prefabComp.PrefabID == handle);

							if (ImGui::Selectable(displayName.c_str(), isSelected))
							{
								prefabComp.PrefabID = handle;
							}
						}

						ImGui::EndCombo();
					}

					if (ImGui::BeginDragDropTarget())
					{
						if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_FILE_PATH"))
						{
							std::filesystem::path draggedPath = std::filesystem::path(static_cast<const char*>(payload->Data));

							if (FileSystem::GetExtension(draggedPath) == ".lprefab")
							{
								ResourceHandle handle = Project::GetEditorResourceManager()->GetResourceHandleFromFilePath(draggedPath);
								prefabComp.PrefabID = handle;
							}
						}
						ImGui::EndDragDropTarget();
					}

					ImGui::SameLine();

					if (prefabComp.PrefabID != 0)
					{
						if (ImGui::Button(ICON_FA_FOLDER_OPEN))
						{
							if (m_EditorApplication)
							{
								auto path = Project::GetEditorResourceManager()->GetFileSystemPath(prefabComp.PrefabID);
								path = Project::GetEditorResourceManager()->GetRelativePath(path);
								m_EditorApplication->RequestOpenResource(path);
							}
						}
						if (ImGui::IsItemHovered())
						{
							ImGui::SetTooltip("Open Prefab Editor");
						}
					}

					ImGui::Separator();

					ImGuiUtils::PrefixLabel("Entity ID");
					ImGui::Text("%llu", prefabComp.EntityID);

					ImGuiUtils::PrefixLabel("Prefab Handle");
					ImGui::Text("%llu", prefabComp.PrefabID);

					ImGui::Separator();

					if (prefabComp.PrefabID != 0)
					{
						if (ImGui::Button(ICON_FA_UPLOAD " Apply to Prefab", ImVec2(-1, 0)))
						{
							ImGui::OpenPopup("ApplyToPrefabConfirmation");
						}
						if (ImGui::IsItemHovered())
						{
							ImGui::SetTooltip("Apply changes from this entity back to the prefab");
						}

						if (ImGui::BeginPopupModal("ApplyToPrefabConfirmation", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
						{
							ImGui::TextColored(ImVec4(0.0f, 0.8f, 1.0f, 1.0f), ICON_FA_INFO " Apply to Prefab");
							ImGui::Text("This will update the prefab with changes from this entity.");
							ImGui::Text("All other prefab instances in ALL scenes will be affected.");
							ImGui::Spacing();
							ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.0f, 1.0f), ICON_FA_TRIANGLE_EXCLAMATION " Warning");
							ImGui::Text("This operation cannot be undone!");
							ImGui::Separator();

							if (ImGui::Button("Yes, Apply", ImVec2(120, 0)))
							{
								auto prefab = ResourceManager::GetResource<Prefab>(prefabComp.PrefabID);
								if (prefab)
								{
									ApplyEntityToPrefab(entity, prefab);

									std::cout << "[InspectorPanel] Changes applied to prefab successfully!" << std::endl;
								}
								ImGui::CloseCurrentPopup();
							}

							ImGui::SameLine();

							if (ImGui::Button("Cancel", ImVec2(120, 0)))
							{
								ImGui::CloseCurrentPopup();
							}

							ImGui::EndPopup();
						}
					}

					if (ImGui::Button(ICON_FA_LINK_SLASH " Break Prefab Link", ImVec2(-1, 0)))
					{
						entity.Remove<PrefabComponent>();
					}
					if (ImGui::IsItemHovered())
					{
						ImGui::SetTooltip("Remove prefab connection and make this a regular entity");
					}

					if (prefabComp.PrefabID != 0)
					{
						if (ImGui::Button(ICON_FA_ROTATE " Revert to Prefab", ImVec2(-1, 0)))
						{
							ImGui::OpenPopup("RevertConfirmation");
						}
						if (ImGui::IsItemHovered())
						{
							ImGui::SetTooltip("Discard all changes and reload from prefab");
						}

						if (ImGui::BeginPopupModal("RevertConfirmation", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
						{
							ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.0f, 1.0f), ICON_FA_TRIANGLE_EXCLAMATION " Warning");
							ImGui::Text("This will revert all changes to match the prefab.");
							ImGui::Text("Are you sure?");
							ImGui::Separator();

							if (ImGui::Button("Yes, Revert", ImVec2(120, 0)))
							{
								auto prefab = ResourceManager::GetResource<Prefab>(prefabComp.PrefabID);
								if (prefab)
								{
									auto scene = entity.GetScene();
									Entity parent = entity.GetParent();
									glm::vec3 pos = entity.Get<TransformComponent>().Translation;
									glm::vec3 rot(entity.Get<TransformComponent>().angle, 0.0f, 0.0f);
									glm::vec3 scale = entity.Get<TransformComponent>().Scale;

									scene->DestroyEntity(entity);

									Entity newInstance = scene->Instantiate(prefab, &pos, &rot, &scale);
									if (parent.IsValid())
										newInstance.SetParent(parent);

									m_SceneHierarchyPanel->SetSelectedEntity(newInstance);
								}
								ImGui::CloseCurrentPopup();
							}

							ImGui::SameLine();

							if (ImGui::Button("Cancel", ImVec2(120, 0)))
							{
								ImGui::CloseCurrentPopup();
							}

							ImGui::EndPopup();
						}
					}
				});

				DisplayComponentInInspector<NativeScriptComponent>( ICON_FA_CODE " Native Script", entity, true, [&]()
				{
						auto& nsc = entity.Get<NativeScriptComponent>();
						auto resourceManager = Project::GetResourceManager();

						ImGuiUtils::PrefixLabel("Script");

						// Current script name
						std::string currentScriptName = "None";
						if (nsc.ScriptHandle != 0)
						{
							auto script = ResourceManager::GetResource<NativeScript>(nsc.ScriptHandle);
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
								auto script = ResourceManager::GetResource<NativeScript>(handle);

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
							auto script = ResourceManager::GetResource<NativeScript>(nsc.ScriptHandle);
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

				DisplayComponentInInspector<SpriteAnimatorComponent>(ICON_FA_PLAY " Animation Component", entity, true, [&]()
				{
						auto& animComp = entity.Get<SpriteAnimatorComponent>();

						ImGui::Text("Animations:");
						ImGui::Separator();

						for (size_t i = 0; i < animComp.animationHandles.size(); i++)
						{
							ImGui::PushID((int)i);

							auto animRes = ResourceManager::GetResource<Animation>(animComp.animationHandles[i]);
							std::string label = animRes ? animRes->GetName() : "Unknown";
							bool isPlaying = (animComp.currentAnimationIndex == i);

							if (isPlaying)
								ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.6f, 0.2f, 1.0f));

							if (ImGui::Button((label + "##" + std::to_string(i)).c_str(), ImVec2(150, 0)))
							{
								if (m_EditorApplication)
								{
									auto path = Project::GetEditorResourceManager()->GetFileSystemPath(animComp.animationHandles[i]);
									path = Project::GetEditorResourceManager()->GetRelativePath(path);
									m_EditorApplication->RequestOpenResource(path);
								}
							}

							if (isPlaying)
								ImGui::PopStyleColor();

							ImGui::SameLine();

							if (ImGui::Button((std::string(ICON_FA_PLAY) + "##Play" + std::to_string(i)).c_str()))
							{
								animComp.currentAnimationIndex = i;
								animComp.currentFrame = 0;
								animComp.frameTimer = 0;
							}

							ImGui::SameLine();

							if (ImGui::Button((std::string(ICON_FA_TRASH) + "##Del" + std::to_string(i)).c_str()))
							{
								animComp.animationHandles.erase(animComp.animationHandles.begin() + i);
								if (animComp.currentAnimationIndex >= animComp.animationHandles.size())
									animComp.currentAnimationIndex = 0;
								ImGui::PopID();
								break;
							}

							if (animRes)
							{
								ImGui::SameLine();
								ImGui::TextDisabled("(%d frames)", (int)animRes->GetFrameCount());
							}

							ImGui::PopID();
						}

						ImGui::Separator();

						if (ImGui::Button(ICON_FA_PLUS " Add Animation", ImVec2(-1, 0)))
						{
							ImGui::OpenPopup("SelectAnimationPopup");
						}

						if (ImGui::BeginPopup("SelectAnimationPopup"))
						{
							ImGui::Text("Select Animation:");
							ImGui::Separator();

							if (ImGui::BeginDragDropTarget())
							{
								if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_FILE_PATH"))
								{
									std::filesystem::path path = std::filesystem::path(static_cast<const char*>(payload->Data));

									if (FileSystem::GetExtension(path) == ".lanim")
									{
										ResourceHandle handle = Project::GetEditorResourceManager()->GetResourceHandleFromFilePath(path);
										animComp.animationHandles.push_back(handle);
										ImGui::CloseCurrentPopup();
									}
								}
								ImGui::EndDragDropTarget();
								ImGui::EndPopup();
							}

							auto animHandles = Project::GetEditorResourceManager()->GetAllResourcesWithType(ResourceType::Animation);

							for (auto handle : animHandles)
							{
								auto anim = ResourceManager::GetResource<Animation>(handle);

								if (!anim)
									continue;

								std::string animName = anim->GetName() + "##";
								if (ImGui::Selectable(animName.c_str()))
								{
									animComp.animationHandles.push_back(handle);
									ImGui::CloseCurrentPopup();
								}
							}

							ImGui::EndPopup();
						}

						ImGui::Separator();

						ImGuiUtils::PrefixLabel("Current Animation");
						if (animComp.animationHandles.empty())
						{
							ImGui::TextDisabled("No animations");
						}
						else
						{
							int currentIdx = (int)animComp.currentAnimationIndex;
							if (ImGui::DragInt("##CurrentAnim", &currentIdx, 1, 0, (int)animComp.animationHandles.size() - 1))
							{
								animComp.currentAnimationIndex = (size_t)currentIdx;
								animComp.currentFrame = 0;
								animComp.frameTimer = 0;
							}
						}

						ImGuiUtils::PrefixLabel("Speed");
						float speed = animComp.playbackSpeed;
						if (ImGui::DragFloat("##Speed", &speed, 1, 1, 100))
						{
							animComp.playbackSpeed = std::max(1.f, speed);
						}

						ImGuiUtils::PrefixLabel("Current Frame");
						ImGui::BeginDisabled();
						int frame = (int)animComp.currentFrame;
						ImGui::DragInt("##Frame", &frame);
						ImGui::EndDisabled();
				});

				DisplayComponentInInspector<TextComponent>(ICON_FA_FONT " Text Component", entity, true, [&]()
					{
						auto& textComp = entity.Get<TextComponent>();

						ImGuiUtils::PrefixLabel("Font");
						if (ImGuiUtils::ResourceButton(textComp.fontHandle, ResourceType::Font))
						{
							// Open font editor
						}

						ImGui::Separator();
						ImGui::Text("Text Content");
						ImGui::Separator();

						ImGuiUtils::PrefixLabel("Text");
						char buffer[512];
						strncpy(buffer, textComp.text.c_str(), sizeof(buffer));
						buffer[sizeof(buffer) - 1] = '\0';

						if (ImGui::InputTextMultiline("##Text", buffer, sizeof(buffer), ImVec2(-1, 80)))
						{
							textComp.text = buffer;
						}

						ImGuiUtils::PrefixLabel("Size");
						int size = (int)textComp.characterSize;
						if (ImGui::DragInt("##CharSize", &size, 1, 8, 200))
						{
							textComp.characterSize = (uint32_t)size;
						}

						ImGui::Separator();
						ImGui::Text("Colors");
						ImGui::Separator();

						ImGuiUtils::PrefixLabel("Fill Color");
						ImVec4 fillColor = {
							textComp.fillColor.r / 255.0f,
							textComp.fillColor.g / 255.0f,
							textComp.fillColor.b / 255.0f,
							textComp.fillColor.a / 255.0f
						};

						if (ImGui::ColorEdit4("##FillColor", &fillColor.x))
						{
							textComp.fillColor = sf::Color(
								static_cast<uint8_t>(fillColor.x * 255.0f),
								static_cast<uint8_t>(fillColor.y * 255.0f),
								static_cast<uint8_t>(fillColor.z * 255.0f),
								static_cast<uint8_t>(fillColor.w * 255.0f)
							);
						}

						ImGuiUtils::PrefixLabel("Outline Color");
						ImVec4 outlineColor = {
							textComp.outlineColor.r / 255.0f,
							textComp.outlineColor.g / 255.0f,
							textComp.outlineColor.b / 255.0f,
							textComp.outlineColor.a / 255.0f
						};

						if (ImGui::ColorEdit4("##OutlineColor", &outlineColor.x))
						{
							textComp.outlineColor = sf::Color(
								static_cast<uint8_t>(outlineColor.x * 255.0f),
								static_cast<uint8_t>(outlineColor.y * 255.0f),
								static_cast<uint8_t>(outlineColor.z * 255.0f),
								static_cast<uint8_t>(outlineColor.w * 255.0f)
							);
						}

						ImGuiUtils::PrefixLabel("Outline Thickness");
						ImGui::DragFloat("##OutlineThickness", &textComp.outlineThickness, 0.1f, 0.0f, 10.0f);

						ImGui::Separator();
						ImGui::Text("Spacing");
						ImGui::Separator();

						ImGuiUtils::PrefixLabel("Letter Spacing");
						ImGui::DragFloat("##LetterSpacing", &textComp.letterSpacing, 0.01f, 0.0f, 5.0f);

						ImGuiUtils::PrefixLabel("Line Spacing");
						ImGui::DragFloat("##LineSpacing", &textComp.lineSpacing, 0.01f, 0.0f, 5.0f);

						ImGui::Separator();
						ImGui::Text("Style");
						ImGui::Separator();

						bool isBold = textComp.style & TextComponent::Bold;
						bool isItalic = textComp.style & TextComponent::Italic;
						bool isUnderlined = textComp.style & TextComponent::Underlined;
						bool isStrikeThrough = textComp.style & TextComponent::StrikeThrough;

						if (ImGui::Checkbox("Bold", &isBold))
						{
							if (isBold)
								textComp.style |= TextComponent::Bold;
							else
								textComp.style &= ~TextComponent::Bold;
						}

						ImGui::SameLine();

						if (ImGui::Checkbox("Italic", &isItalic))
						{
							if (isItalic)
								textComp.style |= TextComponent::Italic;
							else
								textComp.style &= ~TextComponent::Italic;
						}

						if (ImGui::Checkbox("Underlined", &isUnderlined))
						{
							if (isUnderlined)
								textComp.style |= TextComponent::Underlined;
							else
								textComp.style &= ~TextComponent::Underlined;
						}

						ImGui::SameLine();

						if (ImGui::Checkbox("Strike Through", &isStrikeThrough))
						{
							if (isStrikeThrough)
								textComp.style |= TextComponent::StrikeThrough;
							else
								textComp.style &= ~TextComponent::StrikeThrough;
						}

						ImGui::Separator();
						ImGui::Text("Alignment");
						ImGui::Separator();

						ImGuiUtils::PrefixLabel("Line Alignment");
						const char* alignments[] = { "Default", "Left", "Center", "Right" };
						int currentAlignment = (int)textComp.lineAlignment;

						if (ImGui::Combo("##LineAlignment", &currentAlignment, alignments, IM_ARRAYSIZE(alignments)))
						{
							textComp.lineAlignment = (TextComponent::LineAlignment)currentAlignment;
						}

						ImGuiUtils::PrefixLabel("Orientation");
						const char* orientations[] = { "Default", "Top to Bottom", "Bottom to Top" };
						int currentOrientation = (int)textComp.textOrientation;

						if (ImGui::Combo("##TextOrientation", &currentOrientation, orientations, IM_ARRAYSIZE(orientations)))
						{
							textComp.textOrientation = (TextComponent::TextOrientation)currentOrientation;
						}
					});

				DisplayComponentInInspector<SpriteRendererComponent>(ICON_FA_IMAGE " Sprite Renderer Component", entity, true, [&]()
					{
						auto& spriteRendererComponent = entity.Get<SpriteRendererComponent>();
						auto sprite = ResourceManager::GetResource<Sprite>(spriteRendererComponent.spriteHandle);
						ImGuiUtils::PrefixLabel(ICON_FA_FILE_IMAGE " Current");
						if (ImGuiUtils::ResourceButton(spriteRendererComponent.spriteHandle, ResourceType::Sprite))
						{
							//TODO: 
						}

						ImGuiUtils::PrefixLabel(ICON_FA_PALETTE " Tint");

						ImVec4 color = {
							spriteRendererComponent.tint.r / 255.0f,
							spriteRendererComponent.tint.g / 255.0f,
							spriteRendererComponent.tint.b / 255.0f,
							spriteRendererComponent.tint.a / 255.0f
						};

						if (ImGui::ColorEdit4("##SpriteTint", &color.x))
						{
							spriteRendererComponent.tint = sf::Color(
								static_cast<uint8_t>(color.x * 255.0f),
								static_cast<uint8_t>(color.y * 255.0f),
								static_cast<uint8_t>(color.z * 255.0f),
								static_cast<uint8_t>(color.w * 255.0f)
							);
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
						ImGui::Text("Positions:");
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

						ImGuiUtils::PrefixLabel("Change Animation");
						ImGui::Checkbox("##ChangeAnim", &stateComponent.changeAnimation);
					});

				DisplayComponentInInspector<TransformComponent>(ICON_FA_ARROWS_UP_DOWN_LEFT_RIGHT " Transform Component", entity, true, [&]()
					{
						auto& transformComponent = entity.Get<TransformComponent>();

						ImGuiUtils::PrefixLabel("Position");
						ImGuiUtils::DragFloat2Colored("##Pos", &transformComponent.Translation.x, 0.1f);

						ImGuiUtils::PrefixLabel("Scale");
						ImGuiUtils::DragFloat2Colored("##Scale", &transformComponent.Scale.x, 0.1f);

						ImGuiUtils::PrefixLabel("Angle" );
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

	void InspectorPanel::ApplyEntityToPrefab(Entity entity, std::shared_ptr<Prefab> prefab)
	{
		if (!prefab || !prefab->GetScene())
			return;

		Entity prefabRootEntity = prefab->GetRootEntity();
		if (!prefabRootEntity.IsValid())
			return;

		auto prefabScene = prefab->GetScene();
		prefabScene->CopyAllComponents(prefabRootEntity, entity, true);

		ApplyChildrenToPrefab(entity, prefabRootEntity);

		ResourceImporter::Serialize(prefab);

		std::cout << "Entity applied to prefab successfully!" << std::endl;
	}

	void InspectorPanel::ApplyChildrenToPrefab(Entity sourceEntity, Entity prefabEntity)
	{
		auto sourceChildren = sourceEntity.Children();
		auto prefabChildren = prefabEntity.Children();

		size_t minSize = std::min(sourceChildren.size(), prefabChildren.size());

		for (size_t i = 0; i < minSize; i++)
		{
			Entity sourceChild = sourceEntity.GetScene()->TryGetEntityWithUUID(sourceChildren[i]);
			Entity prefabChild = prefabEntity.GetScene()->TryGetEntityWithUUID(prefabChildren[i]);

			if (sourceChild.IsValid() && prefabChild.IsValid())
			{
				prefabChild.GetScene()->CopyAllComponents(prefabChild, sourceChild, true);

				ApplyChildrenToPrefab(sourceChild, prefabChild);
			}
		}
	}
}