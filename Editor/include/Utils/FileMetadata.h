#pragma once

#include "Audio/SoundBuffer.h"
#include "Graphics/Animation.h"
#include "Graphics/Font.h"
#include "Graphics/Texture.h"
#include "Utils/EditorColors.h"
#include "IO/FileSystem.h"

#include <imgui.h>

#include <string>
#include <typeindex>
#include <unordered_map>

namespace Luden
{
	namespace FileMetadata
	{
		constexpr ImVec4 defaultText = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

		struct FileInfo
		{
			std::string extension;
			std::string placeholder;
			ImVec4 color;
		};

		static const std::unordered_map<ResourceType, FileInfo> FileInfoMap =
		{
			{ResourceType::Texture,		{".png",	"[Texture2D]",	EditorColors::image}},
			{ResourceType::Audio,		{".wav",    "[Sound]",		EditorColors::sound}},
			{ResourceType::Animation,	{".lanim",	"[Animation]",	EditorColors::anim}},
			{ResourceType::Font,		{".ttf",    "[Font]",		EditorColors::font}},
			{ResourceType::Scene,		{".lscene",	"[Scene]",		EditorColors::green}},
			{ResourceType::Prefab,		{".lprefab","[Prefab]",		EditorColors::blue}},
		};

		static const std::unordered_map<std::string, ImVec4> ExtensionColorMap =
		{
			{".png",		EditorColors::image},
			{".wav",		EditorColors::sound},
			{".lanim",		EditorColors::anim},
			{".ttf",		EditorColors::font},
			{".lprefab",	EditorColors::prefab},
			{".lscene",		EditorColors::scene},
			{".lproject",	EditorColors::project}
		};

		inline std::string GetFileExtension(ResourceType type)
		{
			auto it = FileInfoMap.find(type);
			return it != FileInfoMap.end() ? it->second.extension : "";
		}

		inline std::string GetPlaceholder(ResourceType type)
		{
			auto it = FileInfoMap.find(type);
			return it != FileInfoMap.end() ? it->second.placeholder : "";
		}

		inline FileSystem::FileDialogFilterItem GetDialogFilter(ResourceType type)
		{
			static std::string ext = "*" + GetFileExtension(type);
			return { GetPlaceholder(type).c_str(), ext.c_str() };
		}

		inline const ImVec4& GetColor(ResourceType type)
		{
			auto it = FileInfoMap.find(type);
			return it != FileInfoMap.end() ? it->second.color : defaultText;
		}

		// Call ImGui::PopStyleColor() after calling this 
		inline void ColorFileText(ResourceType type)
		{
			ImGui::PushStyleColor(ImGuiCol_Text, GetColor(type));
		}

		inline ImVec4 GetColorFromExtension(const std::string& extension)
		{
			auto it = ExtensionColorMap.find(extension);
			return it != ExtensionColorMap.end() ? it->second : defaultText;
		}

		inline void ColorTextByExtension(const std::string& extension)
		{
			ImVec4 color = GetColorFromExtension(extension);
			ImGui::PushStyleColor(ImGuiCol_Text, color);
		}
	}
}
