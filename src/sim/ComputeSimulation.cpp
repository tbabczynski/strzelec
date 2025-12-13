#include "ComputeSimulation.h"
#include "../gl/Shader.h"
#include <glad/glad.h>
#include <iostream>
#include <vector>

ComputeSimulation::ComputeSimulation()
    : m_TexA(0)
    , m_TexB(0)
    , m_SrcTex(0)
    , m_DstTex(0)
    , m_Width(256)
    , m_Height(256)
    , m_TimeStep(0.016f)
    , m_Frame(0)
{
}

ComputeSimulation::~ComputeSimulation()
{
    DestroyTextures();
}

void ComputeSimulation::CreateTextures(int width, int height)
{
    // Destroy existing textures if any
    DestroyTextures();

    m_Width = width;
    m_Height = height;

    // Create initial data with center red spot
    std::vector<float> initialData(m_Width * m_Height * 4, 0.0f);
    
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
                int idx = (iy * m_Width + ix) * 4;
                initialData[idx + 0] = 1.0f; // Red channel
                initialData[idx + 1] = 0.0f; // Green
                initialData[idx + 2] = 0.0f; // Blue
                initialData[idx + 3] = 1.0f; // Alpha
            }
        }
    }

    // Create texture A
    glGenTextures(1, &m_TexA);
    glBindTexture(GL_TEXTURE_2D, m_TexA);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, m_Width, m_Height);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_Width, m_Height, GL_RGBA, GL_FLOAT, initialData.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Create texture B (initially empty)
    glGenTextures(1, &m_TexB);
    glBindTexture(GL_TEXTURE_2D, m_TexB);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, m_Width, m_Height);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_2D, 0);

    // Initialize ping-pong: src = A, dst = B
    m_SrcTex = m_TexA;
    m_DstTex = m_TexB;

    std::cout << "ComputeSimulation textures created: " << m_Width << "x" << m_Height << std::endl;
}

void ComputeSimulation::DestroyTextures()
{
    // Reset pointers before deleting textures to avoid dangling pointers
    m_SrcTex = 0;
    m_DstTex = 0;

    if (m_TexA != 0)
    {
        glDeleteTextures(1, &m_TexA);
        m_TexA = 0;
    }
    if (m_TexB != 0)
    {
        glDeleteTextures(1, &m_TexB);
        m_TexB = 0;
    }
}

void ComputeSimulation::Init()
{
    m_ComputeShader = std::make_unique<Shader>();
    m_ComputeShader->LoadComputeShader("shaders/compute_diffusion.comp.glsl");

    CreateTextures(m_Width, m_Height);

    std::cout << "ComputeSimulation initialized: " << m_Width << "x" << m_Height << std::endl;
}

void ComputeSimulation::UpdateParameters()
{
    m_ComputeShader->Use();
    m_ComputeShader->SetFloat("uTimeStep", m_TimeStep);
    m_ComputeShader->SetInt("uWidth", m_Width);
    m_ComputeShader->SetInt("uHeight", m_Height);
}

void ComputeSimulation::Dispatch()
{
    m_ComputeShader->Use();

    // Set uniforms
    m_ComputeShader->SetFloat("uTimeStep", m_TimeStep);
    m_ComputeShader->SetInt("uWidth", m_Width);
    m_ComputeShader->SetInt("uHeight", m_Height);

    // Bind images for compute shader
    glBindImageTexture(0, m_SrcTex, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
    glBindImageTexture(1, m_DstTex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

    // Dispatch compute shader (16x16 local size)
    int numGroupsX = (m_Width + 15) / 16;
    int numGroupsY = (m_Height + 15) / 16;
    glDispatchCompute(numGroupsX, numGroupsY, 1);

    // Memory barrier to ensure writes complete before next read
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);

    // Swap textures for ping-pong
    unsigned int temp = m_SrcTex;
    m_SrcTex = m_DstTex;
    m_DstTex = temp;

    m_Frame++;
}

void ComputeSimulation::Resize(int width, int height)
{
    if (width > 0 && height > 0)
    {
        CreateTextures(width, height);
        std::cout << "ComputeSimulation resized to: " << width << "x" << height << std::endl;
    }
}
