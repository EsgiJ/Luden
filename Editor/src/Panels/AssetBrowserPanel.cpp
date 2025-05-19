#include "Panels/AssetBrowserPanel.h"
#include "Core/GameEngine.h"
#include "imgui.h"

#include <iostream>

#include <rttr/instance.h>
#include <rttr/type.h>
#include <rttr/variant.h>

namespace Luden::Editor
{
	void AssetBrowserPanel::Render()
	{
		if (!m_Visible) return;
		if (!ImGui::Begin("Assets", &m_Visible))
		{
			ImGui::End();
			return;
		}

		auto& engine = GameEngine::Get();
		rttr::instance inst = engine;
		rttr::type ge_t = rttr::type::get<decltype(engine)>();

		auto assets_prop = ge_t.get_property("m_Assets");
		rttr::variant assets_var = assets_prop.get_value(inst);

		if (!assets_prop.is_valid())
			std::cerr << "m_Assets property not valid\n";
		if (!assets_var.is_valid())
			std::cerr << "m_Assets value not valid\n";

		rttr::variant assets_ref = assets_var.extract_wrapped_value();
		rttr::instance assets_inst = assets_ref;
		rttr::type assets_t = assets_ref.get_type();

		for (auto prop : assets_t.get_properties())
		{
			if (!prop.get_type().is_associative_container())
				continue;

			ImGui::TextUnformatted(prop.get_name().data());
			ImGui::Separator();
			ImGui::Indent();

			auto view = prop.get_value(assets_inst).create_associative_view();
			for (auto& item : view)
			{
				std::string key = item.first.get_value<std::string>();
				if (ImGui::Selectable(key.c_str()))
				{
					// TODO: handle selecting that asset
				}
			}

			ImGui::Unindent();
			ImGui::Spacing();
		}

		ImGui::End();
	}
}
