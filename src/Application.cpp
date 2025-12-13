#include "Application.h"
#include "Window.h"
#include "sim/SimulationManager.h"
#include "sim/ProjectileSimulation.h"
#include "gui/GUILayer.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <cmath>
#include <vector>

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

    m_SimulationManager = std::make_unique<SimulationManager>();
    m_SimulationManager->Init();

    m_GUILayer = std::make_unique<GUILayer>();
    m_GUILayer->Init(m_Window->GetGLFWwindow(), "#version 460 core");

    InitRenderingResources();

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
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.1f, 0.15f, 0.2f, 1.0f);

    // Render projectile scene
    RenderProjectileScene();

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
    }

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

void Application::RenderProjectileScene()
{
    // Check if we have a projectile simulation
    ISimulation* activeSim = m_SimulationManager->GetActiveSimulation();
    ProjectileSimulation* projectileSim = dynamic_cast<ProjectileSimulation*>(activeSim);
    
    if (!projectileSim)
        return;

    // Setup orthographic projection
    // Coordinate system: origin at bottom-left, meters to pixels conversion
    const float metersToPixels = 20.0f;  // 20 pixels per meter
    const float cameraOffsetX = 50.0f;   // Camera offset in pixels from left
    const float cameraOffsetY = 50.0f;   // Camera offset in pixels from bottom
    
    float width = static_cast<float>(m_Window->Width());
    float height = static_cast<float>(m_Window->Height());
    
    // Create orthographic projection matrix
    glm::mat4 projection = glm::ortho(0.0f, width, 0.0f, height, -1.0f, 1.0f);
    
    glUseProgram(m_SimpleShaderProgram);
    int projLoc = glGetUniformLocation(m_SimpleShaderProgram, "uProjection");
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
    
    glBindVertexArray(m_VAO);
    
    // Helper lambda to convert world coordinates (meters) to screen coordinates (pixels)
    auto worldToScreen = [&](const glm::vec2& worldPos) -> glm::vec2 {
        return glm::vec2(
            cameraOffsetX + worldPos.x * metersToPixels,
            cameraOffsetY + worldPos.y * metersToPixels
        );
    };
    
    // Draw ground line
    {
        std::vector<float> groundVertices = {
            0.0f, cameraOffsetY,
            width, cameraOffsetY
        };
        
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferData(GL_ARRAY_BUFFER, groundVertices.size() * sizeof(float), groundVertices.data(), GL_DYNAMIC_DRAW);
        
        int colorLoc = glGetUniformLocation(m_SimpleShaderProgram, "uColor");
        glUniform3f(colorLoc, 0.3f, 0.3f, 0.3f);
        
        glDrawArrays(GL_LINES, 0, 2);
    }
    
    // Draw target
    {
        glm::vec2 targetPos = projectileSim->GetTargetPosition();
        float targetRadius = projectileSim->GetTargetRadius();
        
        glm::vec2 screenTarget = worldToScreen(targetPos);
        float screenRadius = targetRadius * metersToPixels;
        
        // Create circle vertices
        const int segments = 32;
        std::vector<float> circleVertices;
        for (int i = 0; i <= segments; ++i)
        {
            float angle = 2.0f * glm::pi<float>() * float(i) / float(segments);
            float x = screenTarget.x + screenRadius * std::cos(angle);
            float y = screenTarget.y + screenRadius * std::sin(angle);
            circleVertices.push_back(x);
            circleVertices.push_back(y);
        }
        
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferData(GL_ARRAY_BUFFER, circleVertices.size() * sizeof(float), circleVertices.data(), GL_DYNAMIC_DRAW);
        
        int colorLoc = glGetUniformLocation(m_SimpleShaderProgram, "uColor");
        
        if (projectileSim->HasHit())
            glUniform3f(colorLoc, 1.0f, 1.0f, 0.0f);  // Yellow when hit
        else
            glUniform3f(colorLoc, 1.0f, 0.0f, 0.0f);  // Red
        
        glDrawArrays(GL_LINE_LOOP, 0, segments + 1);
    }
    
    // Draw trajectory (if enabled)
    if (projectileSim->GetShowTrajectory())
    {
        const auto& trajectory = projectileSim->GetTrajectory();
        if (trajectory.size() > 1)
        {
            std::vector<float> trajVertices;
            for (const auto& point : trajectory)
            {
                glm::vec2 screenPos = worldToScreen(point);
                trajVertices.push_back(screenPos.x);
                trajVertices.push_back(screenPos.y);
            }
            
            glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
            glBufferData(GL_ARRAY_BUFFER, trajVertices.size() * sizeof(float), trajVertices.data(), GL_DYNAMIC_DRAW);
            
            int colorLoc = glGetUniformLocation(m_SimpleShaderProgram, "uColor");
            glUniform3f(colorLoc, 0.5f, 0.5f, 1.0f);  // Light blue
            
            glDrawArrays(GL_LINE_STRIP, 0, static_cast<GLsizei>(trajectory.size()));
        }
    }
    
    // Draw arrow
    {
        glm::vec2 pos = projectileSim->GetPosition();
        glm::vec2 vel = projectileSim->GetVelocity();
        
        glm::vec2 screenPos = worldToScreen(pos);
        
        // Arrow representation: a line showing direction and magnitude
        float arrowLength = 0.5f * metersToPixels;  // 0.5 meters in pixels
        float angle = projectileSim->GetAngle();
        
        glm::vec2 arrowEnd = screenPos + glm::vec2(
            arrowLength * std::cos(angle),
            arrowLength * std::sin(angle)
        );
        
        std::vector<float> arrowVertices = {
            screenPos.x, screenPos.y,
            arrowEnd.x, arrowEnd.y
        };
        
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferData(GL_ARRAY_BUFFER, arrowVertices.size() * sizeof(float), arrowVertices.data(), GL_DYNAMIC_DRAW);
        
        int colorLoc = glGetUniformLocation(m_SimpleShaderProgram, "uColor");
        glUniform3f(colorLoc, 0.0f, 1.0f, 0.0f);  // Green
        
        glLineWidth(3.0f);
        glDrawArrays(GL_LINES, 0, 2);
        glLineWidth(1.0f);
        
        // Draw arrow point (small circle)
        const int pointSegments = 8;
        float pointRadius = 5.0f;
        std::vector<float> pointVertices;
        for (int i = 0; i <= pointSegments; ++i)
        {
            float a = 2.0f * glm::pi<float>() * float(i) / float(pointSegments);
            float x = screenPos.x + pointRadius * std::cos(a);
            float y = screenPos.y + pointRadius * std::sin(a);
            pointVertices.push_back(x);
            pointVertices.push_back(y);
        }
        
        glBufferData(GL_ARRAY_BUFFER, pointVertices.size() * sizeof(float), pointVertices.data(), GL_DYNAMIC_DRAW);
        glDrawArrays(GL_TRIANGLE_FAN, 0, pointSegments + 1);
    }
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glUseProgram(0);
}
