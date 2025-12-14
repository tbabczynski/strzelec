#pragma once

#include "IControlPanel.h"

class ComputeSimulation;

class ComputeControlPanel : public IControlPanel
{
public:
    explicit ComputeControlPanel(ComputeSimulation* simulation);
    ~ComputeControlPanel() override = default;

    void RenderControls() override;

private:
    ComputeSimulation* m_Simulation;
};
