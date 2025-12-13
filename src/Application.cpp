#include "Application.h"
#include "Window.h"
#include "sim/SimulationManager.h"
#include "gui/GUILayer.h"
#include <iostream>

Application::Application()
    : m_Running(false)
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

    m_GUILayer = std::make_unique<GUILayer>();
    m_GUILayer->Init(m_Window->GetGLFWwindow(), "#version 460 core");

    m_Running = true;

    std::cout << "Application initialized successfully" << std::endl;
}

void Application::Shutdown()
{
    if (m_GUILayer)
        m_GUILayer->Shutdown();

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
}

void Application::Render()
{
    m_GUILayer->NewFrame();

    m_GUILayer->Render(m_SimulationManager.get());

    m_GUILayer->RenderDrawData();

    m_Window->SwapBuffers();
}
