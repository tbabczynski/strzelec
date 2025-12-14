#pragma once

#include "IRenderer.h"

class ProjectileSimulation;
class Window;

class ProjectileRenderer : public IRenderer
{
public:
    ProjectileRenderer(ProjectileSimulation* simulation, Window* window, 
                      unsigned int vao, unsigned int vbo, unsigned int shaderProgram);
    ~ProjectileRenderer() override = default;

    void Render() override;

private:
    ProjectileSimulation* m_Simulation;
    Window* m_Window;
    unsigned int m_VAO;
    unsigned int m_VBO;
    unsigned int m_ShaderProgram;
};
