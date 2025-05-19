#pragma once

#include "EditorAPI.h"

#include <nlohmann/json.hpp>

#include <map>
#include <string>
#include <fstream>

namespace Luden::Editor
{
	enum class EditorMode
	{
		Edit,
		Play,
		Pause
	};

	struct EditorState
	{
		EditorMode m_Mode = EditorMode::Edit;
		std::map<std::string, bool> m_PanelStates;

		static EditorState Load(const std::string& path)
		{
			EditorState state;
			std::ifstream ifs(path);
			if (!ifs.is_open())
				return state;

			nlohmann::json j;

			ifs >> j;
			auto s = j.value("Mode", std::string("Edit"));

			if (s == "Edit")
				state.m_Mode = EditorMode::Edit;
			else if (s == "Play")
				state.m_Mode = EditorMode::Play;
			else if (s == "Pause")
				state.m_Mode = EditorMode::Pause;
			
			if (j.contains("PanelStates") && j["PanelStates"].is_object())
			{
				for (auto& [key, value] : j["PanelStates"].items())
					state.m_PanelStates[key] = value.get<bool>();
			}
			return state;
		}

		void Save(const std::string& path) const 
		{
			nlohmann::json j;
			switch (m_Mode)
			{
			case EditorMode::Edit:
				j["Mode"] = "Edit";
				break;
			case EditorMode::Play:
				j["Mode"] = "Play";
				break;
			case EditorMode::Pause:
				j["Mode"] = "Pause";
				break;
			}
			j["PanelStates"] = m_PanelStates;
			
			std::ofstream(path) << j.dump(4);
		}
	};

	class EDITOR_API EditorStateManager
	{
	public:
		static EditorStateManager& Get();

		EditorMode GetEditorMode();
		void SetEditorMode(EditorMode editorMode);

		bool IsPlayMode() const { return m_State.m_Mode == EditorMode::Play; }
		bool IsEditMode() const { return m_State.m_Mode == EditorMode::Edit; }
		bool IsPaused()   const { return m_State.m_Mode == EditorMode::Pause; }

		EditorState& GetState();
		const EditorState& GetState() const;

	private:
		EditorStateManager() = default;
		EditorState m_State;
	};
}