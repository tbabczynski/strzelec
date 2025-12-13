#pragma once

#include <memory>

class Window;
class SimulationManager;
class GUILayer;
class Shader;
class FullscreenQuad;

class Application
{
public:
    Application();
    ~Application();

    void Run();

private:
    void Init();
    void Shutdown();
    void Update();
    void Render();

    std::unique_ptr<Window> m_Window;
    std::unique_ptr<SimulationManager> m_SimulationManager;
    std::unique_ptr<GUILayer> m_GUILayer;
    std::shared_ptr<Shader> m_DisplayShader;
    std::unique_ptr<FullscreenQuad> m_FullscreenQuad;

    bool m_Running;
    unsigned int m_FrameCount;
};
