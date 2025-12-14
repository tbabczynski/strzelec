#include "Application.h"
#include "Window.h"
#include "sim/SimulationManager.h"
#include "sim/IRenderer.h"
#include "gui/GUILayer.h"
#include <glad/glad.h>
#include <iostream>

Application::Application()
    : m_Running(false)
    , m_VAO(0)
    , m_VBO(0)
    , m_SimpleShaderProgram(0)
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

    InitRenderingResources();

    m_SimulationManager = std::make_unique<SimulationManager>();
    m_SimulationManager->Init(m_Window.get(), m_VAO, m_VBO, m_SimpleShaderProgram);

    m_GUILayer = std::make_unique<GUILayer>();
    m_GUILayer->Init(m_Window->GetGLFWwindow(), "#version 460 core");

    m_Running = true;

    std::cout << "Application initialized successfully" << std::endl;
}

void Application::Shutdown()
{
    CleanupRenderingResources();

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
    // Background rendering
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.1f, 0.15f, 0.2f, 1.0f);

    // Simulation rendering
    IRenderer* activeRenderer = m_SimulationManager->GetActiveRenderer();
    if (activeRenderer)
    {
        activeRenderer->Render();
    }

    // Controls rendering
    m_GUILayer->NewFrame();
    m_GUILayer->Render(m_SimulationManager.get());
    m_GUILayer->RenderDrawData();

    m_Window->SwapBuffers();
}

void Application::InitRenderingResources()
{
    // Create simple shader for rendering lines and shapes
    const char* vertexShaderSource = R"(
        #version 460 core
        layout (location = 0) in vec2 aPos;
        uniform mat4 uProjection;
        void main()
        {
            gl_Position = uProjection * vec4(aPos, 0.0, 1.0);
        }
    )";

    const char* fragmentShaderSource = R"(
        #version 460 core
        out vec4 FragColor;
        uniform vec3 uColor;
        void main()
        {
            FragColor = vec4(uColor, 1.0);
        }
    )";

    // Compile vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    int success;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        char infoLog[512];
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        std::cerr << "Vertex shader compilation failed:\n" << infoLog << std::endl;
        glDeleteShader(vertexShader);
        return;
    }

    // Compile fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        char infoLog[512];
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        std::cerr << "Fragment shader compilation failed:\n" << infoLog << std::endl;
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return;
    }

    // Link shaders
    m_SimpleShaderProgram = glCreateProgram();
    glAttachShader(m_SimpleShaderProgram, vertexShader);
    glAttachShader(m_SimpleShaderProgram, fragmentShader);
    glLinkProgram(m_SimpleShaderProgram);

    glGetProgramiv(m_SimpleShaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        char infoLog[512];
        glGetProgramInfoLog(m_SimpleShaderProgram, 512, nullptr, infoLog);
        std::cerr << "Shader program linking failed:\n" << infoLog << std::endl;
        glDeleteProgram(m_SimpleShaderProgram);
        m_SimpleShaderProgram = 0;
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return;
    }

    // Shaders are now linked into the program and can be deleted
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Create VAO and VBO
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);

    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    std::cout << "Rendering resources initialized" << std::endl;
}

void Application::CleanupRenderingResources()
{
    if (m_VAO)
    {
        glDeleteVertexArrays(1, &m_VAO);
        m_VAO = 0;
    }
    if (m_VBO)
    {
        glDeleteBuffers(1, &m_VBO);
        m_VBO = 0;
    }
    if (m_SimpleShaderProgram)
    {
        glDeleteProgram(m_SimpleShaderProgram);
        m_SimpleShaderProgram = 0;
    }
}
