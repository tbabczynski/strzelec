#pragma once

#include "IRenderer.h"

class ComputeSimulation;

class ComputeRenderer : public IRenderer
{
public:
    explicit ComputeRenderer(ComputeSimulation* simulation);
    ~ComputeRenderer() override = default;

    void Render() override;

private:
    ComputeSimulation* m_Simulation;
};
