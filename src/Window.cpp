#include "Window.h"
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <iostream>
#include <stdexcept>

static void GLFWErrorCallback(int error, const char* description)
{
    std::cerr << "GLFW Error " << error << ": " << description << std::endl;
}

Window::Window()
    : m_Window(nullptr)
    , m_Width(0)
    , m_Height(0)
{
}

Window::~Window()
{
    if (m_Window)
    {
        glfwDestroyWindow(m_Window);
        glfwTerminate();
    }
}

void Window::Init(int width, int height, const std::string& title)
{
    m_Width = width;
    m_Height = height;

    glfwSetErrorCallback(GLFWErrorCallback);

    if (!glfwInit())
    {
        throw std::runtime_error("Failed to initialize GLFW");
    }

    // Request OpenGL 4.6 Core Profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);

    m_Window = glfwCreateWindow(m_Width, m_Height, title.c_str(), nullptr, nullptr);
    if (!m_Window)
    {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window");
    }

    glfwMakeContextCurrent(m_Window);
    glfwSwapInterval(1); // Enable vsync

    // Load OpenGL function pointers with glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        throw std::runtime_error("Failed to initialize GLAD");
    }

    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
}

void Window::PollEvents()
{
    glfwPollEvents();
}

void Window::SwapBuffers()
{
    glfwSwapBuffers(m_Window);
}

bool Window::ShouldClose() const
{
    return glfwWindowShouldClose(m_Window);
}
