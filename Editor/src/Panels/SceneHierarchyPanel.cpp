#include "Panels/SceneHierarchyPanel.h"
#include "Core/UUID.h"
#include "Scene/Scene.h"

#include <IconsFontAwesome7.h>
#include <imgui.h>
#include <imgui_internal.h>
#include "ImGui/ImGuiUtils.h"
#include "Utils/ImGuiStyle.h"

namespace Luden
{
	void SceneHierarchyPanel::SetContext(EditorApplication* editorApplication, std::shared_ptr<Scene>& scene)
	{
		m_Context = scene;
		m_EditorApplication = editorApplication;
		m_SelectedEntity = {};
		if (m_Context)
		{
			const Luden::EntityVec& entities = m_Context->GetEntityManager().GetEntityVec();
			if (entities.size() > 0)
			{
				const auto& entity = entities[0];
				SetSelectedEntity(entity);
			}
		}

	}
	bool SceneHierarchyPanel::IsSelectedEntityValid() const
	{
		return m_SelectedEntity && m_SelectedEntity.IsValid();
	}
	UUID SceneHierarchyPanel::GetSelectedEntityUUID()
	{
		if (m_SelectedEntity)
		{
			return m_SelectedEntity.UUID();
		}
		return -1;
	}
	void SceneHierarchyPanel::SetSelectedEntity(const Entity& entity)
	{
		m_SelectedEntity = entity;
	}
	void SceneHierarchyPanel::SetSelectedEntityWithUUID(const UUID& uuid)
	{
		if (!uuid)
			return;
		const auto& entity = m_Context->GetEntityManager().TryGetEntityWithUUID(uuid);

		if (!entity.IsValid())
			return;
		SetSelectedEntity(entity);
	}

	void SceneHierarchyPanel::RenderContent()
	{
		if (m_Context == nullptr)
			return;

		char buffer[256];
		memset(buffer, 0, sizeof(buffer));
		strcpy_s(buffer, m_Context->GetName().c_str());

		if (ImGui::InputText("##SceneName", buffer, sizeof(buffer)))
		{
			m_Context->SetName(buffer);
		}

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_ENTITY"))
			{
				auto payload_id = static_cast<const UUID*>(payload->Data);
				if (payload_id != nullptr)
				{
					Entity payload_entity = m_Context->TryGetEntityWithUUID(*payload_id);
					if (payload_entity.IsValid())
					{
						m_Context->UnparentEntity(payload_entity);
					}
				}
			}
			ImGui::EndDragDropTarget();
		}

