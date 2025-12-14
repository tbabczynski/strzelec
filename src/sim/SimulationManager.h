#pragma once

#include <memory>
#include <vector>
#include <string>

class ISimulation;
class IRenderer;
class IControlPanel;
struct SimulationModule;
class ModuleRegistry;
class Window;

class SimulationManager
{
public:
    SimulationManager();
    ~SimulationManager();

    void Init(Window* window, unsigned int vao, unsigned int vbo, unsigned int shaderProgram);
    void UpdateAllParameters();
    void DispatchAll();
    void ResizeAll(int width, int height);

    ISimulation* GetActiveSimulation() const;
    IRenderer* GetActiveRenderer() const;
    IControlPanel* GetActiveControlPanel() const;
    void SetActiveSimulation(int index);
    
    std::vector<std::string> GetModuleNames() const;

private:
    void RegisterModules();

    int m_ActiveSimulationIndex;
    
    // Resources needed for module creation
    Window* m_Window;
    unsigned int m_VAO;
    unsigned int m_VBO;
    unsigned int m_ShaderProgram;
    
    std::unique_ptr<ModuleRegistry> m_Registry;
    std::vector<std::unique_ptr<SimulationModule>> m_Modules;
    std::vector<std::string> m_ModuleNames;
};
