#pragma once

#include <memory>
#include <vector>

class ISimulation;

class SimulationManager
{
public:
    SimulationManager();
    ~SimulationManager();

    void Init();
    void UpdateAllParameters();
    void DispatchAll();
    void ResizeAll(int width, int height);

    ISimulation* GetActiveSimulation() const;
    void SetActiveSimulation(int index);

private:
    std::vector<std::unique_ptr<ISimulation>> m_Simulations;
    int m_ActiveSimulationIndex;
};
