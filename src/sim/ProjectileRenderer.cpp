#include "ProjectileRenderer.h"
#include "ProjectileSimulation.h"
#include "../Window.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>
#include <vector>

ProjectileRenderer::ProjectileRenderer(ProjectileSimulation* simulation, Window* window,
                                       unsigned int vao, unsigned int vbo, unsigned int shaderProgram)
    : m_Simulation(simulation)
    , m_Window(window)
    , m_VAO(vao)
    , m_VBO(vbo)
    , m_ShaderProgram(shaderProgram)
{
}

void ProjectileRenderer::Render()
{
    if (!m_Simulation)
        return;

    // Setup orthographic projection
    const float metersToPixels = 20.0f;
    const float cameraOffsetX = 50.0f;
    const float cameraOffsetY = 50.0f;
    
    float width = static_cast<float>(m_Window->Width());
    float height = static_cast<float>(m_Window->Height());
    
    glm::mat4 projection = glm::ortho(0.0f, width, 0.0f, height, -1.0f, 1.0f);
    
    glUseProgram(m_ShaderProgram);
    int projLoc = glGetUniformLocation(m_ShaderProgram, "uProjection");
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
        
        int colorLoc = glGetUniformLocation(m_ShaderProgram, "uColor");
        glUniform3f(colorLoc, 0.3f, 0.3f, 0.3f);
        
        glDrawArrays(GL_LINES, 0, 2);
    }
    
    // Draw target
    {
        glm::vec2 targetPos = m_Simulation->GetTargetPosition();
        float targetRadius = m_Simulation->GetTargetRadius();
        
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
        
        int colorLoc = glGetUniformLocation(m_ShaderProgram, "uColor");
        
        if (m_Simulation->HasHit())
            glUniform3f(colorLoc, 1.0f, 1.0f, 0.0f);  // Yellow when hit
        else
            glUniform3f(colorLoc, 1.0f, 0.0f, 0.0f);  // Red
        
        glDrawArrays(GL_LINE_LOOP, 0, segments + 1);
    }
    
    // Draw trajectory (if enabled)
    if (m_Simulation->GetShowTrajectory())
    {
        const auto& trajectory = m_Simulation->GetTrajectory();
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
            
            int colorLoc = glGetUniformLocation(m_ShaderProgram, "uColor");
            glUniform3f(colorLoc, 0.5f, 0.5f, 1.0f);  // Light blue
            
            glDrawArrays(GL_LINE_STRIP, 0, static_cast<GLsizei>(trajectory.size()));
        }
    }
    
    // Draw arrow
    {
        glm::vec2 pos = m_Simulation->GetPosition();
        glm::vec2 vel = m_Simulation->GetVelocity();
        
        glm::vec2 screenPos = worldToScreen(pos);
        
        float arrowLength = 0.5f * metersToPixels;
        float angle = m_Simulation->GetAngle();
        
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
        
        int colorLoc = glGetUniformLocation(m_ShaderProgram, "uColor");
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
