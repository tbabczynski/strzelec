#include "GUILayer.h"
#include "../sim/SimulationManager.h"
#include "../sim/ISimulation.h"
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

    ImGui::Text("Compute Simulation Parameters");
    ImGui::Separator();

    if (simManager)
    {
        ISimulation* activeSim = simManager->GetActiveSimulation();
        if (activeSim)
        {
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
