#pragma once

#include <string>

struct GLFWwindow;

class Window
{
public:
    Window();
    ~Window();

    void Init(int width, int height, const std::string& title);
    void PollEvents();
    void SwapBuffers();
    bool ShouldClose() const;

    int Width() const { return m_Width; }
    int Height() const { return m_Height; }

    GLFWwindow* GetGLFWwindow() const { return m_Window; }

private:
    GLFWwindow* m_Window;
    int m_Width;
    int m_Height;
};
