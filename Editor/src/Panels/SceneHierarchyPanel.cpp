#include "Panels/SceneHierarchyPanel.h"
#include "Core/UUID.h"
#include "Scene/Scene.h"

#include "imgui.h"
#include "imgui_internal.h"

namespace Luden
{
	void SceneHierarchyPanel::SetContext(std::shared_ptr<Scene>& scene)
	{
		m_Context = scene;

		//Select the first root entity
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

		//TODO: Parenting and Reparentinf of entities
		/*
		if (ImGui::BeginDragDropTarget()) 

			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_ENTITY")) 
			{
				auto payload_id = static_cast<const UUID*>(payload->Data);
				if (payload_id != nullptr) 
				{
					auto payload_entity = m_Context->FindEntityByUUID(*payload_id);
					if (payload_entity) 
					{
						// reparent entity to root level
						payload_entity.Reparent({});
					}
				}
			}
			ImGui::EndDragDropTarget();
		}
		*/

		/* Add Entity Button */ 
		{
			ImGui::SameLine(ImGui::GetContentRegionAvail().x - ImGui::GetFontSize());
			float line_width = ImGui::GetFontSize() + GImGui->Style.FramePadding.y * 2.0f;

			if (ImGui::Button("+", ImVec2(line_width, 0)))
			{
				ImGui::OpenPopup("AddEntity");
			}

			if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal)) 
			{
				ImGui::SetTooltip("Add Entity");
			}

			if (ImGui::BeginPopup("AddEntity")) 
			{
				if (ImGui::MenuItem("Empty Entity")) 
				{
					auto new_entity = m_Context->CreateEntity("Empty Entity");
					m_SelectedEntity = new_entity;
				}
				ImGui::EndPopup();
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

		auto& entities = m_Context->GetEntityManager().GetEntities();
		for(auto& entity: entities)
		{
			DrawEntityInSceneTree(entity);
		}


		if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered()) {
			//TODO: m_SelectedEntity = {};
		}

		ImGui::EndTable();
		ImGui::Dummy(ImVec2(0, 60));
		ImGui::EndChild();
		m_MouseReleased = false;
	}
	void SceneHierarchyPanel::DrawEntityInSceneTree(Entity entity)
	{
		ImGui::TableNextRow();                // satýr aç
		ImGui::TableSetColumnIndex(0);        // sütun seç

		ImGui::PushID(entity.UUID());

		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth
			| ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_DefaultOpen
			| ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_AllowOverlap;
		

		if (m_SelectedEntity == entity)
			flags |= ImGuiTreeNodeFlags_Selected;

		bool isNodeOpen = ImGui::TreeNodeEx((void*)(uint64_t)entity, flags, "%s", entity.Tag().c_str());

		if (ImGui::IsItemClicked()) 
		{
			SetSelectedEntity(entity);
		}

		if (ImGui::BeginDragDropSource())
		{
			UUID temp = entity.UUID();
			ImGui::SetDragDropPayload("DND_ENTITY", (void*)&temp, sizeof(UUID));

			ImGui::Text("%s", std::to_string(entity.UUID()).c_str());
			ImGui::EndDragDropSource();
		}

		//TODO: Parenting on ImGui::BeginDragDropTarget()

		if (m_MouseReleased && ImGui::IsItemFocused() && ImGui::IsItemHovered()) 
		{
			SetSelectedEntity(entity);
			m_MouseReleased = false;
		}

		if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) 
		{
			ImGui::OpenPopup("SHCM");
		}

		if (ImGui::BeginPopup("SHCM"))
		{
			if (ImGui::MenuItem("Create Entity"))
			{
				//TODO: Create new entity
			}
			if (ImGui::MenuItem("Delete Entity"))
			{
				//TODO: Delete entity
			}
			ImGui::EndPopup();
		}

		if (isNodeOpen)
		{
			ImGui::TreePop();
		}

		ImGui::PopID();
	}
}