		{
			ImGui::SameLine();

			if(ImGui::Button(ICON_FA_PLUS " Add Entity"))
			{
				ImGui::OpenPopup("AddEntity");
			}

			if (ImGui::BeginPopup("AddEntity"))
			{
				if (ImGui::MenuItem("Empty Entity"))
				{
					auto newEntity = m_Context->CreateEntity("Empty Entity");
					m_SelectedEntity = newEntity;
				}

				ImGui::Separator();

				if (ImGui::BeginMenu(ICON_FA_CUBE " Instantiate Prefab"))
				{
					auto resourceManager = Project::GetResourceManager();
					auto prefabHandles = resourceManager->GetAllResourcesWithType(ResourceType::Prefab);

					if (prefabHandles.empty())
					{
						ImGui::TextDisabled("No prefabs available");
					}
					else
					{
						for (auto handle : prefabHandles)
						{
							auto prefab = ResourceManager::GetResource<Prefab>(handle);
							if (!prefab)
								continue;

							const auto& metadata = Project::GetEditorResourceManager()->GetMetadata(handle);
							std::string displayName = metadata.FilePath.filename().string();

							if (ImGui::MenuItem(displayName.c_str()))
							{
								glm::vec3 pos(0, 0, 0);
								Entity instance = m_Context->Instantiate(prefab, &pos, &pos, &pos);
								m_SelectedEntity = instance;
							}
						}
					}

					ImGui::EndMenu();
				}

				ImGui::EndPopup();
			}

			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_FILE_PATH"))
				{
					std::filesystem::path draggedPath = std::filesystem::path(static_cast<const char*>(payload->Data));

					if (FileSystem::GetExtension(draggedPath) == ".lprefab")
					{
						ResourceHandle handle = Project::GetEditorResourceManager()->GetResourceHandleFromFilePath(draggedPath);
						auto prefab = ResourceManager::GetResource<Prefab>(handle);

						if (prefab)
						{
							glm::vec3 pos(0, 0, 0);
							Entity instance = m_Context->Instantiate(prefab, &pos, &pos, &pos);
							m_SelectedEntity = instance;
						}
					}
				}

				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_ENTITY"))
				{
					auto payload_id = static_cast<const UUID*>(payload->Data);
					if (payload_id != nullptr)
					{
						Entity payload_entity = m_Context->TryGetEntityWithUUID(*payload_id);
						if (payload_entity.IsValid())
						{
							m_Context->UnparentEntity(payload_entity);
						}
					}
				}

				ImGui::EndDragDropTarget();
			}
		}

		ImGui::Spacing();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(7, 3));
		bool child_open = ImGui::BeginChild("scenetree_child", ImVec2(0, 0), false, ImGuiWindowFlags_AlwaysUseWindowPadding);
		bool table_open = child_open && ImGui::BeginTable("SceneTreeTable", 1);
		if (!table_open) {
			ImGui::PopStyleVar();
			ImGui::EndChild();
			return;
		}
		ImGui::PopStyleVar();


		auto& allEntities = m_Context->GetEntityManager().GetEntities();

		for (auto& entity : allEntities)
		{
			if (entity.IsValid() && entity.Has<RelationshipComponent>() && entity.GetParentUUID() == 0)
			{
				DrawEntityInSceneTree(entity);
			}
		}

		if (ImGui::IsMouseDown(ImGuiMouseButton_Left) && ImGui::IsWindowHovered()) {
			m_SelectedEntity = {};
		}

		ImGui::EndTable();
		ImGui::Dummy(ImVec2(0, 60));
		ImGui::EndChild();

		if (m_ShowCreatePrefabPopup && m_EntityToConvertToPrefab.IsValid())
		{
			ImGui::OpenPopup("Create Prefab");
			m_ShowCreatePrefabPopup = false;
		}

		CreatePrefabFromEntity(m_EntityToConvertToPrefab);

		m_MouseReleased = false;
	}

	void SceneHierarchyPanel::DrawEntityInSceneTree(Entity entity)
	{
		if (!entity.IsValid() || !entity.Has<RelationshipComponent>())
			return;

		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);

		ImGui::PushID(static_cast<int>(entity.UUID()));


		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth
			| ImGuiTreeNodeFlags_OpenOnDoubleClick
			| ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_AllowOverlap;

		bool hasChildren = !entity.Children().empty();

		if (!hasChildren)
		{
			flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
		}

		if (m_SelectedEntity == entity)
			flags |= ImGuiTreeNodeFlags_Selected;

		const char* icon = ICON_FA_CIRCLE;
		bool isNodeOpen = ImGui::TreeNodeEx((void*)(uint64_t)entity.UUID(), flags, "%s %s", icon, entity.Tag().c_str());

		if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
		{
			SetSelectedEntity(entity);
		}

		if (ImGui::BeginDragDropSource())
		{
			UUID temp = entity.UUID();
			ImGui::SetDragDropPayload("DND_ENTITY", (void*)&temp, sizeof(UUID));

			ImGui::Text("%s", entity.Tag().c_str());
			ImGui::EndDragDropSource();
		}

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_ENTITY"))
			{
				auto payload_id = static_cast<const UUID*>(payload->Data);
				if (payload_id != nullptr && *payload_id != entity.UUID())
				{
					Entity childEntity = m_Context->TryGetEntityWithUUID(*payload_id);
					if (childEntity.IsValid())
					{
						m_Context->ParentEntity(childEntity, entity);
					}
				}
			}
			ImGui::EndDragDropTarget();
		}

		if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
		{
			ImGui::OpenPopup("SHCM");
		}

		if (ImGui::BeginPopup("SHCM"))
		{
			SetSelectedEntity(entity);
			ImGui::Text("%s %s", ICON_FA_CUBE, entity.Tag().c_str());
			ImGui::Separator();

			if (ImGui::MenuItem(ICON_FA_COPY " Duplicate"))
			{
				Entity duplicate = m_Context->DuplicateEntity(entity);
				SetSelectedEntity(duplicate);
			}
			if (ImGui::MenuItem(ICON_FA_PLUS " Create Child"))
			{
				Entity newChild = m_Context->CreateChildEntity(entity, "New Child Entity");
				m_SelectedEntity = newChild;
			}
			if (ImGui::BeginMenu(ICON_FA_CUBE " Add Child Prefab"))
			{
				auto resourceManager = Project::GetResourceManager();
				auto prefabHandles = resourceManager->GetAllResourcesWithType(ResourceType::Prefab);

				if (prefabHandles.empty())
				{
					ImGui::TextDisabled("No prefabs available");
				}
				else
				{
					for (auto handle : prefabHandles)
					{
						auto prefab = ResourceManager::GetResource<Prefab>(handle);
						if (!prefab)
							continue;

						const auto& metadata = Project::GetEditorResourceManager()->GetMetadata(handle);
						std::string displayName = metadata.FilePath.filename().string();

						if (ImGui::MenuItem(displayName.c_str()))
						{
							auto& transformComponent = entity.Get<TransformComponent>();
							glm::vec3 rotation{ transformComponent.angle, 0.0f, 0.0f };
							Entity instance = m_Context->InstantiateChild(prefab, entity, &transformComponent.Translation, &rotation, &transformComponent.Scale);
							m_SelectedEntity = instance;
						}
					}
				}

				ImGui::EndMenu();
			}
			if (ImGui::MenuItem(ICON_FA_TRASH " Delete"))
			{
				m_Context->DestroyEntity(entity);
				m_SelectedEntity = {};
			}
			if (ImGui::MenuItem(ICON_FA_LINK_SLASH " Unparent"))
			{
				if (entity.GetParent().IsValid())
					m_Context->UnparentEntity(entity);
			}
			ImGui::Separator();

			if (ImGui::MenuItem(ICON_FA_CUBE " Create Prefab from Entity"))
			{
				m_ShowCreatePrefabPopup = true; 
				m_EntityToConvertToPrefab = entity;
			}

			if (entity.Has<PrefabComponent>())
			{
				ImGui::Separator();
				ImGui::TextColored(ImVec4(0.5f, 0.7f, 1.0f, 1.0f), "Prefab Operations");

				if (ImGui::MenuItem(ICON_FA_FOLDER_OPEN " Open Prefab"))
				{
					auto& prefabComp = entity.Get<PrefabComponent>();
					if (m_EditorApplication)
					{
						auto path = Project::GetEditorResourceManager()->GetFileSystemPath(prefabComp.PrefabID);
						path = Project::GetEditorResourceManager()->GetRelativePath(path);
						m_EditorApplication->RequestOpenResource(path);
					}
				}

				if (ImGui::MenuItem(ICON_FA_ROTATE " Revert to Prefab"))
				{
					auto& prefabComp = entity.Get<PrefabComponent>();
					auto prefab = ResourceManager::GetResource<Prefab>(prefabComp.PrefabID);

					if (prefab)
					{
						Entity parent = entity.GetParent();
						glm::vec3 pos = entity.Get<TransformComponent>().Translation;

						m_Context->DestroyEntity(entity);

						Entity newInstance = m_Context->Instantiate(prefab, &pos, nullptr, nullptr);
						if (parent.IsValid())
							newInstance.SetParent(parent);

						SetSelectedEntity(newInstance);
					}
				}

				if (ImGui::MenuItem(ICON_FA_LINK_SLASH " Break Prefab Link"))
				{
					entity.Remove<PrefabComponent>();
				}

			}
			ImGui::EndPopup();
		}

		if (isNodeOpen && hasChildren)
		{
			for (EntityID childID : entity.Children())
			{
				Entity child = m_Context->TryGetEntityWithUUID(childID);
				if (child.IsValid())
				{
					DrawEntityInSceneTree(child);
				}
			}
			ImGui::TreePop(); 
		}

		ImGui::PopID();
	}

	void SceneHierarchyPanel::CreatePrefabFromEntity(Entity entity)
	{
		if (!entity.IsValid())
			return;

		static char prefabName[256] = "";

		if (ImGui::BeginPopupModal("Create Prefab", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
		{
			if (!entity.IsValid())
			{
				ImGui::Text("Error: Invalid entity!");
				if (ImGui::Button("Close", ImVec2(120, 0)))
				{
					prefabName[0] = '\0';
					m_EntityToConvertToPrefab = {};
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndPopup();
				return;
			}

			ImGui::Text("Create prefab from: %s", entity.Tag().c_str());
			ImGui::Separator();

			ImGui::InputText("Prefab Name", prefabName, sizeof(prefabName));

			ImGui::Spacing();

			static char m_PrefabPathBuffer[512] = "";

			ImGui::Text("Prefab Path:");
			ImGui::SetNextItemWidth(-120);
			ImGui::InputText("##ProjectPath", m_PrefabPathBuffer, sizeof(m_PrefabPathBuffer));

			ImGui::SameLine();
			if (ImGui::Button("Browse...", ImVec2(110, 0)))
			{
				std::filesystem::path selectedPath = FileSystem::OpenFolderDialog();
				if (!selectedPath.empty())
				{
					strncpy_s(m_PrefabPathBuffer, selectedPath.string().c_str(), sizeof(m_PrefabPathBuffer));
				}
			}

			ImGui::Spacing();
			ImGui::Spacing();

			if (ImGui::Button("Create", ImVec2(120, 0)))
			{
				if (prefabName[0] != '\0')
				{
					std::filesystem::path prefabPath = std::string(m_PrefabPathBuffer) + "\\" + std::string(prefabName) + ".lprefab";

					ResourceHandle prefabHandle = Project::GetEditorResourceManager()->CreateResource(ResourceType::Prefab, prefabPath);
					auto prefab = ResourceManager::GetResource<Prefab>(prefabHandle);
					prefab->Create(entity, true);

					auto& prefabComponent = entity.Add<PrefabComponent>();
					prefabComponent.EntityID = entity.UUID();
					prefabComponent.PrefabID = prefab->Handle;

					if (m_EditorApplication)
					{
						m_EditorApplication->RequestOpenResource(prefabPath);
					}

					prefabName[0] = '\0';
					m_EntityToConvertToPrefab = {};
					ImGui::CloseCurrentPopup();
				}
			}

			ImGui::SameLine();

			if (ImGui::Button("Cancel", ImVec2(120, 0)))
			{
				prefabName[0] = '\0';
				m_EntityToConvertToPrefab = {};
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}
	}
}
