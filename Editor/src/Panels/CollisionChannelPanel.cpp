#include "Panels/CollisionChannelPanel.h"

#include <imgui.h>
#include <IconsFontAwesome7.h>
#include "Project/Project.h"

namespace Luden
{
	void CollisionChannelPanel::RenderContent()
	{
		auto& registry = CollisionChannelRegistry::Instance();

		if (ImGui::CollapsingHeader(ICON_FA_PLUS " Add New Channel", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::Indent();

			ImGui::InputText("Name", m_NewChannelName, sizeof(m_NewChannelName));

			if (ImGui::Button(ICON_FA_CHECK " Create Channel"))
			{	
				if (strlen(m_NewChannelName) > 0)
				{
					if (registry.RegisterCollisionChannel(m_NewChannelName))
					{
						m_NewChannelName[0] = '\0';
					}
				}
			}

			ImGui::Unindent();
		}

		ImGui::Separator();

		// List all channels
		if (ImGui::CollapsingHeader(ICON_FA_LIST " Registered Channels", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::Indent();

			if (ImGui::BeginTable("ChannelsTable", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
			{
				ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed, 150.0f);
				ImGui::TableSetupColumn("Bit", ImGuiTableColumnFlags_WidthFixed, 80.0f);
				ImGui::TableSetupColumn("Actions", ImGuiTableColumnFlags_WidthFixed, 80.0f);
				ImGui::TableHeadersRow();

				for (const auto& channel : registry.GetAllChannels())
				{
					ImGui::TableNextRow();

					// Name
					ImGui::TableSetColumnIndex(0);
					ImGui::Text("%s", channel.Name.c_str());

					// Bit (hex)
					ImGui::TableSetColumnIndex(1);
					ImGui::Text("0x%08X", channel.Bit);

					// Actions
					ImGui::TableSetColumnIndex(3);
					ImGui::PushID(channel.Name.c_str());

					if (ImGui::Button(ICON_FA_TRASH))
					{
						m_SelectedChannel = channel.Name;
						ImGui::OpenPopup("ConfirmDelete");
					}

					ImGui::PopID();
				}

				ImGui::EndTable();
			}

			ImGui::Unindent();
		}

		// Confirm delete popup
		if (ImGui::BeginPopupModal("ConfirmDelete", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("Are you sure you want to delete channel '%s'?", m_SelectedChannel.c_str());
			ImGui::Separator();

			if (ImGui::Button("Yes", ImVec2(120, 0)))
			{
				registry.UnregisterCollisionChannel(m_SelectedChannel);
				m_SelectedChannel = "";
				ImGui::CloseCurrentPopup();
			}

			ImGui::SameLine();

			if (ImGui::Button("No", ImVec2(120, 0)))
			{
				m_SelectedChannel = "";
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}

		ImGui::Separator();

		// Utility buttons
		if (ImGui::Button(ICON_FA_ROTATE_LEFT " Reset to Defaults"))
		{
			registry.ResetToDefaults();
		}

		ImGui::SameLine();

		if (ImGui::Button(ICON_FA_FILE " Save"))
		{
			std::filesystem::path out = Project::GetActiveResourceDirectory();
			out /= "CollisionChannels.dat";
			registry.Serialize(out);
		}

		ImGui::SameLine();

		if (ImGui::Button(ICON_FA_FOLDER_OPEN " Load"))
		{
			std::filesystem::path in = Project::GetActiveResourceDirectory();
			in /= "CollisionChannels.dat";
			registry.Deserialize("CollisionChannels.dat");
		}
	}
}