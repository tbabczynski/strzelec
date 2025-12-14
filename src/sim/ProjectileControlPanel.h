#pragma once

#include "IControlPanel.h"

class ProjectileSimulation;

class ProjectileControlPanel : public IControlPanel
{
public:
    explicit ProjectileControlPanel(ProjectileSimulation* simulation);
    ~ProjectileControlPanel() override = default;

    void RenderControls() override;

private:
    ProjectileSimulation* m_Simulation;
};
