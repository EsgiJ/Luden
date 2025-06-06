#include "Panels/AssetBrowserPanel.h"
#include "Core/GameEngine.h"
#include "imgui.h"

#include <iostream>


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

               auto& assets = GameEngine::Get().GetAssets();

               auto showAssetMap = [](const char* label, const auto& map)
               {
                       ImGui::TextUnformatted(label);
                       ImGui::Separator();
                       ImGui::Indent();
                       for (const auto& [key, _] : map)
                       {
                               ImGui::Selectable(key.c_str());
                       }
                       ImGui::Unindent();
                       ImGui::Spacing();
               };

               showAssetMap("Textures", assets.GetTextures());
               showAssetMap("Fonts", assets.GetFonts());
               showAssetMap("Sounds", assets.GetSounds());
               showAssetMap("SoundBuffers", assets.GetSoundBuffers());
               showAssetMap("Animations", assets.GetAnimations());

               ImGui::End();
       }
}
