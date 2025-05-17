#include "Panels/AssetBrowserPanel.h"

#include "Core/GameEngine.h"

#include <filesystem>
#include <string>

#include "imgui.h"
#include "imgui-SFML.h"

namespace Luden::Editor
{
	void AssetBrowserPanel::Render()
	{
		if (!ImGui::Begin("Assets"))
		{
			ImGui::End();
			return;
		}

		auto& engine = Luden::GameEngine::Get();
		rttr::instance ge_inst = engine;
		rttr::type ge_type = rttr::type::get<Luden::GameEngine>();
		auto assets_prop = ge_type.get_property("m_Assets");

		auto assets_var = assets_prop.get_value(ge_inst);
		rttr::type assets_type = assets_var.get_type();

		for (auto& prop: assets_type.get_properties())
		{
			if (!prop.get_type().is_associative_container())
				continue;

			ImGui::TextUnformatted(prop.get_name().to_string().c_str());
			ImGui::Separator();
			ImGui::Indent();

			auto view = prop.get_value(assets_var).create_associative_view();
			for (auto& item: view)
			{
				auto key_var = item.first;
				std::string key = key_var.get_value<std::string>();

				if (ImGui::Selectable(key.c_str()))
				{
					//TODO
				}
			}
			ImGui::Unindent();
			ImGui::Spacing();
		}

		ImGui::End();
	}
}

