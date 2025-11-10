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
	void SceneHierarchyPanel::SetContext(std::shared_ptr<Scene>& scene)
	{
		m_Context = scene;

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

			if (ImGui::Button(ICON_FA_PLUS " Add Entity"))
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
					auto newEntity = m_Context->CreateEntity("Empty Entity");
					m_SelectedEntity = newEntity;
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
		m_MouseReleased = false;
	}

	void SceneHierarchyPanel::DrawEntityInSceneTree(Entity entity)
	{
		if (!entity.IsValid() || !entity.Has<RelationshipComponent>())
			return;

		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);

		ImGui::PushID(entity.UUID());


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

			if (ImGui::MenuItem(ICON_FA_PLUS " Create Child"))
			{
				Entity newChild = m_Context->CreateChildEntity(entity, "New Child Entity");
				m_SelectedEntity = newChild;
			}
			if (ImGui::MenuItem(ICON_FA_TRASH " Delete"))
			{
				m_Context->DestroyEntity(entity);
				m_SelectedEntity = {};
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
}
