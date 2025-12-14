#include "ComputeControlPanel.h"
#include "ComputeSimulation.h"
#include <imgui.h>

ComputeControlPanel::ComputeControlPanel(ComputeSimulation* simulation)
    : m_Simulation(simulation)
{
}

void ComputeControlPanel::RenderControls()
{
    if (!m_Simulation)
        return;

    ImGui::Text("Compute Simulation Parameters");
    ImGui::Separator();

    float timeStep = m_Simulation->GetTimeStep();
    if (ImGui::VSliderFloat("##TimeStep", ImVec2(40, 160), &timeStep, 0.001f, 0.1f, "%.3f"))
    {
        m_Simulation->SetTimeStep(timeStep);
    }
    ImGui::SameLine();
    ImGui::Text("Time Step");

    ImGui::Spacing();
    ImGui::Text("Current timestep: %.4f", timeStep);
}
