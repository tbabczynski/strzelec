#pragma once

#include <string>

struct GLFWwindow;
class SimulationManager;

class GUILayer
{
public:
    GUILayer();
    ~GUILayer();

    void Init(GLFWwindow* window, const std::string& glslVersion);
    void Shutdown();
    void NewFrame();
    void Render(SimulationManager* simManager);
    void RenderDrawData();

private:
    GLFWwindow* m_Window;
};
