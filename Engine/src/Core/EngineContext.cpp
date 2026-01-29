#include "Core/EngineContext.h"

namespace Luden
{
    void EngineContext::RequestSceneChange(const std::string& sceneName)
    {
        m_PendingSceneChange = true;
        m_PendingSceneReload = false;
        m_PendingSceneName = sceneName;
    }

    void EngineContext::RequestSceneReload()
    {
        m_PendingSceneReload = true;
        m_PendingSceneChange = false;
    }
}
