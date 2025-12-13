#include "FullscreenQuad.h"
#include "Shader.h"
#include <glad/glad.h>
#include <iostream>

FullscreenQuad::FullscreenQuad()
    : m_VAO(0)
    , m_VBO(0)
{
}

FullscreenQuad::~FullscreenQuad()
{
    Destroy();
}

bool FullscreenQuad::Init()
{
    // Fullscreen quad vertices: position (xyz) and texcoord (uv)
    float quadVertices[] = {
        // positions        // texCoords
        -1.0f,  1.0f, 0.0f,  0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f,  0.0f, 0.0f,
         1.0f, -1.0f, 0.0f,  1.0f, 0.0f,

        -1.0f,  1.0f, 0.0f,  0.0f, 1.0f,
         1.0f, -1.0f, 0.0f,  1.0f, 0.0f,
         1.0f,  1.0f, 0.0f,  1.0f, 1.0f
    };

    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);

    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    // Position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

    // TexCoord attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

    glBindVertexArray(0);

    std::cout << "FullscreenQuad initialized" << std::endl;
    return true;
}

void FullscreenQuad::Draw(unsigned int texId, std::shared_ptr<Shader> displayShader)
{
    if (!displayShader)
        return;

    displayShader->Use();
    
    // Bind texture to unit 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texId);
    
    // Set uniform
    displayShader->SetInt("uTexture", 0);

    glBindVertexArray(m_VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void FullscreenQuad::Destroy()
{
    if (m_VAO != 0)
    {
        glDeleteVertexArrays(1, &m_VAO);
        m_VAO = 0;
    }
    if (m_VBO != 0)
    {
        glDeleteBuffers(1, &m_VBO);
        m_VBO = 0;
    }
}
