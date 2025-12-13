#include "ComputeSimulation.h"
#include "../gl/Shader.h"
#include "../gl/GLBuffer.h"
#include <glad/glad.h>
#include <iostream>

ComputeSimulation::ComputeSimulation()
    : m_Width(256)
    , m_Height(256)
    , m_TimeStep(0.016f)
    , m_Frame(0)
{
}

ComputeSimulation::~ComputeSimulation()
{
}

void ComputeSimulation::Init()
{
    m_ComputeShader = std::make_unique<Shader>();
    m_ComputeShader->LoadComputeShader("shaders/compute_diffusion.comp.glsl");

    // Create SSBO for state data (256x256 floats for red channel)
    size_t dataSize = m_Width * m_Height * sizeof(float);
    std::vector<float> initialData(m_Width * m_Height, 0.0f);

    // Initialize with a simple pattern (central hot spot)
    int centerX = m_Width / 2;
    int centerY = m_Height / 2;
    for (int y = -10; y <= 10; ++y)
    {
        for (int x = -10; x <= 10; ++x)
        {
            int ix = centerX + x;
            int iy = centerY + y;
            if (ix >= 0 && ix < m_Width && iy >= 0 && iy < m_Height)
            {
                int idx = iy * m_Width + ix;
                initialData[idx] = 1.0f;
            }
        }
    }

    m_StateBuffer = std::make_unique<GLBuffer>();
    m_StateBuffer->Create(dataSize, initialData.data(), GL_SHADER_STORAGE_BUFFER, GL_DYNAMIC_DRAW);

    std::cout << "ComputeSimulation initialized: " << m_Width << "x" << m_Height << std::endl;
}

void ComputeSimulation::UpdateParameters()
{
    m_ComputeShader->Use();
    m_ComputeShader->SetFloat("uTimeStep", m_TimeStep);
    m_ComputeShader->SetInt("uFrame", m_Frame);
}

void ComputeSimulation::Dispatch()
{
    m_ComputeShader->Use();

    // Bind SSBO to binding point 0
    m_StateBuffer->BindBase(GL_SHADER_STORAGE_BUFFER, 0);

    // Dispatch compute shader (16x16 local size, 256x256 grid)
    int numGroupsX = (m_Width + 15) / 16;
    int numGroupsY = (m_Height + 15) / 16;
    glDispatchCompute(numGroupsX, numGroupsY, 1);

    // Memory barrier to ensure writes complete before next read
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);

    m_Frame++;
}

void ComputeSimulation::Resize(int width, int height)
{
    // For this simple example, we keep the simulation resolution fixed
    // In a more advanced version, you might recreate the buffer here
    std::cout << "ComputeSimulation resize requested: " << width << "x" << height 
              << " (keeping internal resolution " << m_Width << "x" << m_Height << ")" << std::endl;
}
