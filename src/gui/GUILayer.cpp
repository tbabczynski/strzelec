#include "GUILayer.h"
#include "../sim/SimulationManager.h"
#include "../sim/ISimulation.h"
#include "../sim/ProjectileSimulation.h"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>
#include <iostream>

GUILayer::GUILayer()
    : m_Window(nullptr)
{
}

GUILayer::~GUILayer()
{
}

void GUILayer::Init(GLFWwindow* window, const std::string& glslVersion)
{
    m_Window = window;

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();

    // Enable docking and viewports
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // When viewports are enabled, we tweak WindowRounding/WindowBg so platform windows can look identical
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(m_Window, true);
    ImGui_ImplOpenGL3_Init(glslVersion.c_str());

    std::cout << "GUILayer initialized with GLSL version: " << glslVersion << std::endl;
}

void GUILayer::Shutdown()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void GUILayer::NewFrame()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void GUILayer::Render(SimulationManager* simManager)
{
    // Create a control window
    ImGui::Begin("Controls");

    if (simManager)
    {
        ISimulation* activeSim = simManager->GetActiveSimulation();
        
        // Try to cast to ProjectileSimulation
        ProjectileSimulation* projectileSim = dynamic_cast<ProjectileSimulation*>(activeSim);
        
        if (projectileSim)
        {
            ImGui::Text("Projectile (Archery) Simulation");
            ImGui::Separator();

            // Launch parameters
            ImGui::Text("Launch Parameters:");
            static float speed = 50.0f;
            static float angle = 25.0f;
            ImGui::SliderFloat("Speed (m/s)", &speed, 10.0f, 100.0f);
            ImGui::SliderFloat("Angle (deg)", &angle, 0.0f, 90.0f);

            // Target configuration
            ImGui::Spacing();
            ImGui::Text("Target Configuration:");
            static float targetX = 30.0f;
            static float targetY = 1.5f;
            static float targetRadius = 0.5f;
            if (ImGui::InputFloat("Target X (m)", &targetX) ||
                ImGui::InputFloat("Target Y (m)", &targetY) ||
                ImGui::SliderFloat("Target Radius (m)", &targetRadius, 0.1f, 2.0f))
            {
                projectileSim->SetTarget(glm::vec2(targetX, targetY), targetRadius);
            }

            // Wind configuration
            ImGui::Spacing();
            ImGui::Text("Wind Configuration:");
            static float windX = 0.0f;
            static float windY = 0.0f;
            if (ImGui::SliderFloat("Wind X (m/s)", &windX, -10.0f, 10.0f) ||
                ImGui::SliderFloat("Wind Y (m/s)", &windY, -5.0f, 5.0f))
            {
                projectileSim->SetWind(glm::vec2(windX, windY));
            }

            // Control buttons
            ImGui::Spacing();
            ImGui::Separator();
            if (ImGui::Button("Launch", ImVec2(100, 30)))
            {
                projectileSim->Launch(speed, angle);
            }
            ImGui::SameLine();
            if (ImGui::Button("Reset", ImVec2(100, 30)))
            {
                projectileSim->Reset();
            }

            // Pause/Step controls
            ImGui::Spacing();
            static bool paused = false;
            if (ImGui::Checkbox("Paused", &paused))
            {
                projectileSim->SetPaused(paused);
            }
            ImGui::SameLine();
            if (ImGui::Button("Step"))
            {
                projectileSim->RequestStep();
            }

            // Trajectory toggle
            bool showTrajectory = projectileSim->GetShowTrajectory();
            if (ImGui::Checkbox("Show Trajectory", &showTrajectory))
            {
                projectileSim->SetShowTrajectory(showTrajectory);
            }

            // Status display
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Text("Status:");
            if (projectileSim->IsFlying())
            {
                ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Flying");
                ImGui::Text("Flight time: %.2f s", projectileSim->GetSimTime());
            }
            else if (projectileSim->HasHit())
            {
                ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "HIT TARGET!");
                ImGui::Text("Flight time: %.2f s", projectileSim->GetSimTime());
            }
            else
            {
                ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Ready");
            }

            glm::vec2 pos = projectileSim->GetPosition();
            glm::vec2 vel = projectileSim->GetVelocity();
            ImGui::Text("Position: (%.2f, %.2f) m", pos.x, pos.y);
            ImGui::Text("Velocity: (%.2f, %.2f) m/s", vel.x, vel.y);
            ImGui::Text("Speed: %.2f m/s", glm::length(vel));
        }
        else if (activeSim)
        {
            // Fallback for other simulation types
            ImGui::Text("Compute Simulation Parameters");
            ImGui::Separator();

            float timeStep = activeSim->GetTimeStep();
            if (ImGui::VSliderFloat("##TimeStep", ImVec2(40, 160), &timeStep, 0.001f, 0.1f, "%.3f"))
            {
                activeSim->SetTimeStep(timeStep);
            }
            ImGui::SameLine();
            ImGui::Text("Time Step");

            ImGui::Spacing();
            ImGui::Text("Current timestep: %.4f", timeStep);
        }
        else
        {
            ImGui::Text("No active simulation");
        }
    }

    ImGui::Separator();
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 
                1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

    ImGui::End();

    ImGui::Render();
}

void GUILayer::RenderDrawData()
{
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // Update and Render additional Platform Windows (for multi-viewport support)
    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        GLFWwindow* backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }
}
