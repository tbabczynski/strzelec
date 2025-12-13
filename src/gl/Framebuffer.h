#pragma once

class Framebuffer
{
public:
    Framebuffer();
    ~Framebuffer();

    void Create(int width, int height);
    void Destroy();
    void Bind() const;
    void Unbind() const;
    void Resize(int width, int height);

    unsigned int GetColorTexture() const { return m_ColorTexture; }
    int GetWidth() const { return m_Width; }
    int GetHeight() const { return m_Height; }

private:
    unsigned int m_FBO;
    unsigned int m_ColorTexture;
    unsigned int m_DepthRenderbuffer;
    int m_Width;
    int m_Height;
};
