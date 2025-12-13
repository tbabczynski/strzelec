#include "Shader.h"
#include <glad/glad.h>
#include <fstream>
#include <sstream>
#include <iostream>

Shader::Shader()
    : m_ProgramID(0)
    , m_VertexID(0)
    , m_FragmentID(0)
    , m_ComputeID(0)
    , m_IsComputeShader(false)
{
}

Shader::~Shader()
{
    if (m_ProgramID)
        glDeleteProgram(m_ProgramID);
    if (m_VertexID)
        glDeleteShader(m_VertexID);
    if (m_FragmentID)
        glDeleteShader(m_FragmentID);
    if (m_ComputeID)
        glDeleteShader(m_ComputeID);
}

void Shader::LoadFromFiles(const std::string& vertexPath, const std::string& fragmentPath)
{
    m_VertexPath = vertexPath;
    m_FragmentPath = fragmentPath;
    m_IsComputeShader = false;

    std::string vertexSource = ReadFile(vertexPath);
    std::string fragmentSource = ReadFile(fragmentPath);

    CompileShader(m_VertexID, GL_VERTEX_SHADER, vertexSource);
    CompileShader(m_FragmentID, GL_FRAGMENT_SHADER, fragmentSource);

    LinkProgram();
}

void Shader::LoadComputeShader(const std::string& computePath)
{
    m_ComputePath = computePath;
    m_IsComputeShader = true;

    std::string computeSource = ReadFile(computePath);
    CompileShader(m_ComputeID, GL_COMPUTE_SHADER, computeSource);

    LinkProgram();
}

void Shader::Reload()
{
    if (m_ProgramID)
        glDeleteProgram(m_ProgramID);
    if (m_VertexID)
        glDeleteShader(m_VertexID);
    if (m_FragmentID)
        glDeleteShader(m_FragmentID);
    if (m_ComputeID)
        glDeleteShader(m_ComputeID);

    m_ProgramID = 0;
    m_VertexID = 0;
    m_FragmentID = 0;
    m_ComputeID = 0;
    m_UniformLocationCache.clear();

    if (m_IsComputeShader)
    {
        LoadComputeShader(m_ComputePath);
    }
    else
    {
        LoadFromFiles(m_VertexPath, m_FragmentPath);
    }

    std::cout << "Shader reloaded successfully" << std::endl;
}

void Shader::Use() const
{
    glUseProgram(m_ProgramID);
}

void Shader::CompileShader(unsigned int& shaderID, unsigned int type, const std::string& source)
{
    const char* src = source.c_str();
    shaderID = glCreateShader(type);
    glShaderSource(shaderID, 1, &src, nullptr);
    glCompileShader(shaderID);

    std::string typeStr = (type == GL_VERTEX_SHADER) ? "VERTEX" :
                          (type == GL_FRAGMENT_SHADER) ? "FRAGMENT" : "COMPUTE";
    CheckCompileErrors(shaderID, typeStr);
}

void Shader::LinkProgram()
{
    m_ProgramID = glCreateProgram();

    if (m_VertexID)
        glAttachShader(m_ProgramID, m_VertexID);
    if (m_FragmentID)
        glAttachShader(m_ProgramID, m_FragmentID);
    if (m_ComputeID)
        glAttachShader(m_ProgramID, m_ComputeID);

    glLinkProgram(m_ProgramID);
    CheckLinkErrors();

    if (m_VertexID)
        glDetachShader(m_ProgramID, m_VertexID);
    if (m_FragmentID)
        glDetachShader(m_ProgramID, m_FragmentID);
    if (m_ComputeID)
        glDetachShader(m_ProgramID, m_ComputeID);
}

void Shader::CheckCompileErrors(unsigned int shader, const std::string& type)
{
    int success;
    char infoLog[1024];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
        std::cerr << "Shader compilation error (" << type << "):\n" << infoLog << std::endl;
    }
}

void Shader::CheckLinkErrors()
{
    int success;
    char infoLog[1024];
    glGetProgramiv(m_ProgramID, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(m_ProgramID, 1024, nullptr, infoLog);
        std::cerr << "Shader linking error:\n" << infoLog << std::endl;
    }
}

std::string Shader::ReadFile(const std::string& filepath)
{
    std::ifstream file(filepath);
    if (!file.is_open())
    {
        std::cerr << "Failed to open shader file: " << filepath << std::endl;
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

int Shader::GetUniformLocation(const std::string& name)
{
    if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
        return m_UniformLocationCache[name];

    int location = glGetUniformLocation(m_ProgramID, name.c_str());
    m_UniformLocationCache[name] = location;
    return location;
}

void Shader::SetInt(const std::string& name, int value)
{
    glUniform1i(GetUniformLocation(name), value);
}

void Shader::SetFloat(const std::string& name, float value)
{
    glUniform1f(GetUniformLocation(name), value);
}

void Shader::SetVec2(const std::string& name, float x, float y)
{
    glUniform2f(GetUniformLocation(name), x, y);
}

void Shader::SetVec3(const std::string& name, float x, float y, float z)
{
    glUniform3f(GetUniformLocation(name), x, y, z);
}

void Shader::SetVec4(const std::string& name, float x, float y, float z, float w)
{
    glUniform4f(GetUniformLocation(name), x, y, z, w);
}
