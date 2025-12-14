#include "SimulationManager.h"
#include "ISimulation.h"
#include "IRenderer.h"
#include "IControlPanel.h"
#include "SimulationModule.h"
#include "ModuleRegistry.h"
#include "ComputeSimulation.h"
#include "ComputeRenderer.h"
#include "ComputeControlPanel.h"
#include "ProjectileSimulation.h"
#include "ProjectileRenderer.h"
#include "ProjectileControlPanel.h"
#include "../Window.h"
#include <iostream>

SimulationManager::SimulationManager()
    : m_ActiveSimulationIndex(0)
    , m_Window(nullptr)
    , m_VAO(0)
    , m_VBO(0)
    , m_ShaderProgram(0)
{
    m_Registry = std::make_unique<ModuleRegistry>();
}

SimulationManager::~SimulationManager()
{
}

void SimulationManager::RegisterModules()
{
    // Register projectile simulation module factory
    m_Registry->Register("Projectile", [this]() -> std::unique_ptr<SimulationModule> {
        auto module = std::make_unique<SimulationModule>();
        
        auto simulation = std::make_unique<ProjectileSimulation>();
        simulation->Init();
        
        auto renderer = std::make_unique<ProjectileRenderer>(
            simulation.get(), m_Window, m_VAO, m_VBO, m_ShaderProgram);
        
        auto controls = std::make_unique<ProjectileControlPanel>(simulation.get());
        
        module->simulation = std::move(simulation);
        module->renderer = std::move(renderer);
        module->controls = std::move(controls);
        
        return module;
    });

    // Register compute simulation module factory
    m_Registry->Register("Compute", [this]() -> std::unique_ptr<SimulationModule> {
        auto module = std::make_unique<SimulationModule>();
        
        auto simulation = std::make_unique<ComputeSimulation>();
        simulation->Init();
        
        auto renderer = std::make_unique<ComputeRenderer>(simulation.get());
        auto controls = std::make_unique<ComputeControlPanel>(simulation.get());
        
        module->simulation = std::move(simulation);
        module->renderer = std::move(renderer);
        module->controls = std::move(controls);
        
        return module;
    });
}

void SimulationManager::Init(Window* window, unsigned int vao, unsigned int vbo, unsigned int shaderProgram)
{
    m_Window = window;
    m_VAO = vao;
    m_VBO = vbo;
    m_ShaderProgram = shaderProgram;
    
    RegisterModules();
    
    m_ModuleNames = m_Registry->Names();
    
    // Create modules for all registered simulations
    for (const auto& name : m_ModuleNames)
    {
        auto module = m_Registry->Create(name);
        if (module)
        {
            m_Modules.push_back(std::move(module));
        }
    }

    std::cout << "SimulationManager initialized with " << m_Modules.size() << " module(s)" << std::endl;
}

void SimulationManager::UpdateAllParameters()
{
    for (auto& module : m_Modules)
    {
        if (module && module->simulation)
        {
            module->simulation->UpdateParameters();
        }
    }
}

void SimulationManager::DispatchAll()
{
    for (auto& module : m_Modules)
    {
        if (module && module->simulation)
        {
            module->simulation->Dispatch();
        }
    }
}

void SimulationManager::ResizeAll(int width, int height)
{
    for (auto& module : m_Modules)
    {
        if (module && module->simulation)
        {
            module->simulation->Resize(width, height);
        }
    }
}

ISimulation* SimulationManager::GetActiveSimulation() const
{
    if (m_ActiveSimulationIndex >= 0 && static_cast<size_t>(m_ActiveSimulationIndex) < m_Modules.size())
    {
        const auto& module = m_Modules[m_ActiveSimulationIndex];
        return module ? module->simulation.get() : nullptr;
    }
    return nullptr;
}

IRenderer* SimulationManager::GetActiveRenderer() const
{
    if (m_ActiveSimulationIndex >= 0 && static_cast<size_t>(m_ActiveSimulationIndex) < m_Modules.size())
    {
        const auto& module = m_Modules[m_ActiveSimulationIndex];
        return module ? module->renderer.get() : nullptr;
    }
    return nullptr;
}

IControlPanel* SimulationManager::GetActiveControlPanel() const
{
    if (m_ActiveSimulationIndex >= 0 && static_cast<size_t>(m_ActiveSimulationIndex) < m_Modules.size())
    {
        const auto& module = m_Modules[m_ActiveSimulationIndex];
        return module ? module->controls.get() : nullptr;
    }
    return nullptr;
}

void SimulationManager::SetActiveSimulation(int index)
{
    if (index >= 0 && static_cast<size_t>(index) < m_Modules.size())
    {
        m_ActiveSimulationIndex = index;
    }
}

std::vector<std::string> SimulationManager::GetModuleNames() const
{
    return m_ModuleNames;
}
