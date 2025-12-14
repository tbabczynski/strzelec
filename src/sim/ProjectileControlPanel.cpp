#include "ProjectileControlPanel.h"
#include "ProjectileSimulation.h"
#include <imgui.h>
#include <glm/glm.hpp>

ProjectileControlPanel::ProjectileControlPanel(ProjectileSimulation* simulation)
    : m_Simulation(simulation)
{
}

void ProjectileControlPanel::RenderControls()
{
    if (!m_Simulation)
        return;

    ImGui::Text("Projectile (Archery) Simulation");
    ImGui::Separator();

    // Launch parameters
    ImGui::Text("Launch Parameters:");
    float speed = m_Simulation->GetInitialSpeed();
    float angle = m_Simulation->GetInitialAngle();
    if (ImGui::SliderFloat("Speed (m/s)", &speed, 10.0f, 100.0f))
        m_Simulation->SetInitialSpeed(speed);
    if (ImGui::SliderFloat("Angle (deg)", &angle, 0.0f, 90.0f))
        m_Simulation->SetInitialAngle(angle);

    // Target configuration
    ImGui::Spacing();
    ImGui::Text("Target Configuration:");
    glm::vec2 targetPos = m_Simulation->GetTargetPosition();
    float targetRadius = m_Simulation->GetTargetRadius();
    if (ImGui::InputFloat("Target X (m)", &targetPos.x) ||
        ImGui::InputFloat("Target Y (m)", &targetPos.y) ||
        ImGui::SliderFloat("Target Radius (m)", &targetRadius, 0.1f, 2.0f))
    {
        m_Simulation->SetTarget(targetPos, targetRadius);
    }

    // Wind configuration
    ImGui::Spacing();
    ImGui::Text("Wind Configuration:");
    glm::vec2 wind = m_Simulation->GetWind();
    if (ImGui::SliderFloat("Wind X (m/s)", &wind.x, -100.0f, 100.0f) ||
        ImGui::SliderFloat("Wind Y (m/s)", &wind.y, -100.0f, 100.0f))
    {
        m_Simulation->SetWind(wind);
    }

    // Control buttons
    ImGui::Spacing();
    ImGui::Separator();
    if (ImGui::Button("Launch", ImVec2(100, 30)))
    {
        m_Simulation->Launch(speed, angle);
    }
    ImGui::SameLine();
    if (ImGui::Button("Reset", ImVec2(100, 30)))
    {
        m_Simulation->Reset();
    }

    // Pause/Step controls
    ImGui::Spacing();
    bool paused = m_Simulation->GetPaused();
    if (ImGui::Checkbox("Paused", &paused))
    {
        m_Simulation->SetPaused(paused);
    }
    ImGui::SameLine();
    if (ImGui::Button("Step"))
    {
        m_Simulation->RequestStep();
    }

    // Trajectory toggle
    bool showTrajectory = m_Simulation->GetShowTrajectory();
    if (ImGui::Checkbox("Show Trajectory", &showTrajectory))
    {
        m_Simulation->SetShowTrajectory(showTrajectory);
    }

    // Status display
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Text("Status:");
    if (m_Simulation->IsFlying())
    {
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Flying");
        ImGui::Text("Flight time: %.2f s", m_Simulation->GetSimTime());
    }
    else if (m_Simulation->HasHit())
    {
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "HIT TARGET!");
        ImGui::Text("Flight time: %.2f s", m_Simulation->GetSimTime());
    }
    else
    {
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Ready");
    }

    glm::vec2 pos = m_Simulation->GetPosition();
    glm::vec2 vel = m_Simulation->GetVelocity();
    ImGui::Text("Position: (%.2f, %.2f) m", pos.x, pos.y);
    ImGui::Text("Velocity: (%.2f, %.2f) m/s", vel.x, vel.y);
    ImGui::Text("Speed: %.2f m/s", glm::length(vel));
}
