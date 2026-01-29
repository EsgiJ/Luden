#pragma once
#include <string>

namespace Luden
{
    class Scene;

    class Application
    {
    public:
        virtual ~Application() = default;

        virtual void ChangeScene(const std::string& sceneName) = 0;
        virtual void ReloadCurrentScene() = 0;
        virtual Scene* GetCurrentScene() = 0;
    };
}