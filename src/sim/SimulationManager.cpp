#include "SimulationManager.h"
#include "ISimulation.h"
#include "ComputeSimulation.h"
#include <iostream>

SimulationManager::SimulationManager()
    : m_ActiveSimulationIndex(0)
{
}

SimulationManager::~SimulationManager()
{
}

void SimulationManager::Init()
{
    // Create and initialize compute simulation
    auto computeSim = std::make_unique<ComputeSimulation>();
    computeSim->Init();
    m_Simulations.push_back(std::move(computeSim));

    std::cout << "SimulationManager initialized with " << m_Simulations.size() << " simulation(s)" << std::endl;
}

void SimulationManager::UpdateAllParameters()
{
    for (auto& sim : m_Simulations)
    {
        sim->UpdateParameters();
    }
}

void SimulationManager::DispatchAll()
{
    for (auto& sim : m_Simulations)
    {
        sim->Dispatch();
    }
}

void SimulationManager::ResizeAll(int width, int height)
{
    for (auto& sim : m_Simulations)
    {
        sim->Resize(width, height);
    }
}

ISimulation* SimulationManager::GetActiveSimulation() const
{
    if (m_ActiveSimulationIndex >= 0 && static_cast<size_t>(m_ActiveSimulationIndex) < m_Simulations.size())
    {
        return m_Simulations[m_ActiveSimulationIndex].get();
    }
    return nullptr;
}

void SimulationManager::SetActiveSimulation(int index)
{
    if (index >= 0 && static_cast<size_t>(index) < m_Simulations.size())
    {
        m_ActiveSimulationIndex = index;
    }
}
