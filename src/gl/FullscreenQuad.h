#pragma once

#include <memory>

class Shader;

class FullscreenQuad
{
public:
    FullscreenQuad();
    ~FullscreenQuad();

    bool Init();
    void Draw(unsigned int texId, std::shared_ptr<Shader> displayShader);
    void Destroy();

private:
    unsigned int m_VAO;
    unsigned int m_VBO;
};
