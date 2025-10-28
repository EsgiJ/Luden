#include "Panels/SceneHierarchyPanel.h"
#include "Core/UUID.h"
#include "Scene/Scene.h"

#include <IconsFontAwesome7.h>
#include <imgui.h>
#include <imgui_internal.h>

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
	// --- RENDER CONTENT (K�k Varl�klar� �izme) ---
	void SceneHierarchyPanel::RenderContent()
	{
		if (m_Context == nullptr)
			return;

		// --- 1. Scene Name Input ---
		char buffer[256];
		memset(buffer, 0, sizeof(buffer));
		strcpy_s(buffer, m_Context->GetName().c_str());

		if (ImGui::InputText("##SceneName", buffer, sizeof(buffer)))
		{
			m_Context->SetName(buffer);
		}

		// --- 2. DragDrop Target (Unparenting) ---
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
						// Varl��� k�ke ta�� (ebeveynsiz yap)
						m_Context->UnparentEntity(payload_entity);
					}
				}
			}
			ImGui::EndDragDropTarget();
		}

		// --- 3. Add Entity Button ---
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
					// Yeni ebeveynsiz varl�k olu�tur
					auto newEntity = m_Context->CreateEntity("Empty Entity");
					m_SelectedEntity = newEntity;
				}
				ImGui::EndPopup();
			}
		}

		ImGui::Spacing();

		// --- 4. Scene Tree Table Setup ---
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(7, 3));
		bool child_open = ImGui::BeginChild("scenetree_child", ImVec2(0, 0), false, ImGuiWindowFlags_AlwaysUseWindowPadding);
		bool table_open = child_open && ImGui::BeginTable("SceneTreeTable", 1);
		if (!table_open) {
			ImGui::PopStyleVar();
			ImGui::EndChild();
			return;
		}
		ImGui::PopStyleVar();

		// --- 5. H�YERAR�� ��Z�M� (Sadece Ebeveynsiz Varl�klar) ---
		auto& allEntities = m_Context->GetEntityManager().GetEntities();

		for (auto& entity : allEntities)
		{
			// Ebeveynsiz varl�k (Implicit Root): Ge�erli, RelationshipComponent'e sahip VE ParentUUID'si 0 olan varl�klar.
			if (entity.IsValid() && entity.Has<RelationshipComponent>() && entity.GetParentUUID() == 0)
			{
				DrawEntityInSceneTree(entity);
			}
		}

		// --- 6. Se�im Kald�rma ---
		if (ImGui::IsMouseDown(ImGuiMouseButton_Left) && ImGui::IsWindowHovered()) {
			m_SelectedEntity = {};
		}

		// --- 7. Kapatma ---
		ImGui::EndTable();
		ImGui::Dummy(ImVec2(0, 60));
		ImGui::EndChild();
		m_MouseReleased = false;
	}

	void SceneHierarchyPanel::DrawEntityInSceneTree(Entity entity)
	{
		// 1. Varl�k ge�erli de�ilse veya RelationshipComponent'i yoksa (g�venlik kontrol�)
		if (!entity.IsValid() || !entity.Has<RelationshipComponent>())
			return;

		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);

		ImGui::PushID(entity.UUID());

		// --- A�a� D���m� Haz�rl��� ---

		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth
			| ImGuiTreeNodeFlags_OpenOnDoubleClick
			| ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_AllowOverlap;

		// Varl���n �ocuklar� var m� kontrol et
		bool hasChildren = !entity.Children().empty();

		// �ocu�u yoksa, ok simgesini g�sterme ve TreePush'� atla
		if (!hasChildren)
		{
			flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
		}

		// Se�ili varl�k bayra��
		if (m_SelectedEntity == entity)
			flags |= ImGuiTreeNodeFlags_Selected;

		// A�a� d���m�n� �iz
		// Not: ImGuiTreeNodeFlags_DefaultOpen kald�r�ld�.
		bool isNodeOpen = ImGui::TreeNodeEx((void*)(uint64_t)entity.UUID(), flags, "%s", entity.Tag().c_str());

		// --- Se�im Mant��� ---
		if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
		{
			SetSelectedEntity(entity);
		}

		// --- S�r�kleme Kayna�� (Drag Source) ---
		if (ImGui::BeginDragDropSource())
		{
			UUID temp = entity.UUID();
			ImGui::SetDragDropPayload("DND_ENTITY", (void*)&temp, sizeof(UUID));

			ImGui::Text("%s", entity.Tag().c_str());
			ImGui::EndDragDropSource();
		}

		// --- B�rakma Hedefi (Drag Target) - Parenting ---
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
						// Ebeveynlik ili�kisini kur
						m_Context->ParentEntity(childEntity, entity);
					}
				}
			}
			ImGui::EndDragDropTarget();
		}

		// --- Context Men� (Sa� T�k) Mant��� ---
		if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
		{
			ImGui::OpenPopup("SHCM");
		}

		if (ImGui::BeginPopup("SHCM"))
		{
			SetSelectedEntity(entity); // Sa� t�kland���nda se�imi g�ncelle

			if (ImGui::MenuItem("Create Child Entity"))
			{
				// Se�ili varl��a ba�l� yeni bir �ocuk varl�k olu�tur
				Entity newChild = m_Context->CreateChildEntity(entity, "New Child Entity");
				m_SelectedEntity = newChild;
			}
			if (ImGui::MenuItem("Delete Entity"))
			{
				m_Context->DestroyEntity(entity);
				m_SelectedEntity = {};
			}
			ImGui::EndPopup();
		}

		// --- �zyinelemeli �izim (Recursive Drawing) ---

		// D���m a��ksa VE �ocuklar� varsa (ImGui'nin TreePush yapt��� tek durum)
		if (isNodeOpen && hasChildren)
		{
			// �ocuklar� yinele
			for (EntityID childID : entity.Children())
			{
				Entity child = m_Context->TryGetEntityWithUUID(childID);
				if (child.IsValid())
				{
					// �zyinelemeli �a�r�
					DrawEntityInSceneTree(child);
				}
			}
			ImGui::TreePop(); // Sadece TreePush yap�lm��sa TreePop �a�r�l�r
		}

		ImGui::PopID();
	}
}
