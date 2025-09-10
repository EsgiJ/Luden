#include "Core/EditorState.h"

namespace Luden
{
	EditorStateManager& EditorStateManager::Get()
	{
		static EditorStateManager instance;
		return instance;
	}

       EditorMode EditorStateManager::GetEditorMode()
       {
               return m_State.m_Mode;
       }

	void EditorStateManager::SetEditorMode(EditorMode editorMode)
	{
		m_State.m_Mode = editorMode;
	}

       EditorState& EditorStateManager::GetState()
       {
               return m_State;
       }
       const EditorState& EditorStateManager::GetState() const
       {
               return m_State;
       }
}

