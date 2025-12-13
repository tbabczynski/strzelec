#pragma once

#include <string>
#include <unordered_map>

class Shader
{
public:
    Shader();
    ~Shader();

    void LoadFromFiles(const std::string& vertexPath, const std::string& fragmentPath);
    void LoadComputeShader(const std::string& computePath);
    void Reload();
    void Use() const;

    void SetInt(const std::string& name, int value);
    void SetFloat(const std::string& name, float value);
    void SetVec2(const std::string& name, float x, float y);
    void SetVec3(const std::string& name, float x, float y, float z);
    void SetVec4(const std::string& name, float x, float y, float z, float w);

    unsigned int GetID() const { return m_ProgramID; }

private:
    void CompileShader(unsigned int& shaderID, unsigned int type, const std::string& source);
    void LinkProgram();
    void CheckCompileErrors(unsigned int shader, const std::string& type);
    void CheckLinkErrors();
    std::string ReadFile(const std::string& filepath);
    int GetUniformLocation(const std::string& name);

    unsigned int m_ProgramID;
    unsigned int m_VertexID;
    unsigned int m_FragmentID;
    unsigned int m_ComputeID;

    std::string m_VertexPath;
    std::string m_FragmentPath;
    std::string m_ComputePath;

    bool m_IsComputeShader;
    std::unordered_map<std::string, int> m_UniformLocationCache;
};
