#include "Application.h"
#include "Window.h"
#include "sim/SimulationManager.h"
#include "sim/ComputeSimulation.h"
#include "gui/GUILayer.h"
#include "gl/Shader.h"
#include "gl/FullscreenQuad.h"
#include <glad/glad.h>
#include <iostream>

Application::Application()
    : m_Running(false)
    , m_FrameCount(0)
{
    Init();
}

Application::~Application()
{
    Shutdown();
}

void Application::Init()
{
    m_Window = std::make_unique<Window>();
    m_Window->Init(1280, 720, "Strzelec - OpenGL 4.6 Compute Simulation");

    m_SimulationManager = std::make_unique<SimulationManager>();
    m_SimulationManager->Init();

    // Create display shader for fullscreen quad
    m_DisplayShader = std::make_shared<Shader>();
    m_DisplayShader->LoadFromFiles("shaders/fullscreen_quad.vert.glsl", "shaders/display.frag.glsl");

    // Create fullscreen quad
    m_FullscreenQuad = std::make_unique<FullscreenQuad>();
    m_FullscreenQuad->Init();

    m_GUILayer = std::make_unique<GUILayer>();
    m_GUILayer->Init(m_Window->GetGLFWwindow(), "#version 460 core");

    m_Running = true;

    std::cout << "Application initialized successfully" << std::endl;
}

void Application::Shutdown()
{
    if (m_GUILayer)
        m_GUILayer->Shutdown();

    m_FullscreenQuad.reset();
    m_DisplayShader.reset();
    m_SimulationManager.reset();
    m_Window.reset();

    std::cout << "Application shutdown complete" << std::endl;
}

void Application::Run()
{
    while (m_Running && !m_Window->ShouldClose())
    {
        Update();
        Render();
    }
}

void Application::Update()
{
    m_Window->PollEvents();

    m_SimulationManager->UpdateAllParameters();
    m_SimulationManager->DispatchAll();
    
    m_FrameCount++;
}

void Application::Render()
{
    // Clear the framebuffer
    glViewport(0, 0, m_Window->Width(), m_Window->Height());
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Render fullscreen quad with simulation texture
    ComputeSimulation* activeSim = dynamic_cast<ComputeSimulation*>(m_SimulationManager->GetActiveSimulation());
    if (activeSim)
    {
        unsigned int simTexture = activeSim->GetTexture();
        m_FullscreenQuad->Draw(simTexture, m_DisplayShader);
    }

    // Render GUI on top
    m_GUILayer->NewFrame();
    m_GUILayer->Render(m_SimulationManager.get());
    m_GUILayer->RenderDrawData();

    m_Window->SwapBuffers();
}
