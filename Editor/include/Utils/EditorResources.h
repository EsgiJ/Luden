#pragma once

#include "Resource/ResourceManager.h"
#include "IO/FileSystem.h"

#include <SFML/Graphics/Texture.hpp>

#include <filesystem>
#include <iostream>

namespace Luden 
{
	enum class NewProjectType 
	{
		EMPTY,
		//2D Template,
		//3D Template etc.
	};

	class EditorResources 
	{
	public:
		static inline std::shared_ptr<Texture> PlayIcon;
		static inline std::shared_ptr<Texture> StopIcon;
		static inline std::shared_ptr<Texture> PauseIcon;
		static inline std::shared_ptr<Texture> StepIcon;
		static inline std::shared_ptr<Texture> SelectIcon;
		static inline std::shared_ptr<Texture> MoveIcon;
		static inline std::shared_ptr<Texture> ScaleIcon;
		static inline std::shared_ptr<Texture> RotateIcon;
		static inline std::shared_ptr<Texture> BannerIcon;

		static inline std::shared_ptr<Texture> FolderIcon;
		static inline std::shared_ptr<Texture> Anim2DIcon;
		static inline std::shared_ptr<Texture> SceneIcon;
		static inline std::shared_ptr<Texture> AudioIcon;
		static inline std::shared_ptr<Texture> FontIcon;

		static inline std::filesystem::path ProjectTemplate;

		static void Init()
		{
			PlayIcon = LoadTexture("Icons\\play_button.png", "PlayIcon");
			StopIcon = LoadTexture("Icons\\stop_button.png", "StopIcon");
			PauseIcon = LoadTexture("Icons\\pause_button.png", "PauseIcon");
			StepIcon = LoadTexture("Icons\\step_button.png", "StepIcon");
			SelectIcon = LoadTexture("Icons\\tool_select.png", "SelectIcon");
			MoveIcon = LoadTexture("Icons\\tool_move.png", "MoveIcon");
			ScaleIcon = LoadTexture("Icons\\tool_scale.png", "ScaleIcon");
			RotateIcon = LoadTexture("Icons\\tool_rotate.png", "RotateIcon");
			BannerIcon = LoadTexture("Icons\\banner.png", "BannerIcon");

			FolderIcon = LoadTexture("Icons\\folder_icon.png", "FolderIcon");
			Anim2DIcon = LoadTexture("Icons\\anim2d_icon.png", "Anim2DIcon");
			SceneIcon = LoadTexture("Icons\\scene_icon.png", "SceneIcon");
			AudioIcon = LoadTexture("Icons\\audio_icon.png", "AudioIcon");
			FontIcon = LoadTexture("Icons\\font_icon.png", "FontIcon");
		}

	private:
		static std::shared_ptr<Texture> LoadTexture(const std::filesystem::path& relativePath)
		{
			return LoadTexture(relativePath, "");
		}

		static std::shared_ptr<Texture> LoadTexture(const std::filesystem::path& relativePath, const std::string& name)
		{
			std::filesystem::path path = std::filesystem::path("Editor\\Resources") / relativePath;

			if (!FileSystem::Exists(path))
			{
				//TODO: Error
				return nullptr;
			}

			std::shared_ptr<Texture> texture = std::make_shared<Texture>();
			
			if (!texture->GetTexture().loadFromFile(path))
			{
				std::cout << "Unable to load SFTexture" << std::endl;
				return nullptr;
			}
			return texture;
		}
	};

}


