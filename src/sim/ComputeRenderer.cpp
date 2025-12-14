#include "ComputeRenderer.h"
#include "ComputeSimulation.h"

ComputeRenderer::ComputeRenderer(ComputeSimulation* simulation)
    : m_Simulation(simulation)
{
}

void ComputeRenderer::Render()
{
    // ComputeSimulation doesn't currently have visual rendering
    // This is a placeholder for future implementation
}
